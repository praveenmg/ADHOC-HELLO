
#include "send.h"
#include <net/sock.h>
#define DEFAULT_PORT 2326
#define CONNECT_PORT 23
#define MODULE_NAME "ksocket"
//#define INADDR_SEND ((unsigned long int)0x7f000001) /* 127.0.0.1 */
#define INADDR_SEND INADDR_LOOPBACK

MODULE_LICENSE("GPL");
static struct workqueue_struct *my_wq;
//ok


typedef struct {
        struct socket *sock;
        struct sockaddr_in addr;
        struct socket *sock_send;
        struct sockaddr_in addr_send;
	
	struct work_struct workmessage;

}my_work_t;

my_work_t * my_work;
int send(struct socket *sock, struct sockaddr_in *addr, void * data, int len);

static void start(struct work_struct *work)
{
	int err,size;
	rrep * tmp_rrep;
	int bufsize = 10;
        unsigned char buf[bufsize+1];
	mm_segment_t oldfs;
	//current->flags |= PF_NOFREEZE;
	//allow_signal(SIGKILL);
	my_work = (my_work_t *)work;
		if ( 
              (err = sock_create(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &my_work->sock_send)) < 0 )
        {
                printk(KERN_INFO MODULE_NAME": Could not create a datagram socket, error = %d\n", -ENXIO);
                return;
        }
	if((tmp_rrep = (rrep *) kmalloc(sizeof(rrep), GFP_ATOMIC)) == NULL)
	{
		printk(KERN_INFO MODULE_NAME": Could not create a datagram socket, error = %d\n", -ENXIO);
		return;
	}
        memset(&my_work->addr, 0, sizeof(struct sockaddr));
        memset(&my_work->addr_send, 0, sizeof(struct sockaddr));
        my_work->addr_send.sin_family = AF_INET;

        //my_work->addr_send.sin_addr.s_addr = htonl(INADDR_SEND);
	my_work->addr_send.sin_addr.s_addr = in_aton("192.168.123.3");
        my_work->addr_send.sin_port = htons(CONNECT_PORT);
	//sock_set_flag(my_work->sock_send,SOCK_BROADCAST);
	if ((err = my_work->sock_send->ops->bind(my_work->sock_send, (struct sockaddr *)&my_work->addr_send, sizeof(struct sockaddr)) < 0 ))
        {
                printk(KERN_INFO MODULE_NAME": Could not bind or connect to socket, error = %d\n", -err);
                goto close_and_out;
        }
	oldfs=get_fs();
	set_fs(KERNEL_DS);
	set_fs(oldfs);
	//printk(KERN_INFO MODULE_NAME": listening on port %d\n", DEFAULT_PORT);
	//for (;;)
        //{
		//printk(KERN_INFO MODULE_NAME"Inside for loop\n");
                //memset(&buf, 0, bufsize+1);
                //size = receive(my_work->sock, &my_work->addr, buf, bufsize);
		//if (signal_pending(current))
                  //      break;
		//printk("\nsize=%d",size);
                

                        

                        
                        
                        strcpy(tmp_rrep->type, "BROADCAST MESSAGE SENT");
			printk(KERN_INFO MODULE_NAME":String Value:%s",tmp_rrep->type);
                        send(my_work->sock_send, &my_work->addr_send, tmp_rrep, sizeof(rrep));
			kfree(tmp_rrep);
			msleep(500);
        //}

close_and_out:
        sock_release(my_work->sock_send);
        my_work->sock = NULL;
        my_work->sock_send = NULL;

}

int send(struct socket *sock, struct sockaddr_in *addr, void * data, int len)
{
	
	struct msghdr msg;
        struct iovec iov;
        mm_segment_t oldfs;
        int size = 0;
	u_int32_t space;
        if (sock->sk==NULL)
           return 0;
	int interface=1;
	if(sock_setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(char *)&interface,sizeof(interface))<0)	
	{
		printk(KERN_WARNING "No Broadcast");	
	}
        iov.iov_base = (char *) data;
        iov.iov_len = len;
	memset(&my_work->addr, 0, sizeof(struct sockaddr));
        memset(&my_work->addr_send, 0, sizeof(struct sockaddr));
        my_work->addr_send.sin_family = AF_INET;

        //my_work->addr_send.sin_addr.s_addr = htonl(INADDR_SEND);
        my_work->addr_send.sin_addr.s_addr = in_aton("255.255.255.255");
	//sock->sk->sk_flags.SOCK_BROADCAST=1;
        my_work->addr_send.sin_port = htons(CONNECT_PORT);
	
        msg.msg_flags = 0;
        msg.msg_name = (void *) & my_work->addr_send;
        msg.msg_namelen  = sizeof(struct sockaddr_in);
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = NULL;
	if((space = sock_wspace(sock->sk))<len)
	{
		printk(KERN_INFO "ERROR");
		return -ENOMEM;

	}
	sock_set_flag(my_work->sock_send,SOCK_BROADCAST); 
	//sock->sk->broadcast=1;
        oldfs = get_fs();
        set_fs(KERNEL_DS);
        size = sock_sendmsg(sock,&msg,len);
	if(size<0)
		printk(KERN_WARNING "ERROR SEND MSG:%d:",size);
        set_fs(oldfs);
//	printk(KERN_INFO MODULE_NAME":Message Sent from new program");
	

        return size;
	
}

/*static int receive(struct socket* sock, struct sockaddr_in* addr, unsigned char* buf, int len)
{
	
	//struct msghdr msg;
        //struct iovec iov;
        mm_segment_t oldfs;
        int size = 0;
	struct kvec vec ={
                .iov_len =len,
                .iov_base=buf,
        };
	struct msghdr msg = {
                .msg_iovlen = 1,
                .msg_iov = (struct iovec*)&vec,
                .msg_flags=MSG_WAITFORONE,
        };
	printk(KERN_INFO MODULE_NAME":Inside receive");
        if (sock->sk==NULL) return 0;

        //iov.iov_base = buf;
        //iov.iov_len = len;
        msg.msg_flags = 0;
        msg.msg_name = addr;
        msg.msg_namelen  = sizeof(struct sockaddr_in);
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
	iov.iov_base = buf;
        iov.iov_len = len;

        msg.msg_control = NULL;
	        oldfs = get_fs();
        set_fs(KERNEL_DS);
        size = sock_recvmsg(sock,&msg,len,msg.msg_flags);
        set_fs(oldfs);
	
	
        return size;
        
}*/

int init_module(void)
{
	
        int ret;
        
	my_wq = create_workqueue("my_queue");
	/*my_work_t.buf[0]='H';
	my_work_t.buf[1]='E';
	my_work_t.buf[2]='L';
	my_work_t.buf[3]='L';
	my_work_t.buf[4]='O';
	my_work_t.buf[5]='\0';*/
	
	if(my_wq){
		my_work=(my_work_t *)kmalloc(sizeof(my_work_t), GFP_KERNEL);
		
	if(my_work){
		INIT_WORK((struct work_struct *)my_work,start);
		//complete work_send
		ret = queue_work(my_wq,(struct work_struct *)my_work);
}
		
}

	return 0;
}

void cleanup_module(void)
{
	if (my_work->sock != NULL) 
        {
                sock_release(my_work->sock);
		sock_release(my_work->sock_send);
		my_work->sock_send=NULL;
                my_work->sock = NULL;
        }
	kfree(my_work);
	my_work=NULL;
	flush_workqueue(my_wq);	
	destroy_workqueue(my_wq);
	my_wq=NULL;
	return; 
}



