#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/page.h>

#include "xconcat.h"


/* forward declarations */
asmlinkage extern long (*sysptr)(void *arg, int argslen);
int file_append(struct file* outfile, struct file* infile, unsigned char* buffer, int *out_offset);
static noinline_for_stack int get_kstat(struct file *file, struct kstat* st);
int file_delete(struct file* file);


/* @file : input file to get kstat
* @st : populate the kstat pointed by the file
* return : 0 on sucess, -1 otherwise
* http://lxr.free-electrons.com/source/drivers/base/firmware_class.c?a=m68k#L276
*/
static noinline_for_stack int get_kstat(struct file *file, struct kstat* st)
{    
    if (vfs_getattr(file->f_path.mnt, file->f_path.dentry, st))
         return -1;
    if (!S_ISREG(st->mode))
        return -1;
    if (st->size != (long)st->size)
         return -1;
    return 0;
}


/* unlink/delete @file from filesystem
* @file: file to unlink
* return: on success 0, -errno otherwise
*/
int file_delete(struct file* file){
    int err = 0;
    struct dentry  *parent;
    struct dentry *dentry;
    
    dentry = file->f_dentry;
    parent = file->f_dentry->d_parent;
    if(!parent){
         return -ENOENT;
    }
    mutex_lock_nested(&parent->d_inode->i_mutex, I_MUTEX_PARENT);
    err =  vfs_unlink(parent->d_inode, dentry);
    mutex_unlock(&parent->d_inode->i_mutex);
    return err;
}

/* read the content of @infile and write it to @outflie
* @outfile : struct file * output file
* @infile : struct file * output file
* @buffer: unsigned char buffer of size PAGE_SIZE
* @out_offset : update the number of bytes read in this variable
* return: on success 0; -errno otherwise.
*/
int file_append(struct file* outfile, struct file* infile, unsigned char* buffer, int *out_offset) {
    int count=0, count2=0;    

    mm_segment_t oldfs;
    oldfs = get_fs();
    set_fs(KERNEL_DS);
    
    while(1){ //read one input file and append it to output file
        count = vfs_read(infile, buffer, PAGE_SIZE, &infile->f_pos);        
        if (count < 0){// error            
            goto out2;
        }
        else if(count == 0){
            //finish reading            
            goto out2;
        }
        else{ //count > 0                                    
            //now write to output file
            count2 = count;
            count = vfs_write(outfile, buffer, count2, &outfile->f_pos);
            //printk("%d bytes write in local\n", count);
            if(count < 0 ){//write error
                goto out2;
            }
            
            //update output accounting info            
            *out_offset += count;                      
        }
    }
    out2:
        set_fs(oldfs);
    return count;
}   


/*
* the core system call to implement
* @arg : golabalArgs_t
* @argslen : len of input parameters
* return: on error return -errno. on success it depends on the flags
*/
asmlinkage long xconcat(void *arg, int argslen)
{
	int total_bytes = 0;
    int total_bytes_written = 0;
    int err = 0;     
    int curr_open_file =0;
    struct file   **infiles;
    struct file   *outfile;    
    struct kstat st;
    int count = 0;
    int file_count=0, write_count=0;          
    unsigned char * buffer;    
    
    golabalArgs_t* globalArgs = NULL;    
    if (arg == NULL) { //1. NULL argument
        return -EINVAL;
    }
    
    if (sizeof(golabalArgs_t)!=argslen){ //2. invalid size of struct and argument
        return -EINVAL;
    }
     
    //allocating kernel memory for arguments    
    globalArgs = kmalloc(sizeof(golabalArgs_t), GFP_KERNEL);    
    if (!globalArgs) {
        err = -ENOMEM; 
        goto out;
    }
    
    //copying data from user-space to kernel-space
    if (copy_from_user (globalArgs, arg, sizeof(golabalArgs_t))){
        err = -EFAULT;
        goto out_kfree;
    }
    
    //3. check whether mode > 0777
    if (globalArgs->mode > 0777) {
        err = -EINVAL;
        goto out_kfree;
    }
        
    //4. validate flags: -P/-N 
    if(!(globalArgs->flags & 0x04)){
        if( (globalArgs->flags & 0x02) && (globalArgs->flags & 0x01)){
            err = -EINVAL;
            goto out_kfree;
        }
    }
    
    //5. check number of input files
    if(globalArgs->infile_count > MAX_INPUT_FILES){
        err = - E2BIG;
        goto out_kfree;
    }   
   
    //6. validate outfile
    outfile = filp_open(globalArgs->outfile, globalArgs->oflags|O_WRONLY, globalArgs->mode);    
    if (IS_ERR(outfile)) {
        err = PTR_ERR(outfile);        
        goto out_kfree;
    } 
    
    //7. input files validate
    infiles = kmalloc(sizeof(struct file *)*globalArgs->infile_count, GFP_KERNEL);
    for(curr_open_file=0; curr_open_file < globalArgs->infile_count; curr_open_file++){
        infiles[curr_open_file] = filp_open(globalArgs->infiles[curr_open_file], O_RDONLY, 0);
        if (IS_ERR(infiles[curr_open_file])) {
            err = PTR_ERR(infiles[curr_open_file]);
            goto out_fclose;
        }
        
        //check if output and input are same
        if (infiles[curr_open_file]->f_dentry->d_inode == outfile->f_dentry->d_inode){
            err = - EINVAL;
            goto out_fclose;
        }
        
        // get the file size    
        if((err = get_kstat(infiles[curr_open_file], &st)) < 0 ){ 
            goto out_fclose;
        }
        total_bytes += (int)st.size;        
    }
        
    //8. allocating kernel memory for buffer
    buffer = kmalloc(sizeof(unsigned char)*PAGE_SIZE, GFP_KERNEL);    
    if (!buffer) {
        err = -ENOMEM;
        goto out_fclose;
    }    
    
    //9. now the real read-write
    for(curr_open_file=0; curr_open_file < globalArgs->infile_count; curr_open_file++){        
        write_count = 0;
        count = file_append(outfile, infiles[curr_open_file], buffer, &write_count);                
        if (count < 0){
            err = count;
            goto out_buffer;        
        }        
        else{
            //update accounting
            total_bytes_written  += write_count;
            file_count++;
            vfs_fsync(outfile, 0);
        }        
    }
    
    //10. return data
    err = total_bytes_written;
    // 10.1 flag handling
    if (globalArgs->flags & PERCENTAGE_MODE){
        err = (int)100.0* total_bytes_written/total_bytes;
    }
    // 10.2 flag handling
    if (globalArgs->flags & NUMFILE_MODE){
        err = file_count;
    }
    
    //11. cleanup section
    out_buffer:
        kfree(buffer);        
    out_fclose:
        filp_close(outfile, NULL);
        for(--curr_open_file; curr_open_file >=0 ; curr_open_file--) 
            filp_close(infiles[curr_open_file], NULL);
        kfree(infiles);
    
    out_kfree: 
        kfree(globalArgs);        
    out:    
        return err;
}
    

static int __init init_sys_xconcat(void)
{
	//printk("installed new sys_xconcat module\n");
	if (sysptr == NULL)
		sysptr = xconcat;
	return 0;
}
static void  __exit exit_sys_xconcat(void)
{
	if (sysptr != NULL)
		sysptr = NULL;
	//printk("removed sys_xconcat module\n");
}
module_init(init_sys_xconcat);
module_exit(exit_sys_xconcat);
MODULE_LICENSE("GPL");
