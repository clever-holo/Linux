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
#include <fcntl.h>
#include <sys/select.h>
#include <sys/poll.h>

#include "../000_common/wrap.h"


#define SERV_PORT 9527

#define OPEN_MAX 1024

int main(int argc, char* argv[])
{
	int i, j, maxi, nready;

	ssize_t n;

	int maxfd = 0;

	int listenfd, connfd, sockfd;

	char buf[BUFSIZ];
	char str[INET_ADDRSTRLEN];

	struct pollfd client[OPEN_MAX];

	// sock addr
	struct sockaddr_in serv_addr, clit_addr;
	socklen_t clit_addr_len;

	// create socket
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	// reuse addr
	int opt = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// server addr
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// bind
	Bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	// listen
	Listen(listenfd, 128);

	// 
	client[0].fd = listenfd;
	client[0].events = POLLIN;

	for(i=1; i<OPEN_MAX; i++)
		client[i].fd = -1;

	maxi = 0;

	

	while (1)
	{

		nready = poll(client, maxi+1, -1);
		if(nready < 0)
			perr_exit("select error");

		if(client[0].revents & POLLIN) {

			clit_addr_len = sizeof(clit_addr);
			connfd = Accept(listenfd, (struct  sockaddr*)&clit_addr, &clit_addr_len);
			
			printf("received from %s at PORT %d\n",
				inet_ntop(AF_INET, &clit_addr.sin_addr, str, sizeof(str)),
				ntohs(clit_addr.sin_port));

			for(i=1; i<OPEN_MAX; i++)
			{
				if(client[i].fd < 0)
				{
					client[i].fd = connfd;
					client[i].events = POLLIN;
					if(i > maxi)
						maxi = i;
					break;
				}
			}

			if(i == OPEN_MAX)
				perr_exit("too many clients");

		
			nready--;

			// no other fd
			if(nready == 0)
				continue;
		}

		for ( i = 1; i <= maxi; i++)
		{
			if(client[i].fd < 0)
				continue;
			
			sockfd = client[i].fd;

			if(client[i].revents & POLLIN) 
			{		
				n = Read(sockfd, buf, sizeof(buf));

				if(n < 0)
				{
					if(errno == ECONNRESET)
					{
						printf("client[%d] closed connection\n", sockfd);
						Close(sockfd);
						client[i].fd = -1;
					}
				}

				if(n == 0)
				{
					Close(sockfd);
					client[i].fd = -1;
					printf("fd = %d : client close!!!\n", sockfd);
					continue;
				}
				
				if(n < BUFSIZ)
					buf[n] = 0;

				printf("fd = %d : %s", sockfd, buf);
				
				for(j = 0; j < n; j++)
					buf[j] = toupper(buf[j]);

				Write(sockfd, buf, n);

			}
		}
		
	}
		
	Close(listenfd);

	return 0;
}
