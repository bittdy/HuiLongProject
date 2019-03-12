#pragma once
#ifndef SOCKET_USE
#define SOCKET_USE
#include<WinSock2.h>
#include<stdio.h>

//#define SERVER_ADDRESS "10.1.76.225" //服务器地址
//#define SERVER_PORT 1355 //服务器端口号
#define MSGSIZE 1024 //收发缓冲区大小
#define _WINSOCK_DEPRECATED_NO_WARNINGS 

#pragma comment (lib,"ws2_32.lib")

namespace own_socket
{
	class Socket_
	{
	public:
		WSADATA wsaData;
		//连接所用套节字      
		SOCKET sClient;
		//保存远程服务器的地址信息      
		SOCKADDR_IN server;
		//收发缓冲区      
		char szMessage[MSGSIZE];
		//成功接收字节的个数      
		int ret;
		char* server_address;
		int server_port;
		Socket_(char *, int);
		~Socket_();
		void initial();
		void connect();
		void send(char*);
		void close();
	};
}

#endif