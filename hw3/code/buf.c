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

/* initialization */
int __init init_buf(struct buf * buf) {        
    INIT_LIST_HEAD(&buf->head);     
    spin_lock_init(&buf->lock);
    buf->len = 0;
    return 0;
}

/* to check whether list is empty*/
int buf_is_empty(struct buf * buf){
    return list_empty(&buf->head);    
}

/*
* buf_enqueue: enqueues a @job to @head
* @head: list_head
* @lock: spin_lock
* @job: struct job * to enqueue
* @buf_len: indicates buffer's current lengh
* @result: 0 on success, -1 otherwise.
*/
int buf_enqueue(struct buf * buf, struct job * job ){    
    struct job * copy;
    if(!buf) return -1;
        
    copy = kmalloc(sizeof(struct job), GFP_KERNEL);   
    if (!copy){         
        return -ENOMEM; 
    }    
    memcpy(copy, job, sizeof(struct job));
    INIT_LIST_HEAD(&copy->list);
    
    spin_lock( &buf->lock);
    list_add_tail(&copy->list, &buf->head);
    buf->len++;    
    spin_unlock(&buf->lock);
    
    //printk(KERN_INFO "inserted one job\n");
    return 0 ;
}


/*
* buf_enqueue: enqueues a @job to @head
* @head: list_head
* @lock: spin_lock
* @job: struct job * to enqueue
* @buf_len: indicates buffer's current lengh
* @result: 0 on success, -1 otherwise.
*/
int buf_enqueue_ref(struct buf * buf, struct job * job ){    
    struct job * copy;
    if(!buf) return -1;
        
    copy = job;   
    if (!copy){         
        return -ENOMEM; 
    }    
    
    INIT_LIST_HEAD(&copy->list);
    
    spin_lock( &buf->lock);
    list_add_tail(&copy->list, &buf->head);
    buf->len++;    
    spin_unlock(&buf->lock);
    
    //printk(KERN_INFO "inserted one job\n");
    return 0 ;
}




/*
* buf_dequeue: dequeues a job from @head to @out
* @head: list_head
* @lock: spin_lock
* @out: struct job * to copy dequed data
* @buf_len: indicates buffer's current lengh
* @result: 0 on success, -1 otherwise.
*/
int buf_dequeue(struct buf * buf, struct job *out){    
    struct job * temp;
    
    if(!buf) return -1;        
    if (list_empty(&buf->head)) return -1;
    
    spin_lock(&buf->lock);
    temp = list_first_entry(&buf->head, struct job, list);        
    list_del(&temp->list);
    buf->len--;
    spin_unlock(&buf->lock);
    memcpy(out, temp, sizeof(struct job));
    kfree(temp);
    
    return 0;
}


/*
* buf_dequeue: dequeues a job from @head to @out
* @head: list_head
* @lock: spin_lock
* @out: struct job * to copy dequed data
* @buf_len: indicates buffer's current lengh
* @result: 0 on success, -1 otherwise.
*/
int buf_dequeue_ref(struct buf * buf, struct job **out){    
    struct job * temp;
    
    if(!buf) return -1;        
    if (list_empty(&buf->head)) return -1;
    
    spin_lock(&buf->lock);
    temp = list_first_entry(&buf->head, struct job, list);        
    list_del(&temp->list);
    buf->len--;
    spin_unlock(&buf->lock);
    
    *out = temp;    
    return 0;
}

/*
* buf_search: search a job from buffer by @id
* @head: list_head
* @lock: spin_lock
* @id: id to lookup
* @out: return element
* @result: 0 on success, -1 otherwise.
*/
int buf_search(struct buf * buf, int id, struct job ** out){
    struct job *job, *tmp;        
    if(!buf) goto failure;
    spin_lock(&buf->lock);
    list_for_each_entry_safe(job, tmp, &buf->head, list){         
         if (job->input->job_id == id){            
            *out = job;
            spin_unlock(&buf->lock);
            goto success;
         }
    }
    spin_unlock(&buf->lock);
failure:
    return -1;    
success:
    return 0;
}

int buf_remove(struct buf * buf, struct job * rem){
    struct job *job, *tmp;        
    if(!buf || !rem) goto failure;
    
    spin_lock(&buf->lock);
    list_for_each_entry_safe(job, tmp, &buf->head, list){         
         if (job == rem){                        
            //spin_unlock(&buf->lock);
            //printk("in _buff to delete");
            goto success;
         }
    }
    spin_unlock(&buf->lock);
failure:
    return -1;    
success:
    list_del(&rem->list);
    buf->len--;
    spin_unlock(&buf->lock);
    return 0;
}

/* to destroy internal data*/     
void buf_destroy(struct buf * buf) {
    struct job *job, *tmp;    
    
    if(!buf) return;
    list_for_each_entry_safe(job, tmp, &buf->head, list){         
         list_del(&job->list);         
         destroy_input(job->input);         
         kfree(job);
         job = NULL;   
    }    
}
