/*
 * Copyright (c) 2006-2014 Syed M Billah
 * Copyright (c) 2006-2014 Stony Brook University
 * Copyright (c) 2006-2014 The Research Foundation of SUNY
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _FIFO_H
#define _FIFO_H


/* job parameters*/
struct fifo_args{ //should be multiple of 2
    struct completion comp;
    int id;
    int err;
	void *ret;
};


/* Extern definitions for Producer functions */
extern void get_random(int * );
extern int init_fifo(void);
extern int fifo_max_size(void);
extern int fifo_current_size(void);
extern int fifo_is_empty(void);
extern int fifo_is_full(void);
extern void fifo_reset(void);
extern int fifo_in(struct fifo_args *);
extern int fifo_out(struct fifo_args*);
extern void stop_fifo(void);

#endif /* not _FIFO_H */
