#include <linux/cdev.h> 
#include <linux/delay.h> 
#include <linux/device.h> 
#include <linux/fs.h> 
#include <linux/init.h> 
#include <linux/irq.h> 
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/poll.h>


#include <linux/configfs.h>
#include <linux/tty.h> /* For fg_console, MAX_NR_CONSOLES */
#include <linux/kd.h> /* For KDSETLED */
#include <linux/vt.h>
#include <linux/console_struct.h> /* For vc_cons */
#include <linux/vt_kern.h>
#include <linux/timer.h>



#define SUCCESS 0 
#define DEVICE_NAME "chardev"  
#define BUF_LEN 80 


static int device_open(struct inode *, struct file *); 
static int device_release(struct inode *, struct file *); 
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *); 
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *); 


static int major;


//leds
#define BLINK_DELAY HZ/5
#define ALL_LEDS_ON 0x07
#define RESTORE_LEDS 0xFF
struct timer_list my_timer;
struct tty_driver *my_driver;
//char kbledstatus = 0;
static int kbledstatus = 0;
static int value = 0;// cod


//leds/////////////////////////////
static void my_timer_func(struct timer_list *unused) {
    struct tty_struct *t = vc_cons[fg_console].d->port.tty;

    if (kbledstatus == value)
        kbledstatus = RESTORE_LEDS;
    else
        kbledstatus = value;

    (my_driver->ops->ioctl)(t, KDSETLED, kbledstatus);

    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);
}

static int kbleds_init(void) {
    int i;
    pr_info("kbleds: loading\n");
    pr_info("kbleds: fgconsole is %x\n", fg_console);
    for (i = 0; i < MAX_NR_CONSOLES; i++) {
        if (!vc_cons[i].d)
            break;
        pr_info("poet_atkm: console[%i/%i] #%i, tty %lx\n", i, MAX_NR_CONSOLES, vc_cons[i].d->vc_num, (unsigned long)vc_cons[i].d->port.tty);
    }
    pr_info("kbleds: finished scanning consoles\n");

    my_driver = vc_cons[fg_console].d->port.tty->driver;
    pr_info("kbleds: tty driver name %x\n", my_driver->name);

    timer_setup(&my_timer, my_timer_func, 0);

    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);
    return 0;
}

static void kbleds_cleanup(void) {
    pr_info("kbleds: exit\n");
    del_timer(&my_timer);
    (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED, RESTORE_LEDS);
}
//leds/////////////////////////////
enum { 
    CDEV_NOT_USED = 0, 
    CDEV_EXCLUSIVE_OPEN = 1, 
}; 
 

static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED); 
static char msg[BUF_LEN]; 
static size_t mode = 0;
static struct class *cls; 
static struct file_operations chardev_fops = { 
    .read = device_read, 
    .write = device_write, 
    .open = device_open, 
    .release = device_release, 
}; 


static int __init chardev_init(void) { 
    major = register_chrdev(0, DEVICE_NAME, &chardev_fops); 
 
    if (major < 0) { 
        pr_alert("Registering char device failed with %d\n", major); 
        return major; 
    } 
    pr_info("I was assigned major number %d.\n", major); 
 
    cls = class_create(DEVICE_NAME);
    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME); 
    pr_info("Device created on /dev/%s\n", DEVICE_NAME); 
    
    
    int error = 0;
    if ((error = kbleds_init())) {
        pr_debug("failed kbleds_init \n");
        return error;
    }
    
    
    return SUCCESS; 
} 
 
static void __exit chardev_exit(void){ 
    device_destroy(cls, MKDEV(major, 0)); 
    class_destroy(cls); 
     
    unregister_chrdev(major, DEVICE_NAME); 
    kbleds_cleanup();
    printk("Device is exited\n");
} 
 


static int device_open(struct inode *inode, struct file *file){ 
    static int counter = 0; 
 
    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN)) 
        return -EBUSY; 
 
    sprintf(msg, "I already communicate with you %d times (communicate = read + write)\nMode: %ld\n", ++counter, mode); 
    try_module_get(THIS_MODULE); 
    return SUCCESS; 
} 
 
 
static int device_release(struct inode *inode, struct file *file){ 
    atomic_set(&already_open, CDEV_NOT_USED); 
    
    
    module_put(THIS_MODULE); 
    return SUCCESS; 
} 
 

static ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset) {
    int bytes_read = 0; 
    const char *msg_ptr = msg; 
    if (!*(msg_ptr + *offset)) {
        *offset = 0;
        return 0; 
    } 
    msg_ptr += *offset; 
    
    while (length && *msg_ptr) { 
		
        put_user(*(msg_ptr++), buffer++); 
        length--; 
        bytes_read++; 
    } 
    *offset += bytes_read; 
    
    return bytes_read; 
} 
 

static ssize_t device_write(struct file *filp, const char __user *buff,  size_t len, loff_t *off){ 
    short count = 0;
    memset(msg, 0, BUF_LEN);
    int temp;
    char ch;
    while (len > 0 && count < BUF_LEN - 1) {
        if (get_user(msg[count], buff + count)) {
            return -EFAULT; // Ошибка доступа к памяти
        }
        ++count;
        len--;
    }
    //записываем только первую цифру в эту переменную
    temp = msg[0] - '0';
    printk("Number from write: %d\n", temp);
    value = temp;
    return count; 
    
} 
 
module_init(chardev_init); 
module_exit(chardev_exit); 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Emelyanov Nikita");
MODULE_DESCRIPTION("chardev WITHOUT leds from Emelyanov Nikita");

























