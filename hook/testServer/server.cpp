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
#include <string>

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

	//set BaudRate = 9600,no parity,one stopbit
	PortDCB.BaudRate = CBR_9600;//CBR_9600;
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
	unsigned char msg[8] = { 0 };
	unsigned long temp = 0;
	short int crc;
	msg[0] = 0x01; //设备主机地址
	msg[1] = 0x05; 
	msg[2] = 0x00; 
	msg[3] = 0x00;
	msg[4] = 0xff; 
	msg[5] = 0x00;

	int keyNum[8] = { 0 };
	unsigned char killmsg[10] = { 0x01, 0x0f, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0xfe, 0x95};
	while (TRUE) {
		ret = recv(sClient, &szMessage, MSGSIZE, 0);
		printf("Received [%d bytes]: ", ret);
		std::cout << szMessage << std::endl;
		//char keyVal[5] = "0x00";
		DWORD dwwrittenLen = 0;
		switch (szMessage)
		{
		case 'a':
			keyNum[0]++;
			if (keyNum[0] % 2 == 1) //奇数次按下,打开该路
			{
				msg[3] = 0x00; //开第一路
				msg[4] = 0xff;
				msg[6] = 0x8c;
				msg[7] = 0x3a;
			}
			else
			{
				msg[3] = 0x00; //关第一路
				msg[4] = 0x00;
				msg[6] = 0xcd;
				msg[7] = 0xca;
			}
			
			if (keyNum[0] >= 5000)  keyNum[0] = 0;
			WriteFile(Rs_Port, msg, 8, &dwwrittenLen, NULL); // Send data via Rs-232
			break;
		case 's':
			keyNum[1]++;
			if (keyNum[1] % 2 == 1) //奇数次按下,打开该路
			{
				msg[4] = 0xff;
				msg[3] = 0x01; //开第二路
				msg[6] = 0xdd;
				msg[7] = 0xfa;
			}
			else
			{
				msg[3] = 0x01; //关第二路
				msg[4] = 0x00;
				msg[6] = 0x9c;
				msg[7] = 0x0a;
			}
			
			if (keyNum[1] >= 5000)  keyNum[1] = 0;
			WriteFile(Rs_Port, msg, 8, &dwwrittenLen, NULL); // Send data via Rs-232
			break;
		case 'd':
			keyNum[2]++;
			if (keyNum[2] % 2 == 1) //奇数次按下,打开该路
			{
				msg[4] = 0xff;
				msg[3] = 0x02; //开第三路
				msg[6] = 0x2d;
				msg[7] = 0xfa;
			}
			else
			{
				msg[3] = 0x02; //关第三路
				msg[4] = 0x00;
				msg[6] = 0x6c;
				msg[7] = 0x0a;
			}
			
			if (keyNum[2] >= 5000)  keyNum[2] = 0;
			WriteFile(Rs_Port, msg, 8, &dwwrittenLen, NULL); // Send data via Rs-232
			break;
		case 'f':
			keyNum[3]++;
			if (keyNum[3] % 2 == 1) //奇数次按下,打开该路
			{
				msg[4] = 0xff;
				msg[3] = 0x03; //开第四路
				msg[6] = 0x7c;
				msg[7] = 0x3a;
			}
			else
			{
				msg[3] = 0x03; //关第四路
				msg[4] = 0x00;
				msg[6] = 0x3d;
				msg[7] = 0xca;
			}

			if (keyNum[3] >= 5000)  keyNum[3] = 0;

			WriteFile(Rs_Port, msg, 8, &dwwrittenLen, NULL); // Send data via Rs-232
			break;
		case 'g':
			keyNum[4]++;
			if (keyNum[4] % 2 == 1) //奇数次按下,打开该路
			{
				msg[4] = 0xff;
				msg[3] = 0x04; //开第五路
				msg[6] = 0xcd;
				msg[7] = 0xfb;
			}
			else
			{
				msg[3] = 0x04; //关第五路
				msg[4] = 0x00;
				msg[6] = 0x8c;
				msg[7] = 0x0b;
			}

			if (keyNum[4] >= 5000)  keyNum[4] = 0;

			WriteFile(Rs_Port, msg, 8, &dwwrittenLen, NULL); // Send data via Rs-232
			break;
		case 'h':
			keyNum[5]++;
			if (keyNum[5] % 2 == 1) //奇数次按下,打开该路
			{
				msg[4] = 0xff;
				msg[3] = 0x05; //开第六路
				msg[6] = 0x9c;
				msg[7] = 0x3b;
			}
			else
			{
				msg[3] = 0x05; //关第六路
				msg[4] = 0x00;
				msg[6] = 0xdd;
				msg[7] = 0xcb;
			}

			if (keyNum[5] >= 5000)  keyNum[5] = 0;

			WriteFile(Rs_Port, msg, 8, &dwwrittenLen, NULL); // Send data via Rs-232
			break;
		case 'j':
			keyNum[6]++;
			if (keyNum[6] % 2 == 1) //奇数次按下,打开该路
			{
				msg[4] = 0xff;
				msg[3] = 0x06; //开第七路
				msg[6] = 0x6c;
				msg[7] = 0x3b;
			}
			else
			{
				msg[3] = 0x06; //关第七路
				msg[4] = 0x00;
				msg[6] = 0x2d;
				msg[7] = 0xcb;
			}

			if (keyNum[6] >= 5000)  keyNum[6] = 0;

			WriteFile(Rs_Port, msg, 8, &dwwrittenLen, NULL); // Send data via Rs-232
			break;
		case 'k':
			keyNum[7]++;
			if (keyNum[7] % 2 == 1) //奇数次按下,打开该路
			{
				msg[4] = 0xff;
				msg[3] = 0x07; //开第八路
				msg[6] = 0x3d;
				msg[7] = 0xfb;
			}
			else
			{
				msg[3] = 0x07; //关第7路
				msg[4] = 0x00;
				msg[6] = 0x7c;
				msg[7] = 0x0b;
			}

			if (keyNum[7] >= 5000)  keyNum[7] = 0;

			WriteFile(Rs_Port, msg, 8, &dwwrittenLen, NULL); // Send data via Rs-232
			break;
		case 'l':
			WriteFile(Rs_Port, killmsg, 10, &dwwrittenLen, NULL); // Send data via Rs-232			
			memset(keyNum, 0, sizeof(keyNum));
			break;
		default:
			WriteFile(Rs_Port, killmsg, 10, &dwwrittenLen, NULL); // Send data via Rs-232
			memset(keyNum, 0, sizeof(keyNum));
			break;
		}

		

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