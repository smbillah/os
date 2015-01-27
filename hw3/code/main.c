#include "xjob.h"

/*global variables*/
static int status = 0;
static int job_id = 0;

struct producer_consumer_args args;
struct buf buf;

asmlinkage extern long (*sysptr)(void *arg, int length);

/* returns true during the lifetime of this module */
int is_running(void){
    return status;
}

/**
* @args element to insert 
* return 1 on success, 0 otherwise
*/
void get_random(int * rand){
    get_random_bytes((void *)rand, sizeof(int));
    *rand = (int)*rand < 0 ? - *rand : *rand;
    *rand %= FIFO_SIZE;        
    return;
}


int construct_job(struct input * input, struct job ** job){
    int err = 0;
    int id=0;
    *job = kmalloc(sizeof(struct job), GFP_KERNEL);
    if(!*job) {
        err = -ENOMEM;
        goto out;
    }    
    
    //populate job struct
    (*job)->status = JOB_WAITING;
    (*job)->input = input;    
    (*job)->input->job_id = id = job_id++;
    
    return id;
out:
    return err;    
}


/*
* dispatcher: dispatch a newly incoming job to buffer
* @job : incoming job request
* return: 0 on success, -ve otherwise
*/
int dispatch(struct job *job)
{        
    int err = 0;        
            
    /* adding task to buffer */        
    if(buf.len >= BUF_SIZE){ //1. buffer is full        
        printk("@buf: BUFFER is full... sending EBUSY\n");
        return -EBUSY;
    }            
    else if(buf.len == 0){ // 2. buffer was empty
        //err = buf_enqueue(&buf, job);
        err = buf_enqueue_ref(&buf, job);
        if(err) return -1;                
        printk("@buf: add new job:%d ...notifying consumer\n", job->input->job_id);        
        
        //notify consumer
        complete(&args.comp);
    }
    else{ //regular operation        
        err = buf_enqueue_ref(&buf, job);
        if(err) return -1;                
        printk("@buf: add  job:%d, current buf_size:%d\n", job->input->job_id, buf.len);            
        
        //if(buf.len > FIFO_SIZE) complete(&args.comp);
    }    
    return err;
}


int signal_a_process(pid_t usr_pid, int code){
    int err;
    struct task_struct *t;    
    struct siginfo info;  
    
    memset(&info, 0, sizeof(struct siginfo));
    info.si_signo = SIG_TEST;
	info.si_code = SI_QUEUE;
    info.si_int = code;
    
    rcu_read_lock();
    t = pid_task(find_vpid(usr_pid), PIDTYPE_PID);	
	if(t == NULL){
		printk("no such pid\n");
		rcu_read_unlock();
		return -ENODEV;
	}
    
	rcu_read_unlock();
	err = send_sig_info(SIG_TEST, &info, t);    //send the signal
	if (err < 0) {
		printk("error sending signal\n");		
	}
    return err;
}


void destroy_input(struct input *data){    
    if(data == NULL)
        return;
    
    if(data->out_file != NULL)           
        putname(data->out_file);
       
    if(data->in_file != NULL)            
        putname(data->in_file);    

    if(data->scheme != NULL)            
        putname(data->scheme);    
	
    kfree(data);
    return;
}


bool construct_input(struct input *data, struct input *temp) 
{
    if(temp->out_file != NULL){    
        data->out_file = getname(temp->out_file);
        if(data->out_file == NULL)
            return false;        
    }
    else
        return false;

    if(temp->in_file != NULL){    
        data->in_file = getname(temp->in_file);
        if(data->in_file == NULL)
            return false;
    }
    else
        return false;
    
    if( data->operation == ENCRYPTION || data->operation == DECRYPTION){    
        if(temp->scheme != NULL){
            data->scheme = getname(temp->scheme);        
        }
        else
            return false;        
    }else{
        data->scheme = NULL;
    }
    
    return true;
}


int output_file_access(struct input *data)
{
    struct file *f2 = NULL;
    int errorno = 0, flag = 0;

    //f2 = filp_open(data->out_file, O_WRONLY|O_CREAT|O_TRUNC, 644);
    f2 = filp_open(data->out_file, O_WRONLY, 0);
	if(f2 == NULL || IS_ERR(f2))
	{
		errorno = PTR_ERR(f2);
		printk(KERN_INFO "Output File Error: %d\n", errorno);
		goto out;
	}

    /*If the file exist then check the permission for file accessibility*/    
    if(get_current_cred()->uid == f2->f_dentry->d_inode->i_uid)
        if((f2->f_dentry->d_inode->i_mode & S_IWUSR) == S_IWUSR)
            flag = 1;
    if(get_current_cred()->gid == f2->f_dentry->d_inode->i_gid)
        if((f2->f_dentry->d_inode->i_mode & S_IWGRP) == S_IWGRP)
            flag = 1;
    if((f2->f_dentry->d_inode->i_mode & S_IWOTH) == S_IWOTH)
        flag = 1;

    if(flag == 0)
    {
        printk(KERN_INFO "UID-GID-S_IWOTH did not Match\n");
        errorno = -EACCES;
    }    

out:
    if(f2 != NULL && !IS_ERR(f2))
            filp_close(f2, NULL);

    return errorno;
}


