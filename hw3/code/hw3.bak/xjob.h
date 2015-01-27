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
#include <asm/types.h>          /* See NOTES */
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

#include "sioq.h"
//#include "consumer.h"
//#include "producer.h"
#include "fifo.h"

/* useful for tracking code reachability */
#define UDBG printk(KERN_DEFAULT "DBG:%s:%s:%d\n", __FILE__, __func__, __LINE__)


#define RUNNING 1

/* fifo size in elements */
#define FIFO_SIZE	8

struct producer_consumer_args{
    struct completion comp;    
    int err;
	void *ret;
};

/* Extern definitions for Consumer functions */
extern int is_running(void);

extern int __init init_consumer(struct producer_consumer_args* );
extern void stop_consumer(void);
extern int run_consumer(void *);




#endif	/* not _XJOB_H_ */
