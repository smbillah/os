#include <linux/string.h>
#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/audit.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/cred.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/namei.h>
#include <linux/fs_stack.h>

/*Maximum number of input files this program support for concatenation.*/
#define MAX_NO_INFILE_SUPPORTED 32
#define DEBUG_MODE 0 
#define DEBUG_ATOMIC 0 

asmlinkage extern long (*sysptr)(void *arg, int length);

asmlinkage long xjob(void *arg, int length)
{
	printk(KERN_INFO "Running the dymmy main method\n");
	return 0;
}

static int __init init_sys_xjob(void)
{
	printk("installed new sys_xjob module\n");
	if (sysptr == NULL)
		sysptr = xjob;
	return 0;
}
static void  __exit exit_sys_xjob(void)
{
	if (sysptr != NULL)
		sysptr = NULL;
	printk("removed sys_xjob module\n");
}
module_init(init_sys_xjob);
module_exit(exit_sys_xjob);
MODULE_LICENSE("GPL");
