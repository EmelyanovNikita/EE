#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/net_namespace.h>

#define NETLINK_USER 31

struct sock *nl_sk = NULL;

static void echo_from_userspace_to_ker_buff_msg(struct sk_buff *skb)
{

    struct nlmsghdr *nlh;
    int pid;
    struct sk_buff *skb_out;
    int msg_size;
    char *msg = "Msg is written to kernel ring buffer";
    int res;
	int res_in;
	
	//логирование входа в функцию
    printk(KERN_INFO "Entering: %s\n", __FUNCTION__);
	

	//получение указателя на заголовок сообщения из входящего буффера
    nlh = (struct nlmsghdr *)skb->data;
    //получение PID отправителя
    pid = nlh->nlmsg_pid; /*pid of sending process */
    //Логирование полуенного сообщения
    res_in = printk(KERN_INFO "Netlink received message: "%s" from user with PID: %d\n", (char *)nlmsg_data(nlh), pid);
    
    
	//получение размера выходного сообщения
    msg_size = strlen(msg);
    
    //создание нового буффера для ответа
    skb_out = nlmsg_new(msg_size, 0);

	//проверка выделилась ли память
    if (!skb_out)
    {

        printk(KERN_ERR "Failed to allocate new skb\n");
        return;

    }
    
    // подготовка заголовка для выходного сообщения
    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    //установка группы назначения
    NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
    if (res_in == 0 )
    {
		//в случаем если сообщение не залогировалось отправляем сообщение об этом 
		//копирование сообщения в выходной буффер //не переживаем размер остаёся прежним, потому что сообщение меньше
		strncpy(nlmsg_data(nlh), "Msg is NOT written to kernel buff", msg_size);
	}
    else 
    {
		//копирование сообщения в выходной буффер
		strncpy(nlmsg_data(nlh), msg, msg_size);
	}
	//отправка сообщение обратно пользователю
    res = nlmsg_unicast(nl_sk, skb_out, pid);
	//логирование ошибки в случае неудачи при отправке
    if (res < 0)
        printk(KERN_INFO "Error while sending back to user\n");
}

struct netlink_kernel_cfg cfg = {
   .groups  = 1,
   .input = echo_from_userspace_to_ker_buff_msg,
};

static int __init echo_init(void)
{

    printk("Entering: %s - will do echo from userspace to kernel ring buffer\n", __FUNCTION__);
    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);

    // nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, 0, hello_nl_recv_msg,
    //                              NULL, THIS_MODULE);
    if (!nl_sk)
    {
        printk(KERN_ALERT "Error creating socket.\n");
        return -10;
    }

    return 0;
}

static void __exit echo_exit(void)
{

    printk(KERN_INFO "Exiting echo module\n");
    netlink_kernel_release(nl_sk);
}
module_init(echo_init);
module_exit(echo_exit);

MODULE_LICENSE("GPL");