int input_file_access(struct input *data)
{
    struct file *f1 = NULL;
    int errorno = 0, flag = 0;

    f1 = filp_open(data->in_file, O_RDONLY, 0);
	if(f1 == NULL || IS_ERR(f1))
	{
		errorno = PTR_ERR(f1);
		printk(KERN_INFO "Input File Error: %d\n", errorno);
		goto out;
	}

    /*If the file exist then check the permission for file accessibility*/
    if(get_current_cred()->uid == f1->f_dentry->d_inode->i_uid)
        if((f1->f_dentry->d_inode->i_mode & S_IWUSR) == S_IWUSR)
            flag = 1;
    
    if(get_current_cred()->gid == f1->f_dentry->d_inode->i_gid)
        if((f1->f_dentry->d_inode->i_mode & S_IWGRP) == S_IWGRP)
            flag = 1;
    
    if((f1->f_dentry->d_inode->i_mode & S_IWOTH) == S_IWOTH)
        flag = 1;

    if(flag == 0)
    {
        printk(KERN_INFO "UID-GID-S_IWOTH did not Match\n");
        errorno = -EACCES;
    }

out:
    if(f1 != NULL && !IS_ERR(f1))
        filp_close(f1, NULL);

    return errorno;
}


/* system call entry-point */
asmlinkage long sys_xjob(void *arg, int length){
    int err = 0;
    int access;
    int id = 0;    

    struct job *job = NULL;        
    struct input *temp = NULL;
	struct input *data = NULL;
   	
	access = access_ok(VERIFY_READ, arg, sizeof(struct input));
    if(!access){    
        printk(KERN_INFO "Verification Failed In Beginning\n");
        err = -EFAULT;
        goto out;
    }

    temp = (struct input *) arg;    
    data = kmalloc(sizeof(struct input), GFP_KERNEL);
    if(data == NULL){    
        err = -ENOMEM;
        printk(KERN_INFO "Not Enough Memory.\n");
        goto out;
    }
    
    err = copy_from_user(data, temp, sizeof(struct input));    
    if(err){
        printk("Not all data are copied\n");
        err = -EFAULT;
        goto kfree;
    }
    
    //check the type of operation
    if( data->operation == CHECKSUM ||
        data->operation == COMPRESSION ||
        data->operation == DECOMPRESSION ||
        data->operation == ENCRYPTION ||
        data->operation == DECRYPTION)
    {
        //construct input struct
        if(!construct_input(data, temp)){    
            printk(KERN_INFO "invalid input/parameter is given\n");
            err = -EINVAL;
            goto kfree;
        }

        //verify input file
        err = input_file_access(data);
        if(err < 0)
            goto kfree;
        
        //verify output file
        err = output_file_access(data);
        if(err < 0)
            goto kfree;
            
        //build job struct   
        err = construct_job(data, &job);
        if(err < 0){        
            printk("error building the job\n");
            goto kfree;
        }
        //current job_id
        id = err; 
        
        //dispatch the job
        err = dispatch(job);
        if(!err){
            //send the job_id to user process
            err = id;
        }
	}
    else if(data->operation == NUMBER_OF_JOBS ||
        data->operation == STATUS_OF_JOB ||
        data->operation == LOG_OF_JOBS)
    {
        //synchronous operation zone
        err = 1010;
    }
    else{
        printk("invalid operation code %d received\n", data->operation );        
        err = -EINVAL;
        goto kfree;    
    }
    
    goto out;
    
kfree: /*careful!! on success another thread would free data*/
	if(data != NULL)
        destroy_input(data);
out:	
	return err ;
}


/* module init */
static int __init init_sys_xjob(void)
{
    int err = 0;
    
    //0. set the global status running
    status = RUNNING;    
        
    //1. initialize buffer and producer_consumer_args
    init_buf(&buf);
    args.buf = &buf;    
        
    //2. initialize sioq
    err = init_sioq();
    if(unlikely(err))
        goto out;    
        
    //3. initialize consumer thread
    init_completion(&args.comp);
    err = init_consumer(&args);
    if(unlikely(err))
        goto out_sioq;
    
    //4. initialize sys_ptr
    if (sysptr == NULL)
        sysptr = sys_xjob;
            
    //5. skip error region
    goto out;

out_sioq:
    stop_sioq();
out:    
    return err;
}

/* module exit */
static void __exit exit_sys_xjob(void){    
    //update status to notify running threads
    status = !RUNNING; 
    
    //stop_consumer thread
    complete(&args.comp);       
    
    //stop sioq
    stop_sioq();
    
    //destroy the buffer
    buf_destroy(&buf);
    
    if (sysptr != NULL)
        sysptr = NULL;
    
    printk(KERN_INFO "sys_xjob module removed successfully\n");
}

module_init(init_sys_xjob);
module_exit(exit_sys_xjob);
MODULE_LICENSE("GPL");
