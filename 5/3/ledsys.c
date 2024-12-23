#include <linux/module.h>
#include <linux/configfs.h>
#include <linux/init.h>
#include <linux/tty.h>          /* For fg_console, MAX_NR_CONSOLES */
#include <linux/kd.h>           /* For KDSETLED */
#include <linux/vt.h>
#include <linux/console_struct.h>       /* For vc_cons test*/
#include <linux/vt_kern.h>
#include <linux/timer.h>
#include <linux/printk.h> 
#include <linux/kobject.h> 
#include <linux/sysfs.h> 
#include <linux/fs.h> 
#include <linux/string.h>

#define K_OBJ_DIR "systest"
#define BLINK_DELAY   HZ/5
#define ALL_LEDS_ON   0x07
#define RESTORE_LEDS  0xFF


MODULE_DESCRIPTION("Example module illustrating the use of Keyboard LEDs.");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Emelyanov Nikita");


static struct kobject *example_kobject;
struct timer_list my_timer;
struct tty_driver *my_driver;
static int _kbledstatus = 0;
static int buttom_leds = 7;// cod




static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
                      char *buf)
{
        return sprintf(buf, "%d\n", buttom_leds);
}
 
static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
                      const char *buf, size_t count)
{
        sscanf(buf, "%du", &buttom_leds);
        return count;
}


static void my_timer_func(struct timer_list *ptr)
{
        if (_kbledstatus == buttom_leds)
                _kbledstatus = RESTORE_LEDS;
        else
                _kbledstatus = buttom_leds;
        (my_driver->ops->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED,
                            _kbledstatus);
        my_timer.expires = jiffies + BLINK_DELAY;
        add_timer(&my_timer);
}


static struct kobj_attribute foo_attribute =__ATTR(buttom_leds, 0775, foo_show,
                                                   foo_store);
                                                   
                                                   
static int __init m_init(void)
{
        int i;
        int error = 0;
        pr_debug("Module initialized successfully \n");
 
        example_kobject = kobject_create_and_add(K_OBJ_DIR,
                                                 kernel_kobj);
        if(!example_kobject)
                return -ENOMEM;
 
        error = sysfs_create_file(example_kobject, &foo_attribute.attr);
        if (error) {
                pr_debug("failed to create the foo file in /sys/kernel/systest \n");
        }
        
        printk(KERN_INFO "kbleds: loading\n");
        printk(KERN_INFO "kbleds: fgconsole is %x\n", fg_console);
        for (i = 0; i < MAX_NR_CONSOLES; i++) {
                if (!vc_cons[i].d)
                        break;
                printk(KERN_INFO "poet_atkm: console[%i/%i] #%i, tty %lx\n", i,
                       MAX_NR_CONSOLES, vc_cons[i].d->vc_num,
                       (unsigned long)vc_cons[i].d->port.tty);
        }
        printk(KERN_INFO "kbleds: finished scanning consoles\n");
        my_driver = vc_cons[fg_console].d->port.tty->driver;
        
        
        timer_setup(&my_timer, my_timer_func, 0);
        
        
        my_timer.expires = jiffies + BLINK_DELAY;
        add_timer(&my_timer);
        return 0;
}
static void __exit m_exit(void)
{
        pr_debug ("Module un initialized successfully \n");
        del_timer(&my_timer);
        (my_driver->ops->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED,
                            RESTORE_LEDS);
        kobject_put(example_kobject);
        
}
module_init(m_init);
module_exit(m_exit);
