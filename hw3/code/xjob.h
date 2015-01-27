/*
 * Copyright (c) 2006-2014 Syed M Billah 
 * Copyright (c) 2006-2014 Stony Brook University
 * Copyright (c) 2006-2014 The Research Foundation of SUNY
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _XJOB_H_
#define _XJOB_H_

#include <linux/export.h>
#include <asm/types.h>          
#include <linux/dcache.h>
#include <linux/file.h>
#include <linux/list.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/mount.h>
#include <linux/namei.h>
#include <linux/page-flags.h>
#include <linux/pagemap.h>
#include <linux/poll.h>
#include <linux/security.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/statfs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/writeback.h>
#include <linux/buffer_head.h>
#include <linux/fs_stack.h>
#include <linux/magic.h>
#include <linux/log2.h>
#include <linux/poison.h>
#include <linux/mman.h>
#include <linux/backing-dev.h>
#include <linux/splice.h>
#include <linux/sched.h>
#include <linux/async.h>
#include <asm/system.h>
#include <linux/delay.h>
#include <linux/kfifo.h>
#include <linux/kthread.h>  // for threads
#include <linux/sched.h>  // for task_struct
#include <linux/time.h>   // for using jiffies  
#include <linux/timer.h>
#include <linux/random.h>
#include <linux/kernel.h>

#include <asm/sockios.h>
#include <asm/socket.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/signal.h>

#include <linux/init.h>
#include <asm/siginfo.h>	 //siginfo
#include <linux/rcupdate.h>	 //rcu_read_lock
#include <linux/sched.h>	 //find_task_by_pid_type
#include <linux/semaphore.h> 

#include "utility.h"
#include "services.h"

/* useful for tracking code reachability */
#define UDBG printk(KERN_DEFAULT "DBG:%s:%s:%d\n", __FILE__, __func__, __LINE__)

#define RUNNING 1

/* fifo size in elements */
#define FIFO_SIZE	8
#define BUF_SIZE	128

#define SIG_TEST 44
#define NETLINK_USER 31
#define MAX_PAYLOAD 1024  /* maximum payload size*/

struct job {        
    int status;    
    struct input *input;
    struct list_head list;
    struct list_head hlist;
};

struct buf{
    spinlock_t lock;
    int len;
    struct list_head head;
};

struct producer_consumer_args{
    struct completion comp;    
    struct buf *buf;
    int err;
	void *ret;
};

struct sioq_args {	
	struct work_struct work;    	    
    struct job * job;	        
    int err;
    void *ret;		    
};


/* Extern definitions for Consumer functions */


extern int __init init_buf(struct buf * );
extern int buf_is_empty(struct buf * );

extern int buf_search(struct buf * , int , struct job **);

extern int buf_enqueue(struct buf * , struct job *);
extern int buf_enqueue_ref(struct buf * , struct job *);

extern int buf_dequeue(struct buf * , struct job *);
extern int buf_dequeue_ref(struct buf * , struct job **);

extern void buf_destroy(struct buf *  );



extern void destroy_input(struct input *);


extern int is_running(void);
extern void get_random(int *);
extern int signal_a_process(pid_t, int);

extern int __init init_consumer(struct producer_consumer_args* );
extern void stop_consumer(void);
extern int run_consumer(void *);

extern void up_semaphor(void);
extern void down_semaphor(void);


/* Extern definitions for SIOQ functions */
extern int __init init_sioq(void);
extern void stop_sioq(void);
extern int run_sioq(work_func_t func, struct sioq_args *args);
extern void flush_sioq(void);

extern void __xjob_sleep(struct work_struct *);

extern void __xjob_checksum(struct work_struct *);

extern void __xjob_encryption(struct work_struct *);
extern void __xjob_decryption(struct work_struct *);

extern void __xjob_compression(struct work_struct *);
extern void __xjob_decompression(struct work_struct *);



#endif	/* not _XJOB_H_ */
