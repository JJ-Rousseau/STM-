#ifndef __FLASH_H
#define __FLASH_H 			   
#include "sys.h"
 
#define FLASH_START_ADDR  0x0801f000	  //写入的起始地址

extern u8 alhour[10]; //闹钟的小时	//其中数组中的【9】【10】是温度整数值
extern u8 almin[10];  //闹钟的分钟 	//其中数组中的【9】【10】是温度小数值


void FLASH_W(u32 add,u16 dat);
u16 FLASH_R(u32 add);
void ALFLASH_W (void);
#endif





























