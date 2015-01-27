#include "xjob.h"

asmlinkage extern long (*sysptr)(void *arg, int length);

static struct producer_consumer_args args;
asmlinkage long xjob(void *arg, int length)
{
	int err = 0;
    int i;
    struct fifo_args farg;
    printk("fifo max size %d \n", fifo_max_size());
    printk("fifo init size %d \n", fifo_current_size());
    while(fifo_in(&farg));
    //fifo_in(&farg);
    printk("fifo current size %d \n", fifo_current_size());
    
    while(fifo_out(&farg));
    printk("fifo current size %d \n", fifo_current_size());
    return err;
}


static int __init init_sys_xjob(void)
{
	int err = 0;        
    init_completion(&args.comp);
    printk("installed new sys_xjob module\n");	    
	
    //1. initialize kernel work_queue
    err = init_sioq();
    if(unlikely(err))
        goto out;
    
    //2. initialize fifo 
    err = init_fifo();
    if(unlikely(err))
        goto out_sioq;
    
    //3. initialize producer thread
    err = init_producer(&args);
    if(unlikely(err))
        goto out_fifo;
    
    //4. initialize consumer thread
    err = init_consumer(&args);
    if(unlikely(err))
        goto out_fifo;
    
    if (sysptr == NULL)
		sysptr = xjob;
    goto out;
    
out_fifo:
    stop_fifo();
out_sioq:
    stop_sioq();            
    stop_producer();
    stop_consumer();    
out:        
    return err;
}

static void  __exit exit_sys_xjob(void)
{	    

    stop_producer();    
    stop_consumer();    
    flush_sioq();
    UDBG;
    stop_sioq();
    UDBG;
    stop_fifo();    
    UDBG;
    
    if (sysptr != NULL)
		sysptr = NULL;
    
	printk("removed sys_xjob module\n");
}

module_init(init_sys_xjob);
module_exit(exit_sys_xjob);
MODULE_LICENSE("GPL");
