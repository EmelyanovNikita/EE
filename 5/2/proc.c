#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#define MSGSIZE 10
#define PROCNAME "hello"


MODULE_LICENSE("EEnterprise");
MODULE_AUTHOR("Emelanov_Nikita");


static int len,temp;
static char *msg;
 
static ssize_t read_proc(struct file *filp, char *buf, size_t count, loff_t *offp ) {
    if(count > temp) {
        count = temp;
    }
    temp = temp - count;
    if (copy_to_user(buf, msg, count))
    {
		printk(KERN_ERR "copy_to_user\n");
        	return -EFAULT;
    }
    if(count == 0)
        temp = len;
    return count;
}
 
static ssize_t write_proc(struct file *filp, const char *buf, size_t count, loff_t *offp) {
    if (copy_from_user(msg, buf, count))
    {
		printk(KERN_ERR "copy_from_user\n");
	    	return -EFAULT;
    }
    len = count;
    temp = len;
    return count;
}
 
static const struct proc_ops proc_fops = {
    proc_read: read_proc,
    proc_write: write_proc,
};
 
static void create_new_proc_entry(void) { //use of void for no arguments is compulsory now
    proc_create(PROCNAME, 0, NULL, &proc_fops);
    msg = kmalloc(MSGSIZE * sizeof(char), GFP_KERNEL);
}
 
static int proc_init (void) {
    create_new_proc_entry();
    return 0;
}
 
static void proc_cleanup(void) {
    remove_proc_entry(PROCNAME, NULL);
    kfree(msg);
}
 

module_init(proc_init);
module_exit(proc_cleanup);
