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
#include <signal.h>

//#include <asm/sockios.h>
//#include <sys/socket.h>
//#include <asm/socket.h>
//#include <linux/socket.h>
//#include <linux/netlink.h>

#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define SIG_TEST 44

#define __NR_sys_xjob	349	/* our private syscall number */

void receive(int i){
    int rc;
    pid_t p = getpid();
    p = i;
    printf("process id %d\n", p);
    rc = syscall(__NR_sys_xjob, (void*)&p, 0 );	
    
    if (rc >= 0){
		printf("syscall returned %d\n", rc);
    }
	else {
		printf("syscall returned %d\n", rc);
    }
}

void receiveData(int n, siginfo_t *info, void *unused) {
	printf("received a signal %i\n", info->si_int);
}

int main() {   
    int i=0;
	struct sigaction sig;
	sig.sa_sigaction = receiveData;
	sig.sa_flags = SA_SIGINFO;
	sigaction(SIG_TEST, &sig, NULL);
    
    for(i=0; i<1; i++)
        receive(i);
    
    return 1;
}

