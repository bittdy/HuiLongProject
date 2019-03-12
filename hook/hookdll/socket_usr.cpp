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
	this->sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //AF_INETָ��ʹ��TCP/IPЭ���壻 
	memset(&(this->server), 0, sizeof(SOCKADDR_IN)); //�Ƚ������ַ��server��Ϊȫ0    
	
	this->server.sin_family = PF_INET; //������ַ��ʽ��TCP/IP��ַ��ʽ      
	this->server.sin_port = htons(this->server_port); //ָ�����ӷ������Ķ˿ںţ�htons()���� converts values between the host and network byte order      
	this->server.sin_addr.s_addr = inet_addr(this->server_address); //ָ�����ӷ�������IP��ַ     
	//�ṹSOCKADDR_IN��sin_addr�ֶ����ڱ���IP��ַ��sin_addr�ֶ�Ҳ��һ���ṹ�壬sin_addr.s_addr�������ձ���IP��ַ      
	//inet_addr()���ڽ� �����"127.0.0.1"�ַ���ת��ΪIP��ַ��ʽ 
}

void own_socket::Socket_::close()
{
	closesocket(this->sClient);
	WSACleanup();
}
