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


#include "../000_common/wrap.h"


#define SERV_PORT 9527


int main(int argc, char* argv[])
{
	int i, j, n, nready;

	int maxfd = 0;

	int listenfd, connfd;

	char buf[BUFSIZ];

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

	
	fd_set rset, allset;
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	maxfd = listenfd;

	while (1)
	{
		rset = allset;
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);
		if(nready < 0)
			perr_exit("select error");

		if(FD_ISSET(listenfd, &rset)) {
			clit_addr_len = sizeof(clit_addr);
			connfd = Accept(listenfd, (struct  sockaddr*)&clit_addr, &clit_addr_len);
			
			FD_SET(connfd, &allset);

			if(maxfd < connfd)
				maxfd = connfd;
			
			nready--;

			// no other fd
			if(nready == 0)
				continue;
		}

		for ( i = listenfd+1; i <= maxfd; i++)
		{
			if(FD_ISSET(i, &rset))
			{				
				int cfd = i;

				n = Read(cfd, buf, sizeof(buf));
				if(n == 0)
				{
					FD_CLR(cfd, &allset);
					Close(cfd);
					printf("fd = %d : client close!!!\n", cfd);
					continue;
				}
				
				if(n < BUFSIZ)
					buf[n] = 0;

				printf("fd = %d : %s", cfd, buf);
				
				for(j = 0; j < n; j++)
					buf[j] = toupper(buf[j]);

				Write(cfd, buf, n);

			}
		}
		
	}
		
	Close(listenfd);

	return 0;
}
