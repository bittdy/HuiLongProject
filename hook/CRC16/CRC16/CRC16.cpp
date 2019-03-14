


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



	for (int n = 0; n < 6; n++){/*此处的6 -- 要校验的位数为6个*/

		tmp = buff[n] ^ tmp;

		for (int i = 0; i < 8; i++){  /*此处的8 -- 指每一个char类型又8bit，每bit都要处理*/

			if (tmp & 0x01){

				tmp = tmp >> 1;

				tmp = tmp ^ 0xa001;

			}

			else{

				tmp = tmp >> 1;

			}

		}

	}

	/*CRC校验后的值*/

	printf("%X\n", tmp);

	/*将CRC校验的高低位对换位置*/

	ret1 = tmp >> 8;

	ret1 = ret1 | (tmp << 8);

	printf("ret: %X\n", ret1);

	Sleep(50000);
	return 0;

}
