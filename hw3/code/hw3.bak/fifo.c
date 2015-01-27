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

static struct kfifo fifo;
static spinlock_t fifo_lock;

/**
* onetime initialization of fifo
*/
int __init init_fifo(void){
    if (kfifo_alloc(&fifo, FIFO_SIZE * sizeof(struct fifo_args), GFP_KERNEL)) {
        UDBG;
        printk("error kfifo_alloc\n");
        return -ENOMEM;
    }
    UDBG;	
    printk("queue is initialized\n");   
	return 0;
}

/**
* returns FIFO_SIZE
*/
int fifo_max_size(void){
    return kfifo_size(&fifo)/sizeof(struct fifo_args);
    //return (int)FIFO_SIZE;
}

/**
* returns current element index
*/
int fifo_current_size(){
    return kfifo_len(&fifo)/sizeof(struct fifo_args);        
}

/**
* return 1 if fifo is empty
*/
int fifo_is_empty(void){
    //return kfifo_is_empty(fifo);
    return fifo_current_size() == 0;
}

/**
* return 1 if fifo is full
*/
int fifo_is_full(void){    
    return fifo_current_size() == fifo_max_size();
}

/**
* reset all &fifo elements to zero
*/
void fifo_reset(void){
    kfifo_reset(&fifo);
    return;     
}

/**
* @args element to insert 
* return 1 on success, 0 otherwise
*/
void get_random(int * rand){
    get_random_bytes((void *)rand, sizeof(int));
    *rand = (int)*rand < 0 ? - *rand : *rand;
    *rand %= FIFO_SIZE;        
    return;
}

int fifo_in(struct fifo_args* args){
    if(fifo_current_size() < fifo_max_size()){
        //kfifo_in(&fifo, (void *) args, sizeof(struct fifo_args));
        kfifo_in_locked(&fifo, (void *) args, sizeof(struct fifo_args), &fifo_lock);  
        return 1;
    }else{
        return 0;
    } 
}

/**
* @args element to fill-in 
* return 1 on success, 0 otherwise
*/
int fifo_out(struct fifo_args* args){
    unsigned int sz;

    if(!fifo_is_empty()){
        //sz = kfifo_out(&fifo, (void *)args , sizeof(struct fifo_args));
        sz = kfifo_out_locked(&fifo, (void *)args , sizeof(struct fifo_args), &fifo_lock); 
        if (sz == sizeof(struct fifo_args))
            return 1;
    }
    return 0;    
}

/*
* @destroy &fifo 
*/
void stop_fifo(void){	
    kfifo_free(&fifo);      
    return;
}

