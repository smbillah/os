#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

/*
#include <asm/sockios.h>
#include <sys/socket.h>
#include <errno.h>
//#include <asm/socket.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
*/
#define __NR_sys_xjob	349	/* our private syscall number */
 
#define NETLINK_USER 31

#define MAX_PAYLOAD 1024  /* maximum payload size*/

/*
struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
static int sock_fd;
struct msghdr msg;
struct msghdr reply;
char * blank = "asd";
*/

void receive(){
    int rc;
    pid_t p = getpid();
    printf("process id %d\n", p);
    rc = syscall(__NR_sys_xjob, (void*)&rc, 10 );
	
    if (rc >= 0){
		printf("syscall returned %d\n", rc);
    }
	else {
		printf("syscall returned %d\n", rc);
    }
}

int main() {   
   receive();
   return 1; 
}



/*
int i=0;
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if(sock_fd<0)
        return -1;
        
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();  
    
    bind(sock_fd, (struct sockaddr*)&src_addr,
        sizeof(src_addr));

    memset(&dest_addr, 0, sizeof(dest_addr));    
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;   
    dest_addr.nl_groups = 0; 
    
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    receive();
    for (i =0;i < 1 ; i++){
        
        memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
        nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
        nlh->nlmsg_pid = getpid();
        nlh->nlmsg_flags = 0;
        strcpy(NLMSG_DATA(nlh), "Hello");
        
        iov.iov_base = (void *)nlh;
        iov.iov_len = nlh->nlmsg_len;
        
        msg.msg_name = (void *)&dest_addr;
        msg.msg_namelen = sizeof(dest_addr);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

    
        //printf("%d: Sending message to kernel\n",i);
        //sendmsg(sock_fd,&msg,0);
        printf("%d: Waiting for message from kernel\n",i);

        
        recvmsg(sock_fd, &msg, 0);
        printf("%d: Received message payload: %s\n", i,(char *)NLMSG_DATA(nlh));
    }
    
   close(sock_fd);
   free(nlh);

*/
