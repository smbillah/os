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
static struct semaphore sem_lock;

int __init init_consumer(struct producer_consumer_args *args)
{
	int err;
    sema_init(&sem_lock, FIFO_SIZE);
	
    consumer = kthread_run(&run_consumer,(void *)args, "consumer");    
	if (!IS_ERR(consumer))
		return 0;

	err = PTR_ERR(consumer);
	printk(KERN_ERR "consumer: create_consumer failed %d\n", err);
	consumer = NULL;
	return err;
}

void up_semaphor(void){
    up(&sem_lock);
}

void down_semaphor(void){
    down(&sem_lock);
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
    struct sioq_args* sioq_args;            
    struct producer_consumer_args * pc_args = (struct producer_consumer_args *)args;
    struct job *job;    
    
    while(1){
        // if fifo is empty, then simply sleeps                 
        if(buf_is_empty(pc_args->buf)) {
            printk("@consumer: BUF is empty... going to sleep\n");                                
            wait_for_completion_interruptible(&pc_args->comp);            
            printk("@consumer: woke up to do work\n");            
            init_completion(&pc_args->comp);
        }
        
        // this is required for graceful termination
        if(!is_running()){            
            break; 
        }
        
        //make sure there are maximum FIFO_SIZE running jobs
        down_semaphor();
        
        // dequeue a job from producer-consumer queue
        if(!buf_dequeue_ref(pc_args->buf,  &job)){         
            //wrap the job in work_struct
            sioq_args = kmalloc(sizeof(struct sioq_args), GFP_KERNEL);                            
            if(sioq_args){                                
                sioq_args->job = job;                
                printk("@consumer: consume job:%d, current buf_size:%d\n", job->input->job_id, pc_args->buf->len);
                
                //send the job to kernel work_queue
                switch(job->input->operation){                    
                    case CHECKSUM:
                        run_sioq(__xjob_checksum, sioq_args);
                        break;
                    case COMPRESSION:
                        run_sioq(__xjob_compression, sioq_args);
                        break;
                    case DECOMPRESSION:
                        run_sioq(__xjob_decompression, sioq_args);
                        break;
                    case ENCRYPTION:
                        run_sioq(__xjob_encryption, sioq_args);
                        break;
                    case DECRYPTION:
                        run_sioq(__xjob_decryption, sioq_args);
                        break;
                    default:
                        run_sioq(__xjob_sleep, sioq_args);        
                        break;
                }                
            }
        }        
    }    
    
    //consumer is about to exit
    do_exit(0);
}
