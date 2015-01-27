#include "xjob.h"


#define NETLINK_USER 31

static struct sock *nl_sk = NULL;
//static DECLARE_WAIT_QUEUE_HEAD(wq);
static struct producer_consumer_args args;
static int status = 0;
static int job_id = 0;


static pid_t usr_pid_hello = 0;
static int failed_to_send_hello = 0;

struct job job;
static spinlock_t lock;
static int buf_len;
struct list_head waiting_list;
struct list_head running_list;

struct buf buf;

asmlinkage extern long (*sysptr)(void *arg, int length);


int is_running(void){
    return status;
}

static int send_hello_response( void *msg, int size )
{
    int result;
    struct sk_buff *skb = NULL;
    struct nlmsghdr *nlh = NULL;
    if (0 == usr_pid_hello){
        printk(KERN_CRIT"User spce Application is not ready\n");
        return -1;
    }
    skb = alloc_skb( NLMSG_SPACE( size ), GFP_ATOMIC );
    if( !skb )
    {
        printk(KERN_CRIT"send_hello_response :: Failed to allocate memory\n");
        return -ENOMEM;
    }
    result = -EINVAL;
    nlh = NLMSG_PUT( skb, 0, 0, 0, size );
    memset(nlh, 0, size);
    memcpy( NLMSG_DATA( nlh ), msg, size );
    if (failed_to_send_hello)
        goto nlmsg_failure;
    
    if(nl_sk == NULL) printk("nl_sk == NULL\n");
    if(usr_pid_hello == 0) printk("usr_pid_hello == 0\n");
    if ((nl_sk != NULL) && (usr_pid_hello != 0)){        
        result = netlink_unicast( nl_sk, skb, usr_pid_hello, 0 );
    }
    else{
        printk(KERN_CRIT"Hello >> Comunication between U-Plane and C-Plane closed\n");
        goto nlmsg_failure;
    }
    if( result < 0 )
    {
            printk(KERN_CRIT"Hello :: netlink_unicast fails\n");
            failed_to_send_hello = 1;
            skb = NULL;
            goto nlmsg_failure;
    }
    return result;

nlmsg_failure: /* Required by NLMSG_PUT */
    printk(KERN_CRIT"Hello :: netlink_unicast fails return code = %d\n",result);
    if (skb != NULL){
        kfree_skb(skb);
        skb = NULL;
    }
    return result;
}
static void hello_nl_recv_msg2(struct sk_buff *skb){
    
}

static void hello_nl_recv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    int pid;
    struct sk_buff *skb_out;
    int msg_size;
    char msg[256];
    int res;    
    struct fifo_args f_args; 
        
    nlh=(struct nlmsghdr*)skb->data;
    printk(KERN_INFO "Netlink received msg payload: %s\n",(char*)nlmsg_data(nlh));
    pid = nlh->nlmsg_pid; /*pid of sending process */

    //add task to queue
    if(fifo_is_full()){
        printk("@prod: FIFO is full... sending EBUSY\n");
        sprintf(msg, "SERVER BUSY");        
    }
    else{ 
        f_args.id = job_id++;
        sprintf(msg, "JOB ADMITTED, ID:%d", f_args.id);
        
        if(fifo_is_empty()){
            fifo_in(&f_args);
            printk("@prod: notify consumer, add job:%d\n", f_args.id);
            //notify consumer
            complete(&args.comp);
        }
        else{
            fifo_in(&f_args);
            printk("@prod: add  job:%d, current fifo_size:%d\n", f_args.id, fifo_current_size());            
        }
    }
        
    //generating response
    msg_size = strlen(msg);        
    skb_out = nlmsg_new(msg_size,0);
    if(!skb_out)
    {
        printk(KERN_ERR "Failed to allocate new skb\n");
        return;
    }
    
    nlh = nlmsg_put(skb_out,0,0,NLMSG_DONE,msg_size,0);
    NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
    strncpy(nlmsg_data(nlh),msg,msg_size);
    
    res=nlmsg_unicast(nl_sk,skb_out,pid);    
    if(res<0)
        printk(KERN_INFO "Error while sending bak to user\n");
}

