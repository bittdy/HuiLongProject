#pragma once
#ifndef SOCKET_USE
#define SOCKET_USE
#include<WinSock2.h>
#include<stdio.h>

//#define SERVER_ADDRESS "10.1.76.225" //��������ַ
//#define SERVER_PORT 1355 //�������˿ں�
#define MSGSIZE 1024 //�շ���������С
#define _WINSOCK_DEPRECATED_NO_WARNINGS 

#pragma comment (lib,"ws2_32.lib")

namespace own_socket
{
	class Socket_
	{
	public:
		WSADATA wsaData;
		//���������׽���      
		SOCKET sClient;
		//����Զ�̷������ĵ�ַ��Ϣ      
		SOCKADDR_IN server;
		//�շ�������      
		char szMessage[MSGSIZE];
		//�ɹ������ֽڵĸ���      
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