#include <stdio.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "wrap.h"


#define SERV_PORT 9527

int main(int argc, char* argv[])
{
	int lfd = 0, cfd = 0;
	int ret;
	int i;
	char buf[BUFSIZ], client_IP[1024];    


	struct sockaddr_in serv_addr, clit_addr;
	socklen_t clit_addr_len;
	
	// server addr
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// create socket
	lfd = Socket(AF_INET, SOCK_STREAM, 0);
	if(lfd == 1){
		sys_err("socket error");
	}

	// bind
	bind(lfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	// listen
	listen(lfd, 128);

	clit_addr_len = sizeof(clit_addr);

	// accept
	cfd = accept(lfd, (struct sockaddr*)&clit_addr, &clit_addr_len);

	if(cfd == 1)
		sys_err("accept error");

	// print client ip
	printf("client ip:%s port:%d\n", 
			inet_ntop(AF_INET, &clit_addr.sin_addr.s_addr, client_IP, sizeof(client_IP)), 
			ntohs(clit_addr.sin_port));

	// read and write
	while(1){

		ret = read(cfd, buf, sizeof(buf));

		if(ret == 0)
			break;

		write(STDOUT_FILENO, buf, ret);
		
		for(i = 0; i < ret; i++)
			buf[i] = toupper(buf[i]);

		write(cfd, buf, ret);
	}

	close(lfd);
	close(cfd);

	return 0;
}