static int send_hello_response2(void *msg, int size ){
    struct task_struct *t;
    int ret;
    struct siginfo info;        
    memset(&info, 0, sizeof(struct siginfo));
    info.si_signo = SIG_TEST;
	info.si_code = SI_QUEUE;
    info.si_int = 1234;
    
    rcu_read_lock();
    t = pid_task(find_vpid(usr_pid_hello), PIDTYPE_PID);
	//t = find_task_by_pid_type(PIDTYPE_PID, usr_pid_hello);  //find the task_struct associated with this pid
	if(t == NULL){
		printk("no such pid\n");
		rcu_read_unlock();
		return -ENODEV;
	}
	rcu_read_unlock();
	ret = send_sig_info(SIG_TEST, &info, t);    //send the signal
	if (ret < 0) {
		printk("error sending signal\n");
		return ret;
	}
    return ret;
}


// system call
asmlinkage long sys_xjob(void *arg, int length)
{
	char * msg = "welcome home!";
    int err = 0;
    usr_pid_hello = *((pid_t*) arg);
    printk("received request from pid %lu \n", usr_pid_hello);
    if(nl_sk == NULL)
        printk("in sys_xjob: nl_sk is NULL\n");
    //usr_pid_hello =  length;   
    //printk("fifo max size %d \n", fifo_max_size());    
    
    err = send_hello_response2(msg, strlen(msg));
    printk("return from sginal  %d \n", err);    
    return err;
}


static void testing(){
    int i=0;
    int err;
    struct job job;
    struct job *entry, data;
        
    //waiting_jobs_head = &waiting_jobs.list;
    
    init_buf(&buf);
    if(buf_is_empty(&buf))
        printk("buffer is empty\n");
    
    UDBG;
    
    for (i=3; i<10 ; i++){                
        job.id = i;
        err = buf_enqueue(&buf, &job);
        if(err) goto out;
    }
    printk("current len: %d \n", buf.len);
    UDBG;
    //err = buf_get_job(&waiting_list, 2, &entry);
    //if(err) goto out;
    //printk("entry returned from the list: %d \n", entry->id);
    if(buf_is_empty(&buf))
        printk("buffer is empty\n");
    else
        printk("buffer has some date\n");
    
    while(buf.len){
        err = buf_dequeue(&buf, &data);
        if(err) goto out;
        printk("entry returned from the list: %d, current len: %d \n", data.id, buf.len);
    }
    UDBG;
    printk("current len: %d \n", buf.len);
    if(buf_is_empty(&buf))
        printk("buffer is empty\n");
    else
        printk("buffer has some date\n");
    /*
    err = buf_is_empty(waiting_jobs)
    if(err)
        printk("buf is empty\n");
    }else{
        printk("buf is full\n");
    }
    */
    
    return;
out:        
    printk("error happened: %d\n", err);
}

static int __init hello_init(void)
{
    int err = 0;
    /*set the global status running */
    status = RUNNING;
    testing();
    //2. initialize fifo 
    //err = init_fifo();
    //if(unlikely(err))
     //   goto out;
    
    //2. initialize sioq
    //err = init_sioq();
    //if(unlikely(err))
     //   goto out;    
    
    //0. initialize netlink socket 
    //nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, 0, hello_nl_recv_msg2, NULL, THIS_MODULE);
    //if(!nl_sk){        
        //err = PTR_ERR(nl_sk);
        //printk("NETLINK init failed\n");
        //goto out_sioq;
    //}


        
    //3. initialize consumer thread
    //init_completion(&args.comp);    
    //err = init_consumer(&args);
    //if(unlikely(err))
    //    goto out_netlink;
    
    //"initialize sys_ptr"
    if (sysptr == NULL)
        sysptr = sys_xjob;
    
    buf_destroy(&buf);
    //4. skip error region
    goto out;
        

out_netlink:
    //netlink_kernel_release(nl_sk);
out_sioq:
    //stop_sioq();
out:    
    return err;
}

static void __exit hello_exit(void){
    status = !RUNNING; //exiting
    printk(KERN_INFO "exiting hello module\n");
    if(nl_sk) 
        netlink_kernel_release(nl_sk);        
    
    //stop_consumer thread
    //complete(&args.comp);       
    
    //stop_fifo();
    //stop_sioq();
        
    if (sysptr != NULL)
        sysptr = NULL;
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
