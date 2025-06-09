#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include <sys/wait.h>

#include "../000_common/wrap.h"


#define SERV_PORT 9527

void catch_child(int signum){

	printf("catch_child\n");

	while(1){
		pid_t pid = waitpid(0, NULL, WNOHANG);
		if(pid > 0)
		{
			printf("wait pid: pid[%d]!!!\n", pid);
		}
		else
		{
			printf("wait pid: pid[%d]!!!\n", pid);
			break;
		}
	}
	return;
}


int main(int argc, char* argv[])
{
	int lfd = 0, cfd = 0;
	int ret;
	int i;
	char buf[BUFSIZ], client_IP[1024];    
	int fx = 0;

	pid_t pid;

	struct sockaddr_in serv_addr, clit_addr;
	socklen_t clit_addr_len;
	
	// server addr
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// create socket
	lfd = Socket(AF_INET, SOCK_STREAM, 0);

	// bind
	 Bind(lfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	// listen
	Listen(lfd, 128);

	clit_addr_len = sizeof(clit_addr);

	// accept
	while (1)
	{
		cfd = Accept(lfd, (struct sockaddr*)&clit_addr, &clit_addr_len);
		fx++;

		pid = fork();
		if(pid < 0) {
			perr_exit("fork err");
		}
		else if(pid == 0){
			close(lfd);
			break;
		}
		else{
			struct sigaction act;
			act.sa_handler = catch_child;
			sigemptyset(&act.sa_mask);
			act.sa_flags = 0;

			ret = sigaction(SIGCHLD, &act, NULL);
			if(ret != 0){
				perr_exit("sigaction error");
			}

			close(cfd);
			continue;
		}
	}
	

	// sub process read/write
	if(pid == 0)
	{
		// print client ip
		printf("f%d client ip:%s port:%d\n", fx, 
				inet_ntop(AF_INET, &clit_addr.sin_addr.s_addr, client_IP, sizeof(client_IP)), 
				ntohs(clit_addr.sin_port));

		int need_quit = 0;
		
		// read and write
		while(1){

			ret = Read(cfd, buf, sizeof(buf));
			if(ret == 0)
			{
				Close(cfd);
				printf("f%d client close!!!\n", fx);
				break;
			}
			
			if(strncmp(buf, "quit\n", ret) == 0)
			{
				need_quit = 1;
				printf("f%d server close!!!\n", fx);
				break;
			}

			Write(STDOUT_FILENO, buf, ret);
			
			for(i = 0; i < ret; i++)
				buf[i] = toupper(buf[i]);

			Write(cfd, buf, ret);
		}

		if(need_quit){
			Close(cfd);
		}
	}

	return 0;
}
