


#include <stdio.h>
#include <Windows.h>

int main(void)

{

	unsigned short tmp = 0xffff;

	unsigned short ret1 = 0;

	unsigned char buff[6] = { 0 };

	buff[0] = 0x01;

	buff[1] = 0x05;

	buff[2] = 0x00;

	buff[3] = 0x07;

	buff[4] = 0x00;

	buff[5] = 0x00;



	for (int n = 0; n < 6; n++){/*�˴���6 -- ҪУ���λ��Ϊ6��*/

		tmp = buff[n] ^ tmp;

		for (int i = 0; i < 8; i++){  /*�˴���8 -- ָÿһ��char������8bit��ÿbit��Ҫ����*/

			if (tmp & 0x01){

				tmp = tmp >> 1;

				tmp = tmp ^ 0xa001;

			}

			else{

				tmp = tmp >> 1;

			}

		}

	}

	/*CRCУ����ֵ*/

	printf("%X\n", tmp);

	/*��CRCУ��ĸߵ�λ�Ի�λ��*/

	ret1 = tmp >> 8;

	ret1 = ret1 | (tmp << 8);

	printf("ret: %X\n", ret1);

	Sleep(50000);
	return 0;

}
