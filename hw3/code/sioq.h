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

#ifndef _SIOQ_H
#define _SIOQ_H

#define MAX_FILE_PATH 256
#define MAX_KEY 256



/* struct for each task */

/* for dummy sleep job*/
struct sleep_args {	
    int id;	
};

struct crypto_args {	
    int job_id;
    int operation;
    char in_file[MAX_FILE_PATH];
    char out_file[MAX_FILE_PATH];
    char key[MAX_KEY];    
};


/* workqueue argument*/
/*
struct sioq_args {	
	struct work_struct work;    
	int err;
	void *ret;

	union {
		struct crypto_args crypto;		
        struct sleep_args sleep;
	};
};
*/
struct sioq_args {	
	struct work_struct work;    	    
    struct job * job;	        
    int err;
    void *ret;		    
};

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

#endif /* not _SIOQ_H */
