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

#ifndef _SIOQ_H
#define _SIOQ_H


/* struct for each task */
struct checksum_args {
	struct inode *parent;
	struct dentry *dentry;
	umode_t mode;
    int id;
	struct nameidata *nd;
};


struct sioq_args {
	struct completion comp;    
	struct work_struct work;    
	int err;
	void *ret;

	union {
		struct checksum_args checksum;		
	};
};


/* Extern definitions for SIOQ functions */
extern int __init init_sioq(void);
extern void stop_sioq(void);
extern void run_sioq(work_func_t func, struct sioq_args *args);
extern void flush_sioq(void);

/* Extern definitions for our privilege escalation helpers */
//extern void __xjob_checksum2();
//extern void __xjob_checksum3();
//extern void __xjob_checksum();

extern void __xjob_checksum(struct work_struct *work);
extern void __xjob_checksum2(struct work_struct *work);
extern void __xjob_checksum3(struct work_struct *work);
//extern void __xjob_checksum(struct delayed_work *work);
#endif /* not _SIOQ_H */
