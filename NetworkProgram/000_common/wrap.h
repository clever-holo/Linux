#ifndef _WRAP_H
#define _WRAP_H

#include <stdio.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>


void perr_exit(const char* str);
inline void perr_exit(const char* str)
{
	perror(str);
	exit(1);
}


int Socket(int domain, int type, int protocol);
inline int Socket(int domain, int type, int protocol)
{
	int n;
	n = socket(domain, type, protocol);
	if(n == -1)
		perr_exit("socket error!");
	
	return n;
}


int Listen(int sockfd, int backlog);
inline int Listen(int sockfd, int backlog)
{
	int n = 0;
	n = listen(sockfd, backlog);
	if(n == -1)
	{
		perr_exit("listen error");
		return n;
	}
	return 0;

}

int Accept(int fd, struct sockaddr* sa, socklen_t* salenptr);
inline int Accept(int fd, struct sockaddr* sa, socklen_t* salenptr)
{
	int n;

again:
	n = accept(fd, sa, salenptr);
	if(n < 0)
	{
		if(errno == ECONNABORTED || errno == EINTR)
			goto again;
		else
			perr_exit("accept error");
	}

	return n;
}

/// @brief  
/// @param fd 
/// @param sa 
/// @param salen 
/// @return 
int Bind(int fd, const struct sockaddr* sa, socklen_t salen);
inline int Bind(int fd, const struct sockaddr* sa, socklen_t salen)
{
	int n;
	n = bind(fd, sa, salen);
	if(n < 0)
		perr_exit("bind error");

	return n;
}

int Connect(int fd, const struct sockaddr* sa, socklen_t salen);
inline int Connect(int fd, const struct sockaddr* sa, socklen_t salen)
{
	int n;
	n = connect(fd, sa, salen);
	if(n < 0)
		perr_exit("connect error");

	return n;
}

ssize_t Read(int fd, void* ptr, size_t nbytes);
inline ssize_t Read(int fd, void* ptr, size_t nbytes)
{
	ssize_t n;

again:
	n = read(fd, ptr, nbytes);
	if(n < 0)
	{
		if(errno == EINTR)
			goto again;
		else
			return -1;
	}
	
	return n;
}

ssize_t Write(int fd, void* ptr, size_t nbytes);
inline ssize_t Write(int fd, void* ptr, size_t nbytes)
{
	ssize_t n;

again:
	n = write(fd, ptr, nbytes);
	if(errno == EINTR)
		goto again;
	else
		return -1;

	return n;

}

int Close(int fd);
inline int Close(int fd)
{
	int n;
	n = close(fd);
	if(n == -1)
		perr_exit("close error");

	return n;
}

#endif
