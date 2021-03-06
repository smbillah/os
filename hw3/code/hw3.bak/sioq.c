/*
 * Copyright (c) 2006-2014 Erez Zadok
 * Copyright (c) 2006      Charles P. Wright
 * Copyright (c) 2006-2007 Josef 'Jeff' Sipek
 * Copyright (c) 2006      Junjiro Okajima
 * Copyright (c) 2006      David P. Quigley
 * Copyright (c) 2006-2014 Stony Brook University
 * Copyright (c) 2006-2014 The Research Foundation of SUNY
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "xjob.h"

/*
 * Super-user IO work Queue - sometimes we need to perform actions which
 * would fail due to the unix permissions on the parent directory (e.g.,
 * rmdir a directory which appears empty, but in reality contains
 * whiteouts).
 */

static struct workqueue_struct *superio_workqueue;

int __init init_sioq(void)
{
	int err;

	superio_workqueue = alloc_workqueue("xjob_siod", WQ_NON_REENTRANT|WQ_MEM_RECLAIM, 0);    
    //superio_workqueue = create_workqueue("xjob_siod3");
	if (!IS_ERR(superio_workqueue))
		return 0;

	err = PTR_ERR(superio_workqueue);
	printk(KERN_ERR "xjob: create_workqueue failed %d\n", err);
	superio_workqueue = NULL;
	return err;
}

void stop_sioq(void)
{
	if (superio_workqueue){
        flush_workqueue(superio_workqueue);
        destroy_workqueue(superio_workqueue);        
    }
}

//void is_pending_or cancel_(){
    //work_pending( work ); is pending
    //int cancel_work_sync( struct work_struct *work );
//}

void flush_sioq(){
    if (superio_workqueue)
        flush_workqueue(superio_workqueue);
}

void run_sioq(work_func_t func, struct sioq_args *args)
{          
    INIT_WORK(&args->work, func);    
	init_completion(&args->comp);   
    
    queue_work(superio_workqueue, &args->work);         
    //while (!queue_work(superio_workqueue, &args->work))
        //schedule();
		
    wait_for_completion_killable(&args->comp);
}

/*
void run_sioq(work_func_t func, struct sioq_args *args)
{        
    //int err;        
    INIT_WORK(&args->work, func);
    //INIT_DELAYED_WORK(&args->work, func);
	//init_completion(&args->comp);   
    
     queue_work(superio_workqueue, &args->work);
     //err = queue_delayed_work(superio_workqueue, &args->work, j0);
     //printk("%ld: job posting value %d\n", jiffies,err);
    //queue_work(superio_workqueue, &args->work);
	//while (!queue_work(superio_workqueue, &args->work)) {		
        //schedule();
	//}
	//wait_for_completion(&args->comp);
    //wait_for_completion_killable(&args->comp);
}
*/
//void __xjob_checksum2()
void __xjob_checksum2(struct work_struct *work)
//void __xjob_checksum(struct delayed_work *work)
{
	//struct sioq_args *args = container_of(work, struct sioq_args, work);
	//struct checksum_args *c = &args->checksum;
    
    int count;
    int err = 0;
    struct file   *outfile;   
    unsigned char * buffer;
    
    unsigned long j0,j1;
    int i, delay;
    delay = 3*HZ;    
    
    mm_segment_t oldfs;
    oldfs = get_fs();
    set_fs(KERNEL_DS); 
    
    
    outfile = filp_open("/usr/src/hw3-cse506g12/hw3/big.pdf", O_RDONLY, 0);    
    if (IS_ERR(outfile)) {
        err = PTR_ERR(outfile);        
        printk("consumer file open failed\n");
        goto out;
    } 
    
    printk("consumer file opened \n");
    buffer = kmalloc(sizeof(unsigned char)*PAGE_SIZE, GFP_KERNEL);    
    if (!buffer) {
        err = -ENOMEM;
        goto out_fclose;
    }
    
    printk("consumer memory allocation\n");
    
    while(1){        
        count = vfs_read(outfile, buffer, PAGE_SIZE, &outfile->f_pos);
        
        if (count < 0){// error            
            goto out_free;
        }
        else if(count == 0){
            //finish reading            
            break;
        }
        else{
            //printk("consumer read a block\n");
            msleep(5);
        }
    }
    
    
    /*
    printk("consumer reding ended now context started\n");
    for(i =0; i<5; i++){
        printk("consumer runs ...\n");
        j0 = jiffies; 
        j1 = j0 + delay; 
        while (time_before(jiffies, j1))        
            schedule();                
    }    
    */
    printk("consumer ended\n");
///*
    out_free:
    set_fs(oldfs); 
    kfree(buffer);             

out_fclose:
    filp_close(outfile, NULL);
//*/        
out:
    //complete(&args->comp);   
    return;    
    
}


