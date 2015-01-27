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
 * Super-user IO work Queue - sometimes we need to perform actions which
 * would fail due to the unix permissions on the parent directory (e.g.,
 * rmdir a directory which appears empty, but in reality contains
 * whiteouts).
 */


async_cookie_t async_run(async_func_ptr *ptr, struct sioq_args *args)
{
	async_cookie_t cookie_t = async_schedule(ptr, (void *)args);
    //async_schedule();
	return cookie_t;
    
}

void async_callback(async_cookie_t cookie
void async_synchronize_cookie(async_cookie_t cookie);



void __xjob_checksum(struct work_struct *work)
{
	struct sioq_args *args = container_of(work, struct sioq_args, work);
	struct checksum_args *c = &args->checksum;    
    
    unsigned long j0,j1;
    int delay = 60*HZ;
    j0 = jiffies; 
    j1 = j0 + delay; 
    printk("dummy task %d started... \n", c->id);
    while (time_before(jiffies, j1)) 
        schedule();
    printk("dummy task %d ended... \n", c->id);
    args->err = 0;
	//args->err = vfs_create(c->parent, c->dentry, c->mode, c->nd);
	complete(&args->comp);
}
