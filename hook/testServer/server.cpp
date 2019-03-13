/*
* testSocketService.c
*
*  Created on: 2012-8-16
*      Author: 皓月繁星
*/
#include <winsock.h>
#include <stdio.h>   
#include <iostream>
#include "windows.h"
#include "winBase.h"
#include "stdio.h"

//#define  _CRT_SECURE_NO_WARNINGS

#define PORT           5150    
#define MSGSIZE        1024    

#pragma comment(lib, "ws2_32.lib")   

//===============Global Variable declare===========//
HANDLE Rs_Port;
//-------------------------------------------------//

//============function prototype declare===========//
BOOL Open_Port();
//-------------------------------------------------//

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


BOOL Open_Port()
{
	//Open the serial port
	Rs_Port = CreateFile("COM3:",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	DCB PortDCB;

	//Initialize the DCBlength member
	PortDCB.DCBlength = sizeof(PortDCB);

	//Get the default port setting information.
	GetCommState(Rs_Port, &PortDCB);

	//set BaudRate = 115200,no parity,one stopbit
	PortDCB.BaudRate = CBR_115200;//CBR_115200;
	PortDCB.ByteSize = 8;
	PortDCB.Parity = NOPARITY;
	PortDCB.StopBits = ONESTOPBIT;

	PortDCB.fBinary = TRUE;
	PortDCB.fParity = TRUE;
	PortDCB.fOutxCtsFlow = FALSE;
	PortDCB.fOutxDsrFlow = FALSE;
	PortDCB.fDtrControl = DTR_CONTROL_ENABLE;
	PortDCB.fDsrSensitivity = FALSE;
	PortDCB.fTXContinueOnXoff = TRUE;
	PortDCB.fOutX = FALSE;
	PortDCB.fInX = FALSE;
	PortDCB.fErrorChar = FALSE;
	PortDCB.fNull = FALSE;
	PortDCB.fRtsControl = RTS_CONTROL_ENABLE;
	PortDCB.fAbortOnError = FALSE;

	if (!SetCommState(Rs_Port, &PortDCB))
	{
		printf("Unable to configure the serial port.\n");
		return FALSE;
	}

	//Retrieve the time-out parameters for all read and write operations
	//on the port.

	COMMTIMEOUTS CommTimeouts;

	GetCommTimeouts(Rs_Port, &CommTimeouts);


	CommTimeouts.ReadIntervalTimeout = 0;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.ReadTotalTimeoutConstant = 0;

	CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	CommTimeouts.WriteTotalTimeoutConstant = 0;

	if (!SetCommTimeouts(Rs_Port, &CommTimeouts))
	{
		printf("Unable to set the time-out parameters.\n"); // error check
		return FALSE;
	}
	//	EscapeCommFunction(Rs_Port,CLRDTR);
	//	EscapeCommFunction(Rs_Port,CLRRTS);

	return true;
}

int main()
{
	char hostip[40] = "";
	bool gethost = false;
	gethost = GetLocalIP(hostip);
	if (gethost)
	{
		std::cout << "local IP is: " << hostip << std::endl;
		std::cout << "waiting for connecting..." << std::endl;
	}

	WSADATA wsaData;
	SOCKET sListen;
	SOCKET sClient;
	SOCKADDR_IN local;
	SOCKADDR_IN client;
	char szMessage;
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
	DWORD TxNum = 0;
	if (!Open_Port()) std::cout<<"error!"<<std::endl;					//Open Serial port
	unsigned char msg[10] = { 0 };
	unsigned long temp = 0;
	short int crc;
	msg[0] = 0x01; //设备主机地址
	msg[1] = 0x0f; //功能码
	msg[2] = 0x00; //起始码
	msg[3] = 0x00;
	msg[4] = 0x00; //输出数量，设为8个
	msg[5] = 0x08;
	msg[6] = 0x01; //字节计数

	while (TRUE) {
		ret = recv(sClient, &szMessage, MSGSIZE, 0);
		printf("Received [%d bytes]: ", ret);
		std::cout << szMessage << std::endl;
		msg[7] = 0x00; //初始设为关闭
		//char keyVal[5] = "0x00";
		switch (szMessage)
		{
		case 'a':
			msg[7] = 0x01; 
			break;
		case 's':
			msg[7] = 0x02;
			break;
		case 'd':
			msg[7] = 0x04;
			break;
		case 'f':
			msg[7] = 0x08;
			break;
		case 'g':
			msg[7] = 0x10;
			break;
		case 'h':
			msg[7] = 0x20;
			break;
		case 'j':
			msg[7] = 0x40;
			break;
		case 'k':
			msg[7] = 0x80;
			break;
		case 'l':
			msg[7] = 0x00;
			break;
		default:
			msg[7] = 0x00;
			break;
		}

		//计算CRC
		char result_len = 8;
		unsigned char *ptr = msg;
		while (result_len--){
			for (int i = 0x80; i != 0; i = i >> 1){
				temp = temp * 2;
				if ((temp & 0x10000) != 0)
					temp = temp ^ 0x11021;
				if ((*ptr & i) != 0)
					temp = temp ^ (0x10000 ^ 0x11021);
			}
			ptr++;
		}
		crc = temp;
		msg[8] = temp/10000;
		msg[9] = temp%10000;

		//TxNum = strlen(msg);
		DWORD dwwrittenLen = 0;
		WriteFile(Rs_Port, msg, 10, &dwwrittenLen, NULL); // Send data via Rs-232
	}
	return 0;
}




// write.cpp : Defines the entry point for the console application.


/*
int t_main(int argc, char* argv[])
{
	char inputString[100];
	int stringLength;
	DWORD TxNum = 0;

	

	while (1)
	{
		printf("Enter char :");
		gets(inputString);						//get String form input
		stringLength = strlen(inputString);		// get length of string
		TxNum = stringLength;
		
	}
	return 0;
}

*/