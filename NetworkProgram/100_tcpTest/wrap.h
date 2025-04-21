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


void sys_err(const char* str);
inline void sys_err(const char* str)
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
		sys_err("socket error!");
	
	return n;
}




#endif
