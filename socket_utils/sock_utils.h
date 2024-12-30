#ifndef SOCK_UTILS_H
#define SOCK_UTILS_H

#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<malloc.h>
#include<stdio.h>
#include<string.h>




int CreateTCPIpv4Socket();


struct sockaddr_in* CreateTCPIpv4Address(char* ip, int port);

#endif  //SOCK_UTILS_H
