#ifndef __FLASH_H
#define __FLASH_H 			   
#include "sys.h"
 
#define FLASH_START_ADDR  0x0801f000	  //д�����ʼ��ַ

extern u8 alhour[10]; //���ӵ�Сʱ	//���������еġ�9����10�����¶�����ֵ
extern u8 almin[10];  //���ӵķ��� 	//���������еġ�9����10�����¶�С��ֵ


void FLASH_W(u32 add,u16 dat);
u16 FLASH_R(u32 add);
void ALFLASH_W (void);
#endif





























