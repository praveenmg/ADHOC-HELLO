#include "receive.h"

#define DEFAULT_PORT 23
#define CONNECT_PORT 2300
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
int send(struct socket *sock, struct sockaddr_in *addr, unsigned char *buf, int len);
static int receive(struct socket* sock, struct sockaddr_in* addr, void * data, int len);

static void start(struct work_struct *work)
{
	int err,size;
	int bufsize = 10;
        unsigned char buf[bufsize+1];
	rrep * tmp_rrep;
	//current->flags |= PF_NOFREEZE;
	//allow_signal(SIGKILL);
	my_work = (my_work_t *)work;
		if ( ( (err = sock_create(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &my_work->sock)) < 0) ||
             ( (err = sock_create(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &my_work->sock_send)) < 0 ))
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
        my_work->addr.sin_family      = AF_INET;
        my_work->addr_send.sin_family = AF_INET;

        my_work->addr.sin_addr.s_addr      = htonl(INADDR_ANY);
        my_work->addr_send.sin_addr.s_addr = htonl(INADDR_SEND);

        my_work->addr.sin_port      = htons(DEFAULT_PORT);
        my_work->addr_send.sin_port = htons(CONNECT_PORT);
	if ( ( (err = my_work->sock->ops->bind(my_work->sock, (struct sockaddr *)&my_work->addr, sizeof(struct sockaddr) ) ) < 0) ||
               (err = my_work->sock_send->ops->connect(my_work->sock_send, (struct sockaddr *)&my_work->addr_send, sizeof(struct sockaddr), 0) < 0 ))
        {
                printk(KERN_INFO MODULE_NAME": Could not bind or connect to socket, error = %d\n", -err);
                goto close_and_out;
        }
	printk(KERN_INFO MODULE_NAME": listening on port %d\n", DEFAULT_PORT);
	//for (;;)
        //{    
		printk(KERN_INFO MODULE_NAME"Inside for loop\n");
		//while(1)
		//{
                //memset(&buf, 0, bufsize+1);
                size = receive(my_work->sock, &my_work->addr, tmp_rrep, sizeof(rrep));
		kfree(tmp_rrep);
		//if(size>0)
		//	break;
		//msleep(500);
		//}
		//if (signal_pending(current))
                  //      break;
		//printk("\nsize=%d",size);
                if (size < 0)
                        printk(KERN_INFO MODULE_NAME": error getting datagram, sock_recvmsg error = %d\n", size);
                /*else 
                {
                        printk(KERN_INFO MODULE_NAME": received %d bytes\n", size);
                        
                        printk("\n data: %s\n", buf);

                        
                        memset(&buf, 0, bufsize+1);
                        strcat(buf, "testing...");
                        send(my_work->sock_send, &my_work->addr_send, buf, strlen(buf));
                }*/
        //}

close_and_out:
        sock_release(my_work->sock);
        sock_release(my_work->sock_send);
        my_work->sock = NULL;
        my_work->sock_send = NULL;

}

int send(struct socket *sock, struct sockaddr_in *addr, unsigned char *buf, int len)
{
	
	struct msghdr msg;
        struct iovec iov;
        mm_segment_t oldfs;
        int size = 0;

        if (sock->sk==NULL)
           return 0;

        iov.iov_base = buf;
        iov.iov_len = len;

        msg.msg_flags = MSG_DONTWAIT;
        msg.msg_name = addr;
        msg.msg_namelen  = sizeof(struct sockaddr_in);
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = NULL;

        oldfs = get_fs();
        set_fs(KERNEL_DS);
        size = sock_sendmsg(sock,&msg,len);
        set_fs(oldfs);
	printk(KERN_INFO MODULE_NAME":Message Sent");
	

        return size;
	
}

static int receive(struct socket* sock, struct sockaddr_in* addr, void * data, int len)
{
	
	//struct msghdr msg;
        //struct iovec iov;
        mm_segment_t oldfs;
        int size = 0;
	rrep * tmp_rrep;
	struct kvec vec ={
                .iov_len =len,
                .iov_base=(char *)data,
        };
	struct msghdr msg = {
                .msg_iovlen = 1,
                .msg_iov = (struct iovec*)&vec,
                //.msg_flags=MSG_DONTWAIT,
		.msg_flags=0,
        };
	printk(KERN_INFO MODULE_NAME":Inside receive");
        if (sock->sk==NULL) return 0;

        //iov.iov_base = buf;
        //iov.iov_len = len;
        /*msg.msg_flags = 0;
        msg.msg_name = addr;
        msg.msg_namelen  = sizeof(struct sockaddr_in);
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
	iov.iov_base = buf;
        iov.iov_len = len;

        msg.msg_control = NULL;
	*/
        oldfs = get_fs();
        set_fs(KERNEL_DS);
        size = sock_recvmsg(sock,&msg,len,msg.msg_flags);
        set_fs(oldfs);
	tmp_rrep = (rrep *)data;
	printk(KERN_INFO "Message is :%s",tmp_rrep->type);
	kfree(tmp_rrep);	
        return size;
        
}

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



