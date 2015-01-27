/*
 * Copyright (c) 2006-2014 Syed Masum Billah
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

	superio_workqueue = alloc_workqueue("xjob_siod", WQ_NON_REENTRANT, 0);        
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

int run_sioq(work_func_t func, struct sioq_args *args)
{   
    int err;
    INIT_WORK(&args->work, func);    	    
    err = queue_work(superio_workqueue, &args->work);         
    return err;    
}

/*special purpose jobs executor*/
void __xjob_sleep(struct work_struct *work)
{
	int i=0;
    struct sioq_args *args = container_of(work, struct sioq_args, work);	
    
    for (i=0;i<10;i++){
        get_random(&args->err);
        msleep(args->err);
    }
    
    printk("@wq: job:d just finished... \n");
    args->err = 0;	    	
    kfree(args);
    up_semaphor();
}

void __xjob_encryption(struct work_struct *work)
{	
    int err = 0;
    struct sioq_args *args = container_of(work, struct sioq_args, work);
	struct job *job = args->job;
    
    err = file_encryption(job->input->in_file, 
				job->input->out_file, job->input->scheme);       
    
    printk("@wq: job:%d just finished... \n", job->input->job_id);
    args->err = err;	    	    
    
    //send signal to user process
    signal_a_process(job->input->process_id, JOB_FINISHED);
    destroy_input(job->input);            
    kfree(job);
    kfree(args); 
    
    //allow next job to dequeue from buffer
    up_semaphor();          
}

void __xjob_decryption(struct work_struct *work)
{	
    int err = 0;
    struct sioq_args *args = container_of(work, struct sioq_args, work);
	struct job *job = args->job;
    
    err = file_decryption(job->input->in_file, 
				job->input->out_file, job->input->scheme);       
    
    printk("@wq: job:%d just finished... \n", job->input->job_id);
    args->err = err;	    	    
    
    //send signal to user process
    signal_a_process(job->input->process_id, JOB_FINISHED);
    destroy_input(job->input);            
    kfree(job);
    kfree(args); 
    
    //allow next job to dequeue from buffer
    up_semaphor();
}

void __xjob_compression(struct work_struct *work)
{	
    int err = 0;
    struct sioq_args *args = container_of(work, struct sioq_args, work);
	struct job *job = args->job;
    
    err = file_compression(job->input->in_file, 
				job->input->out_file, "zlib");       
    
    printk("@wq: job:%d just finished... \n", job->input->job_id);
    args->err = err;	    	    
    
    //send signal to user process
    signal_a_process(job->input->process_id, JOB_FINISHED);
    destroy_input(job->input);            
    kfree(job);
    kfree(args); 
    
    //allow next job to dequeue from buffer
    up_semaphor();
}

void __xjob_decompression(struct work_struct *work)
{	
    int err = 0;
    struct sioq_args *args = container_of(work, struct sioq_args, work);
	struct job *job = args->job;
    
    err = file_decompression(job->input->in_file, 
				job->input->out_file, "zlib");       
    
    printk("@wq: job:%d just finished... \n", job->input->job_id);
    args->err = err;	    	    
    
    //send signal to user process
    signal_a_process(job->input->process_id, JOB_FINISHED);
    destroy_input(job->input);            
    kfree(job);
    kfree(args); 
    
    //allow next job to dequeue from buffer
    up_semaphor();
}

void __xjob_checksum(struct work_struct *work)
{
    int err = 0;
    struct sioq_args *args = container_of(work, struct sioq_args, work);
	struct job *job = args->job;
    
    err = file_checksum(job->input->in_file, 
				job->input->out_file, "sha1");       
    
    printk("@wq: job:%d just finished... \n", job->input->job_id);
    args->err = err;	    	    
    
    //send signal to user process
    signal_a_process(job->input->process_id, JOB_FINISHED);
    destroy_input(job->input);            
    kfree(job);
    kfree(args); 
    
    //allow next job to dequeue from buffer
    up_semaphor();
}
