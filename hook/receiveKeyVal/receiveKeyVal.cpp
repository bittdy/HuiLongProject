#include <WINSOCK2.H>   
#include <iostream>
#include <stdio.h>     

#define PORT           5950    
#define MSGSIZE        1024    
#pragma comment(lib, "ws2_32.lib")      

bool GetLocalIP(char* ip)
{
	//1.初始化wsa  
	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0)
	{
		return false;
	}
	//2.获取主机名  
	char hostname[256];
	ret = gethostname(hostname, sizeof(hostname));
	if (ret == SOCKET_ERROR)
	{
		return false;
	}
	//3.获取主机ip  
	HOSTENT* host = gethostbyname(hostname);
	if (host == NULL)
	{
		return false;
	}
	//4.转化为char*并拷贝返回  
	strcpy(ip, inet_ntoa(*(in_addr*)*host->h_addr_list));
	return true;
}


int main()
{
	char hostip[40] = { 0 };
	bool gethost = false;
	gethost = GetLocalIP(hostip);
	if (gethost)
	{
		std::cout << "local IP is:" << hostip << std::endl;
	}

	WSADATA wsaData;
	SOCKET sListen;
	SOCKET sClient;
	SOCKADDR_IN local;
	SOCKADDR_IN client;
	char szMessage[MSGSIZE];
	int ret;
	int iaddrSize = sizeof(SOCKADDR_IN);
	WSAStartup(0x0202, &wsaData);
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	local.sin_family = AF_INET;
	local.sin_port = htons(PORT);
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sListen, (struct sockaddr *) &local, sizeof(SOCKADDR_IN));
	listen(sListen, 1);
	sClient = accept(sListen, (struct sockaddr *) &client, &iaddrSize);
	printf("Accepted client:%s:%d\n", inet_ntoa(client.sin_addr),
		ntohs(client.sin_port));
	int num = 0;
	while (TRUE) {
		ret = recv(sClient, szMessage, MSGSIZE, 0);
		szMessage[ret] = '\0';
		num++;
		/*if (ret != 0)
		{
		printf("Received [%d bytes]: '%s'\n", ret, szMessage);
		}*/
		printf("Received [%d bytes]: '%s'\n", ret, szMessage);
	}
	return 0;
}
