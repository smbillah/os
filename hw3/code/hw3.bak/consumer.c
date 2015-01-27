/*
 * Copyright (c) 2006-2014 Syed M Billah
 * Copyright (c) 2006-2014 Stony Brook University
 * Copyright (c) 2006-2014 The Research Foundation of SUNY
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "xjob.h"

/*
 *
 */

static struct task_struct *consumer;

int __init init_consumer(struct producer_consumer_args *args)
{
	int err;
    //struct consumer_args args;
	consumer = kthread_run(&run_consumer,(void *)args, "consumer");    
	if (!IS_ERR(consumer))
		return 0;

	err = PTR_ERR(consumer);
	printk(KERN_ERR "xjob: create_consumer failed %d\n", err);
	consumer = NULL;
	return err;
}

void stop_consumer(void)
{
	if (consumer){
        printk("consumer is going to exit\n");
        kthread_stop( consumer);		
    }
}


int run_consumer(void *args)
{    
    struct sioq_args sioq;        
    int delay;
    struct producer_consumer_args * pc_args = (struct producer_consumer_args *)args;
    struct fifo_args f_args;    
    
    while(1){
        //response to module-kill signal

        // if fifo is empty, then simply sleeps 
        if(fifo_is_empty()){
            printk("@consm: FIFO is empty... going to sleep\n");            
            wait_for_completion_interruptible(&pc_args->comp);            
            printk("@consm: woke up to do work\n");
            INIT_COMPLETION(pc_args->comp);
        }
        
        if(!is_running()){
            printk("@consm: existing now\n");
            break; 
        }
        
        // pop a job from producer-consumer queue
        if(fifo_out(&f_args)){         
            //wrap the job in work_struct
            sioq.checksum.id = f_args.id;                                         
            printk("@consm: consume job:%d, current fifo_size:%d\n", f_args.id, fifo_current_size());
            
            //send the job to kernel work_queue
            run_sioq(__xjob_checksum, &sioq);
            
            //schedule();        
            //get_random(&delay);
            //msleep(delay);
        }        
    }    
    
    //consumer is about to exit
    do_exit(0);
}