//void __xjob_checksum3(void)
void __xjob_checksum3(struct work_struct *work)
//void __xjob_checksum(struct delayed_work *work)
{
	//struct sioq_args *args = container_of(work, struct sioq_args, work);
	//struct checksum_args *c = &args->checksum;
    
    int count;
    int err = 0;
    struct file   *outfile;   
    unsigned char * buffer;
    
    unsigned long j0,j1;
    int i, delay;
    delay = 3*HZ;    
    
    mm_segment_t oldfs;
    oldfs = get_fs();
    set_fs(KERNEL_DS); 
    
    
    outfile = filp_open("/usr/src/hw3-cse506g12/hw3/big", O_RDONLY, 0);    
    if (IS_ERR(outfile)) {
        err = PTR_ERR(outfile);        
        printk("consumer2 file open failed\n");
        goto out;
    } 
    
    printk("consumer2 file opened \n");
    buffer = kmalloc(sizeof(unsigned char)*PAGE_SIZE, GFP_KERNEL);    
    if (!buffer) {
        err = -ENOMEM;
        goto out_fclose;
    }
    
    printk("consumer memory allocation\n");
    
    while(1){        
        count = vfs_read(outfile, buffer, PAGE_SIZE, &outfile->f_pos);
        
        if (count < 0){// error            
            goto out_free;
        }
        else if(count == 0){
            //finish reading            
            break;
        }
        else{
            //printk("consumer read a block\n");
        }
    }
    
    
    
    printk("consumer2 reding ended now context started\n");
    /*
    for(i =0; i<5; i++){
        printk("consumer2 runs ...\n");
        j0 = jiffies; 
        j1 = j0 + delay; 
        while (time_before(jiffies, j1))        
            schedule();                
    }    
    
    printk("consumer2 ended\n");
    */
    out_free:
    set_fs(oldfs); 
    kfree(buffer);             

out_fclose:
    filp_close(outfile, NULL);
//*/        
out:
    //complete(&args->comp);   
    return;    
    
}

void __xjob_checksum(struct work_struct *work)
{
	struct sioq_args *args = container_of(work, struct sioq_args, work);
	struct checksum_args *checksum = &args->checksum;
    
    get_random(&args->err);
    msleep(args->err);
    
    printk("@wq: job:%d finished... \n", checksum->id);
    args->err = 0;	
	complete(&args->comp);   
}

/*
void __xjob_checksum(struct work_struct *work)
{
	//struct sioq_args *args = container_of(work, struct sioq_args, work);
	//struct checksum_args *c = &args->checksum;
    unsigned long j0,j1;
    int i, delay;
    delay = 3*HZ;
    
    j0 = jiffies; 
    j1 = j0 + delay; 
    printk("producer started... \n");
    for(i =0 ; i < 5;i++){
        printk("producer runs ...\n");
        j0 = jiffies; 
        j1 = j0 + delay; 
        
        while (time_before(jiffies, j1))        
            schedule();                
    } 
    
    printk("producer ended... \n");
    //args->err = 0;
	//args->err = vfs_create(c->parent, c->dentry, c->mode, c->nd);
	//complete(&args->comp);   
}
*/