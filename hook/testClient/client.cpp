#include <WINSOCK2.H>   
#include <stdio.h>     

//���������ʹ�õĳ���      
#define SERVER_ADDRESS "192.168.109.1" //��������IP��ַ      
#define PORT           5150         //�������Ķ˿ں�      
#define MSGSIZE        1024         //�շ��������Ĵ�С      
#pragma comment(lib, "ws2_32.lib")      

int main()
{
	WSADATA wsaData;
	//���������׽���      
	SOCKET sClient;
	//����Զ�̷������ĵ�ַ��Ϣ      
	SOCKADDR_IN server;
	//�շ�������      
	char szMessage[MSGSIZE];
	//�ɹ������ֽڵĸ���      
	int ret;

	// Initialize Windows socket library      
	WSAStartup(0x0202, &wsaData);

	// �����ͻ����׽���      
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //AF_INETָ��ʹ��TCP/IPЭ���壻      
	//SOCK_STREAM, IPPROTO_TCP����ָ��ʹ��TCPЭ��      
	// ָ��Զ�̷������ĵ�ַ��Ϣ(�˿ںš�IP��ַ��)      
	memset(&server, 0, sizeof(SOCKADDR_IN)); //�Ƚ������ַ��server��Ϊȫ0      
	server.sin_family = PF_INET; //������ַ��ʽ��TCP/IP��ַ��ʽ      
	server.sin_port = htons(PORT); //ָ�����ӷ������Ķ˿ںţ�htons()���� converts values between the host and network byte order      
	server.sin_addr.s_addr = inet_addr(SERVER_ADDRESS); //ָ�����ӷ�������IP��ַ      
	//�ṹSOCKADDR_IN��sin_addr�ֶ����ڱ���IP��ַ��sin_addr�ֶ�Ҳ��һ���ṹ�壬sin_addr.s_addr�������ձ���IP��ַ      
	//inet_addr()���ڽ� �����"127.0.0.1"�ַ���ת��ΪIP��ַ��ʽ      
	//�����ղ�ָ���ķ�������      
	connect(sClient, (struct sockaddr *) &server, sizeof(SOCKADDR_IN)); //���Ӻ������sClient��ʹ���������      
	//server������Զ�̷������ĵ�ַ��Ϣ      
	while (TRUE) {
		printf("Send:");
		//�Ӽ�������      
		gets(szMessage); //The gets() functionreads characters from stdin and loads them into szMessage      
		// ��������      
		send(sClient, szMessage, strlen(szMessage), 0); //sClientָ�����ĸ����ӷ��ͣ� szMessageָ�����������ݵı����ַ ��strlen(szMessage)ָ�����ݳ���      
	}

	// �ͷ����Ӻͽ��н�������      
	closesocket(sClient);
	WSACleanup();
	return 0;
}