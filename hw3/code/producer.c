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

static struct task_struct *producer;
static DECLARE_WAIT_QUEUE_HEAD(pwq);

int __init init_producer(struct producer_consumer_args *args)
{
	int err;
    //struct producer_args args;
	producer = kthread_run(&run_producer,(void *)args, "producer");    
	if (!IS_ERR(producer))
		return 0;

	err = PTR_ERR(producer);
	printk(KERN_ERR "xjob: create_producer failed %d\n", err);
	producer = NULL;
	return err;
}

void stop_producer(void)
{
	if (producer){
        printk("producer is going to exit\n");
        kthread_stop( producer);	     
    }    
}


//void run_sioq(work_func_t func, struct sioq_args *args)
int run_producer(void *args)
{            
    struct producer_consumer_args * pc_args = (struct producer_consumer_args *)args;
    //init_completion(&p_args->comp);
    
    //struct sioq_args sioq;
    //struct sioq_args sioq2;
    //sioq.completion = &args->comp;
	//sioq.checksum.id = 1;     
    //run_sioq(__xjob_checksum, &sioq);
    //__xjob_checksum();
        
    //task 2    
	//sioq2.checksum.id = 2; 
    //run_sioq(__xjob_checksum3, &sioq2);    
    
    //if producer queue is full, then go to sleep
    //wait_for_completion(&p_args->comp);
    //flush_sioq();
    struct fifo_args f_args;    
    int delay;
    int id = 0;
    int i=3;
    while(1){        
        //response to module kill signal
        if(kthread_should_stop())
            break;
            
        // if fifo is full, then simply sleeps 
        if(fifo_is_full()){
            printk("@prod: FIFO is full... going to sleep\n");
            
            //wait_for_completion(&pc_args->comp);
            printk("@prod: woke up to work \n");
            INIT_COMPLETION(pc_args->comp);
        }
        
        if(fifo_is_empty()){            
            f_args.id = id++;
            fifo_in(&f_args);
            printk("@prod: notify consumer, add job:%d\n", f_args.id);
            complete(&pc_args->comp);
            //schedule();
        }else{
            f_args.id = id++;
            fifo_in(&f_args);
            printk("@prod: add  job:%d, current fifo_size:%d\n", f_args.id, fifo_current_size());
            
        }
        get_random(&delay);
        msleep(delay);
        //schedule();
                
    }
        
    //printk("producer is about to exit\n");
    //do_exit(0);     
}
