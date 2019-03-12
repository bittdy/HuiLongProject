#include "socket_usr.h"
//#include<arpa/net.h>

own_socket::Socket_::Socket_(char* address, int port)
{
	this->server_address = address;
	this->server_port = port;
}

own_socket::Socket_::~Socket_()
{
}

void own_socket::Socket_::initial()
{
	WSAStartup(0x0202, &wsaData);
	this->sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //AF_INET指明使用TCP/IP协议族； 
	memset(&(this->server), 0, sizeof(SOCKADDR_IN)); //先将保存地址的server置为全0    
	
	this->server.sin_family = PF_INET; //声明地址格式是TCP/IP地址格式      
	this->server.sin_port = htons(this->server_port); //指明连接服务器的端口号，htons()用于 converts values between the host and network byte order      
	this->server.sin_addr.s_addr = inet_addr(this->server_address); //指明连接服务器的IP地址     
	//结构SOCKADDR_IN的sin_addr字段用于保存IP地址，sin_addr字段也是一个结构体，sin_addr.s_addr用于最终保存IP地址      
	//inet_addr()用于将 形如的"127.0.0.1"字符串转换为IP地址格式 
}

void own_socket::Socket_::close()
{
	closesocket(this->sClient);
	WSACleanup();
}
