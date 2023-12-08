/*********************************************************************************************
模板制作：  杜洋工作室/洋桃电子
程序名：	洋桃项目组--项目1：电子时钟扩展项目
编写人：	杜洋	
编写时间：	2018年10月25日
硬件支持：	STM32F103C8   外部晶振8MHz RCC函数设置主频72MHz　  

修改日志：　　
1-201810251645 开始编程。基于洋桃示例的17号压缩包。
2-201811142351 完成了8个闹钟的小时分钟设置界面，优化了注释。
3-201811180113 完成了闹钟的到时触发效果，优化了注释，闹钟功能全部完成。
4-201811192314 完成了闹钟控制2路继电器的程序。
5-201811220021 加入温度控制继电器程序，但未完成到温度测试的程序部分。 	
6-201811240150 完成温度控制继电器的全部程序，优化了注释。【项目1完成】	
							
说明：
 # 本模板加载了STM32F103内部的RCC时钟设置，并加入了利用滴答定时器的延时函数。
 # 可根据自己的需要增加或删减。

*********************************************************************************************/
#include "stm32f10x.h" //STM32头文件
#include "sys.h"
#include "delay.h"
#include "rtc.h"
#include "TM1640.h"
#include "buzzer.h"
#include "flash.h" 

#include "lm75a.h"
#include "touch_key.h"
#include "relay.h"


int main (void){//主程序
	u8 MENU;//菜单值
	u16 i;
	u16 t;
	u8 h,k; //温度判断用交替开关标志位
	u8 m; //闹钟触发标志位（为1时触发，0时正常）
	u8 a=0,c=0; //通用的变量
	u8 MENU2; //独立的闹钟序号菜单（从1～8）
	u8 rup;//时间更新标志位
	u8 buffer[3]; //温度数据寄存器
	RCC_Configuration(); //系统时钟初始化
	RELAY_Init();//继电器初始化 
	delay_ms(200); //延时毫秒
	RTC_Config();  //RTC初始化
	TM1640_Init(); //TM1640初始化
	I2C_Configuration();//I2C初始化
	LM75A_GetTemp(buffer); //读取LM75A的温度数据
	TOUCH_KEY_Init();//按键初始化
	BUZZER_Init();//蜂鸣器初始化
	BUZZER_BEEP2();//蜂鸣器音1
	MENU=0;	 //菜单初始值为0
	rup=0; //RTC时间设置标志位，为1时更新RTC时间
	t=0; //各种变量初始化
	m=0;
	h=0;
	k=0;
	RELAY_1(0);//继电器的控制程序（0继电器放开,1继电器吸合）
	RELAY_2(0);//继电器的控制程序（0继电器放开,1继电器吸合）
	//读FLASH数据
	for(i=0;i<10;i++){  //将8组闹钟数据从FLASH读出来
 		t = FLASH_R(FLASH_START_ADDR+i*2);//从指定页的地址读FLASH
		alhour[i]=t/0x100; //把FLASH读出的16位半字放入2个8位变量
		almin[i]=t%0x100;
	}
	if(alhour[0]>24){ //判断，如果闹钟1的小时值大于24就初始化闹钟的所有数据
		for(i=0;i<10;i++){
			if(i<8)	alhour[i]=24; //闹钟的小时（闹钟的24小时为关闹钟）
			else alhour[i]=0;
			almin[i]=0;  //闹钟的分钟
		}
		ALFLASH_W(); //将闹钟的所有数据写入到FLASH
	}
	while(1){  //主循环
//【数据刷新与主界面切换】
		if(rup==1){	//判断设置时间标志位
			rup=0; //标志位清0
			RTC_Set(ryear,rmon,rday,rhour,rmin,rsec); //设置时间，将变量写回到RTC
		}
		if(MENU<3 || MENU >8){ //如果菜单值不在RTC时间设置菜单时可更新时间
			if(MENU==0)LM75A_GetTemp(buffer); //读取LM75A的温度数据
			RTC_Get(); //读出RTC时间
		}
		//显示内容切换的控制程序
		if(MENU<3){//只在切换的3个菜单里调用，不用于调时菜单
			t++;
			if(t==1)MENU=1;	 //根据t的值切换菜单
			if(t==5000)MENU=0;
			if(t==8000)MENU=2;
			if(t>=11000)t=0; //t的值溢出则清0
		}
		if(MENU<3){ //显示切换的3个菜单
			if(MENU==0){
				TM1640_display(0,ryear%100/10);	//年 
				TM1640_display(1,ryear%100%10);
				TM1640_display(2,21);  // -
				TM1640_display(3,rmon/10);//月
				TM1640_display(4,rmon%10);	
				TM1640_display(5,21); // -
				TM1640_display(6,rday/10); //日
				TM1640_display(7,rday%10);
			}else if(MENU==1){
				TM1640_display(0,20);	//无
				TM1640_display(1,20);
				TM1640_display(2,rhour/10); //时
				TM1640_display(3,rhour%10+10);
				TM1640_display(4,rmin/10);	//分
				TM1640_display(5,rmin%10+10);
				TM1640_display(6,rsec/10); //秒
				TM1640_display(7,rsec%10);
			}else{
				TM1640_display(0,20);	//无
				TM1640_display(1,20);
				TM1640_display(2,20); 
				if(!buffer[0])TM1640_display(3,20);  //正负号
				else TM1640_display(3,21);
				TM1640_display(4,buffer[1]%100/10);	//温度 
				TM1640_display(5,buffer[1]%10+10);
				TM1640_display(6,buffer[2]%100/10); //
				TM1640_display(7,22); //c
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){ //读触摸按键的电平
				MENU=0;
				BUZZER_BEEP1();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){ //读触摸按键的电平
				MENU=1;
				BUZZER_BEEP1();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //读触摸按键的电平
				MENU=2;
				BUZZER_BEEP1();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //读触摸按键的电平
				MENU=3;
				BUZZER_BEEP1();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
//【调时菜单部分】
		if(MENU==3){ //SEt 设置RTC时间的菜单，从3到8
			TM1640_display(0,23);	//SEt
			TM1640_display(1,24);
			TM1640_display(2,25); 
			TM1640_display(3,20);  //无
			TM1640_display(4,20);	
			TM1640_display(5,20);
			TM1640_display(6,20); //
			TM1640_display(7,20); //
			BUZZER_BEEP3();//蜂鸣器音1
			MENU=100;
			MENU2=0;
		}
		if(MENU==100){ //功能菜单切换
			if(MENU2==0){ //显示SET
				TM1640_display(0,23);	//SEt
				TM1640_display(1,24);
				TM1640_display(2,25); 
			}
			if(MENU2>=1 && MENU2<9){ //显示AL1到8 闹钟值
				TM1640_display(0,26);	//AL1
				TM1640_display(1,27);
				TM1640_display(2,MENU2); //显示闹钟的序号（MENU2）
			}
			if(MENU2>=9 && MENU2<=10){ //显示温度控制TC1～2
				TM1640_display(0,25);	//TC1
				TM1640_display(1,22);
				TM1640_display(2,MENU2-8); //显示温度的序号（MENU2）
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//能让调时过程中时数一直显示
						MENU2++; //闹钟序号加1
						if(MENU2>10)MENU2=0;	//如果序号大于8则回到0
						if(MENU2==0){ //长按时的显示更新
							TM1640_display(0,23);	//SEt
							TM1640_display(1,24);
							TM1640_display(2,25); 
						}
						if(MENU2>=1){
							TM1640_display(0,26);	//AL1
							TM1640_display(1,27);
							TM1640_display(2,MENU2); 
						}
						delay_ms(KEY_SPEED2); //延时毫秒，延时值可决定长按的数据加减速度
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						MENU2++;  //闹钟序号加1
						if(MENU2>10)MENU2=0;//如果序号大于8则回到0
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//能让调时过程中时数一直显示
						if(MENU2==0)MENU2=11;
						MENU2--;
						if(MENU2==0){
							TM1640_display(0,23);	//SEt
							TM1640_display(1,24);
							TM1640_display(2,25); 
						}
						if(MENU2>=1){
							TM1640_display(0,26);	//AL1
							TM1640_display(1,27);
							TM1640_display(2,MENU2); 
						}
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						if(MENU2==0)MENU2=11;
						MENU2--;
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //读触摸按键的电平
				MENU=101+MENU2;	 //根据闹钟序号跳到对应的设置菜单
				BUZZER_BEEP1();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //读触摸按键的电平
				MENU=0; //返回主界面
				BUZZER_BEEP4();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
		//对进入设置项目的跳转
		if(MENU==101)MENU=4; //跳到RTC设置菜单SET
		if(MENU>=102 && MENU<110)MENU=150; //跑到闹钟设置菜单AL1~8
		if(MENU>=110 && MENU<112)MENU=210; //温度控制部分TC1～2
		//RTC调时从这里开始
		if(MENU==4){ //设置RTC 年
			i++; //用于显示闪烁的计时器
			if(i>700){
				TM1640_display(0,ryear%100/10);	//年  2018 
				TM1640_display(1,ryear%100%10);
			}else{
				TM1640_display(0,20);	//无 
				TM1640_display(1,20);
			}
			if(i>1400)i=0; //如果显示大于上限后清0
			TM1640_display(2,21);  // -
			TM1640_display(3,rmon/10);//月
			TM1640_display(4,rmon%10);	
			TM1640_display(5,21); // -
			TM1640_display(6,rday/10); //日
			TM1640_display(7,rday%10);
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//能让调时过程中时数一直显示
						ryear++; //年值加1
						if(ryear>2099)ryear=2000;
						TM1640_display(0,ryear%100/10);	//年  2018 
						TM1640_display(1,ryear%100%10);
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						ryear++;
						if(ryear>2099)ryear=2000;
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//能让调时过程中时数一直显示
						ryear--;
						if(ryear<2000)ryear=2099;
						TM1640_display(0,ryear%100/10);	//年  2018 
						TM1640_display(1,ryear%100%10);
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						ryear--;
						if(ryear<2000)ryear=2099;
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //读触摸按键的电平
				MENU=5;
				rup=1;//时间修改的标志位置1
				BUZZER_BEEP1();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //读触摸按键的电平
				MENU=0;
				rup=1;//时间修改的标志位置1
				BUZZER_BEEP4();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
		if(MENU==5){ //设置RTC 月
			i++;
			TM1640_display(0,ryear%100/10);	//年  2018 
			TM1640_display(1,ryear%100%10);
			TM1640_display(2,21);  // -
			if(i>700){
				TM1640_display(3,rmon/10);//月
				TM1640_display(4,rmon%10);	
			}else{
				TM1640_display(3,20);	//无 
				TM1640_display(4,20);
			}
			if(i>1400)i=0;
			TM1640_display(5,21); // -
			TM1640_display(6,rday/10); //日
			TM1640_display(7,rday%10);
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//能让调时过程中时数一直显示
						rmon++;
						if(rmon>12)rmon=1;
						TM1640_display(3,rmon/10);//月
						TM1640_display(4,rmon%10);	
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						rmon++;
						if(rmon>12)rmon=1;
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//能让调时过程中时数一直显示
						rmon--;
						if(rmon<1)rmon=12;
						TM1640_display(3,rmon/10);//月
						TM1640_display(4,rmon%10);	
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						rmon--;
						if(rmon<1)rmon=12;
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //读触摸按键的电平
				MENU=6;
				rup=1;//时间修改的标志位置1
				BUZZER_BEEP1();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //读触摸按键的电平
				MENU=0;
				rup=1;//时间修改的标志位置1
				BUZZER_BEEP4();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
		if(MENU==6){ //设置RTC 日
			i++;
			TM1640_display(0,ryear%100/10);	//年  2018 
			TM1640_display(1,ryear%100%10);
			TM1640_display(2,21);  // -
			TM1640_display(3,rmon/10);//月
			TM1640_display(4,rmon%10);	
			TM1640_display(5,21); // -
			if(i>700){
				TM1640_display(6,rday/10); //日
				TM1640_display(7,rday%10);
			}else{
				TM1640_display(6,20);	//无 
				TM1640_display(7,20);
			}
			if(i>1400)i=0;
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//能让调时过程中时数一直显示
						rday++;
						if(rday>31)rday=1;
						TM1640_display(6,rday/10); //日
						TM1640_display(7,rday%10);
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						rday++;
						if(rday>31)rday=1;
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//能让调时过程中时数一直显示
						rday--;
						if(rday<1)rday=31;
						TM1640_display(6,rday/10); //日
						TM1640_display(7,rday%10);
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						rday--;
						if(rday<1)rday=31;
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //读触摸按键的电平
				MENU=7;
				rup=1;//时间修改的标志位置1
				BUZZER_BEEP1();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //读触摸按键的电平
				MENU=0;
				rup=1;//时间修改的标志位置1
				BUZZER_BEEP4();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
		if(MENU==7){ //设置RTC 小时
			i++;
			TM1640_display(0,20);	//无
			TM1640_display(1,20);
			if(i>700){
				TM1640_display(2,rhour/10); //时
				TM1640_display(3,rhour%10+10);
			}else{
				TM1640_display(2,20);	//无 
				TM1640_display(3,20);
			}
			TM1640_display(4,rmin/10);	//分
			TM1640_display(5,rmin%10+10);
			TM1640_display(6,rsec/10); //秒
			TM1640_display(7,rsec%10);
			if(i>1400)i=0; //调节闪的速度

			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//能让调时过程中时数一直显示
						rhour++;
						if(rhour>23)rhour=0;
						TM1640_display(2,rhour/10); //时
						TM1640_display(3,rhour%10+10);
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						rhour++;
						if(rhour>23)rhour=0;
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//能让调时过程中时数一直显示
						if(rhour==0)rhour=24;
						rhour--;
						TM1640_display(2,rhour/10); //时
						TM1640_display(3,rhour%10+10);
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						if(rhour==0)rhour=24;
						rhour--;
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //读触摸按键的电平
				MENU=8;
				rup=1;//时间修改的标志位置1
				BUZZER_BEEP1();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //读触摸按键的电平
				MENU=1;
				rup=1;//时间修改的标志位置1
				BUZZER_BEEP4();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
		if(MENU==8){ //设置RTC 分
			i++;
			TM1640_display(0,20);	//无
			TM1640_display(1,20);
			TM1640_display(2,rhour/10); //时
			TM1640_display(3,rhour%10+10);
			if(i>700){
				TM1640_display(4,rmin/10);	//分
				TM1640_display(5,rmin%10+10);
			}else{
				TM1640_display(4,20);	//无 
				TM1640_display(5,20);
			}
			TM1640_display(6,rsec/10); //秒
			TM1640_display(7,rsec%10);
			if(i>1400)i=0; //调节闪的速度
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//能让调时过程中时数一直显示
						rmin++;
						if(rmin>59)rmin=0;
						rsec=0;
						TM1640_display(4,rmin/10);	//分
						TM1640_display(5,rmin%10+10);
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						rmin++;
						if(rmin>59)rmin=0;
						rsec=0;
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//能让调时过程中时数一直显示
						if(rmin==0)rmin=60;
						rmin--;
						rsec=0;
						TM1640_display(4,rmin/10);	//分
						TM1640_display(5,rmin%10+10);
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						if(rmin==0)rmin=60;
						rmin--;
						rsec=0;
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //读触摸按键的电平
				MENU=4;
				rup=1;//时间修改的标志位置1
				BUZZER_BEEP1();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //读触摸按键的电平
				MENU=1;
				rup=1;//时间修改的标志位置1
				BUZZER_BEEP4();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
//【8个闹钟设置部分】
		if(MENU==150){ //设置闹钟小时
			i++;
			TM1640_display(0,26);	//AL1～8
			TM1640_display(1,27);
			TM1640_display(2,MENU2); 
			if(i>700){
				if(alhour[MENU2-1]==24){
					TM1640_display(4,28); //OFF
					TM1640_display(5,29);
					TM1640_display(6,29);
				}else{
					TM1640_display(4,alhour[MENU2-1]/10); //时
					TM1640_display(5,alhour[MENU2-1]%10+10);
				}
			}else{
				if(alhour[MENU2-1]==24){
					TM1640_display(4,20); //OFF闪烁的无显示 
					TM1640_display(5,20);
					TM1640_display(6,20);
				}else{
					TM1640_display(4,20);	//无 
					TM1640_display(5,20);
				}
			}
			if(alhour[MENU2-1]==24){
				TM1640_display(7,20);	//分隐藏
			}else{
				TM1640_display(6,almin[MENU2-1]/10);	//分
				TM1640_display(7,almin[MENU2-1]%10);
			}
			if(i>1400)i=0; //调节闪的速度
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//能让调时过程中时数一直显示
						alhour[MENU2-1]++;
						if(alhour[MENU2-1]>24)alhour[MENU2-1]=0;
						TM1640_display(4,alhour[MENU2-1]/10); //时
						TM1640_display(5,alhour[MENU2-1]%10+10);
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						alhour[MENU2-1]++;
						if(alhour[MENU2-1]>24)alhour[MENU2-1]=0;
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//能让调时过程中时数一直显示
						if(alhour[MENU2-1]==0)alhour[MENU2-1]=25;
						alhour[MENU2-1]--;
						TM1640_display(4,alhour[MENU2-1]/10); //时
						TM1640_display(5,alhour[MENU2-1]%10+10);
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						if(alhour[MENU2-1]==0)alhour[MENU2-1]=25;
						alhour[MENU2-1]--;
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //读触摸按键的电平
				if(alhour[MENU2-1]!=24)MENU=151;
				BUZZER_BEEP1();//蜂鸣器音1
				ALFLASH_W(); //把数据写入到FLASH中
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //读触摸按键的电平
				MENU=1;
				rup=1;//时间修改的标志位置1
				ALFLASH_W(); //把数据写入到FLASH中
				BUZZER_BEEP4();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
		if(MENU==151){ //设置闹钟分钟
			i++;
			TM1640_display(0,26);	//AL1～8
			TM1640_display(1,27);
			TM1640_display(2,MENU2); 
			TM1640_display(4,alhour[MENU2-1]/10); //时
			TM1640_display(5,alhour[MENU2-1]%10+10);
			if(i>700){
				TM1640_display(6,almin[MENU2-1]/10);	//分
				TM1640_display(7,almin[MENU2-1]%10);
			}else{
				TM1640_display(6,20);	//无 
				TM1640_display(7,20);
			}
			if(i>1400)i=0; //调节闪的速度
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//能让调时过程中时数一直显示
						almin[MENU2-1]++;
						if(almin[MENU2-1]>59)almin[MENU2-1]=0;
						TM1640_display(6,almin[MENU2-1]/10);	//分
						TM1640_display(7,almin[MENU2-1]%10);
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						almin[MENU2-1]++;
						if(almin[MENU2-1]>59)almin[MENU2-1]=0;
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//能让调时过程中时数一直显示
						if(almin[MENU2-1]==0)almin[MENU2-1]=60;
						almin[MENU2-1]--;
						TM1640_display(6,almin[MENU2-1]/10);	//分
						TM1640_display(7,almin[MENU2-1]%10);
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						if(almin[MENU2-1]==0)almin[MENU2-1]=60;
						almin[MENU2-1]--;
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //读触摸按键的电平
				MENU=150;
				ALFLASH_W(); //把数据写入到FLASH中
				BUZZER_BEEP1();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //读触摸按键的电平
				MENU=1;
				rup=1;//时间修改的标志位置1
				ALFLASH_W(); //把数据写入到FLASH中
				BUZZER_BEEP4();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
//【闹钟处理部分】
		if(MENU<3 && m==0){ //闹钟触发到时判断程序
			for(a=0;a<8;a++){ //循环判断8个独立闹钟
				if(alhour[a]==rhour && almin[a]==rmin && rsec==0){	//判断同一个闹钟的小时和分钟
					m=1;   //执行闹钟到时的处理
					c=a; //把当前闹钟序号存入一个专用寄存器
					i=0;t=0; //计数器清0
					if(c==4||c==5||c==6||c==7){
						BUZZER_BEEP4();//蜂鸣器音1
						if(c==4)RELAY_1(1);//继电器的控制程序（0继电器放开,1继电器吸合）
						if(c==5)RELAY_1(0);//继电器的控制程序（0继电器放开,1继电器吸合）
						if(c==6)RELAY_2(1);//继电器的控制程序（0继电器放开,1继电器吸合）
						if(c==7)RELAY_2(0);//继电器的控制程序（0继电器放开,1继电器吸合）
						MENU=1; 
					}else{
						MENU=200; //跳到闹钟处理程序
					}
				}
			}
		}
		if(m==1){ //闹钟标志位到时后的闹钟呜响时间长度
			t++; //闹钟计数加1
			if(t>1000){ //菜单循环N次后退出（即闹钟呜响的总时长）
				MENU=1;	//退出闹钟回到主界面
				m=0; //闹钟标志位清0
				t=0; //计数器清0
			}
		}
		if(MENU==200){ //闹钟触发处理程序
			BUZZER_BEEP4();//蜂鸣器音1	
			i++; //闪烁计数器
			if(i>1){
				TM1640_display(0,26);	//AL1～8
				TM1640_display(1,27);
				TM1640_display(2,c+1); 
				TM1640_display(4,alhour[c]/10); //时
				TM1640_display(5,alhour[c]%10+10);
				TM1640_display(6,almin[c]/10);	//分
				TM1640_display(7,almin[c]%10);
			}else{
				TM1640_display(0,20);	//无 
				TM1640_display(1,20);
				TM1640_display(2,20);
				TM1640_display(3,20);
				TM1640_display(4,20);
				TM1640_display(5,20);
				TM1640_display(6,20);
				TM1640_display(7,20);
			}
			if(i>3)i=0; //调节闪的速度
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){ //读触摸按键的电平
				MENU=1;	//退出闹钟回到主界面
				m=0; //闹钟标志位清0
				BUZZER_BEEP1();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){ //读触摸按键的电平
				MENU=1;
				m=0; //闹钟标志位清0
				BUZZER_BEEP1();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //读触摸按键的电平
				MENU=1;
				m=0;
				BUZZER_BEEP1();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //读触摸按键的电平
				MENU=1;
				m=0;
				BUZZER_BEEP1();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
//【温度控制部分】
		if(MENU<3){ //闹钟触发到时判断程序
			for(a=8;a<10;a++){ //循环判断8个独立闹钟
				if((alhour[a]*10+almin[a])>=(buffer[1]%100*10+(buffer[2]%100/10))&&alhour[a]!=0){ //判断设置温度与当前温度的大小
					if(a==8 && h!=1){ //TC1
						h=1; //交替开关标志位
						BUZZER_BEEP4();//蜂鸣器音1
						RELAY_1(0);//继电器的控制程序（0继电器放开,1继电器吸合）
						MENU=1; //回到主界面 
					}
					if(a==9 && k!=1){ //TC2
						k=1; //交替开关标志位
						BUZZER_BEEP4();//蜂鸣器音1
						RELAY_2(1);//继电器的控制程序（0继电器放开,1继电器吸合）
						MENU=1; //回到主界面 
					}
				}
				if((alhour[a]*10+almin[a])<(buffer[1]%100*10+(buffer[2]%100/10))&&alhour[a]!=0){ //判断设置温度与当前温度的大小
					if(a==8&&h!=2){ //TC1
						h=2; //交替开关标志位
						BUZZER_BEEP4();//蜂鸣器音1
						RELAY_1(1);//继电器的控制程序（0继电器放开,1继电器吸合）
						MENU=1; //回到主界面
					}
					if(a==9 && k!=2){ //TC2
						k=2; //交替开关标志位
						BUZZER_BEEP4();//蜂鸣器音1
						RELAY_2(0);//继电器的控制程序（0继电器放开,1继电器吸合）
						MENU=1; //回到主界面
					}
				}
			}
		}
//【温度设置部分】
		if(MENU==210){ //设置温度整数
			i++;
			TM1640_display(0,25);	//TC1～2
			TM1640_display(1,22);
			TM1640_display(2,MENU2-8); 
			if(i>700){
				if(alhour[MENU2-1]==0){ //当温度值为0 则显示OFF
					TM1640_display(4,28); //OFF
					TM1640_display(5,29);
					TM1640_display(6,29);
					TM1640_display(7,20); //
				}else{
//					if(!buffer[0])TM1640_display(3,20);  //正负号
//					else TM1640_display(3,21);
					TM1640_display(4,alhour[MENU2-1]%100/10);	//温度整数 
					TM1640_display(5,alhour[MENU2-1]%10+10);
					TM1640_display(7,22); //c
				}
			}else{
				if(alhour[MENU2-1]==0){ //当温度值为0 则显示OFF的闪烁 关
					TM1640_display(4,20); //OFF闪烁的无显示 
					TM1640_display(5,20);
					TM1640_display(6,20);
				}else{
					TM1640_display(4,20);	//显示温度整数的闪烁 关 
					TM1640_display(5,20);
				}
			}
			if(alhour[MENU2-1]!=0){ //如果温度显示为OFF 则不显示小数值
				TM1640_display(6,almin[MENU2-1]%10); //
			}
			if(i>1400)i=0; //调节闪的速度
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//能让调时过程中时数一直显示
						alhour[MENU2-1]++;
						if(alhour[MENU2-1]>60)alhour[MENU2-1]=0;
						TM1640_display(4,alhour[MENU2-1]/10); //温度值整数部分刷新
						TM1640_display(5,alhour[MENU2-1]%10+10);
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						alhour[MENU2-1]++;
						if(alhour[MENU2-1]>60)alhour[MENU2-1]=0;
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//能让调时过程中时数一直显示
						if(alhour[MENU2-1]==0)alhour[MENU2-1]=61;
						alhour[MENU2-1]--;
						TM1640_display(4,alhour[MENU2-1]/10); //温度值整数部分刷新
						TM1640_display(5,alhour[MENU2-1]%10+10);
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						if(alhour[MENU2-1]==0)alhour[MENU2-1]=61;
						alhour[MENU2-1]--;
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //读触摸按键的电平
				if(alhour[MENU2-1]!=0)MENU=211;
				BUZZER_BEEP1();//蜂鸣器音1
				ALFLASH_W(); //把数据写入到FLASH中
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //读触摸按键的电平
				MENU=1;
				rup=1;//时间修改的标志位置1
				ALFLASH_W(); //把数据写入到FLASH中
				BUZZER_BEEP4();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
		if(MENU==211){ //设置温度的小数
			i++;
			TM1640_display(0,25);	//TC1～2
			TM1640_display(1,22);
			TM1640_display(2,MENU2-8);
			TM1640_display(7,22); //c
//			if(!buffer[0])TM1640_display(3,20);  //正负号
//			else TM1640_display(3,21);
			TM1640_display(4,alhour[MENU2-1]%100/10);	//温度 
			TM1640_display(5,alhour[MENU2-1]%10+10);
			if(i>700){
				TM1640_display(6,almin[MENU2-1]%10); //
			}else{
				TM1640_display(6,20);	//无 
			}
			if(i>1400)i=0; //调节闪的速度
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//能让调时过程中时数一直显示
						almin[MENU2-1]++;
						if(almin[MENU2-1]>9)almin[MENU2-1]=0; //值的限制
						TM1640_display(6,almin[MENU2-1]%10);	//温度小数部分刷新
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						almin[MENU2-1]++;
						if(almin[MENU2-1]>9)almin[MENU2-1]=0; //值的限制
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEY_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//能让调时过程中时数一直显示
						if(almin[MENU2-1]==0)almin[MENU2-1]=10;
						almin[MENU2-1]--;
						TM1640_display(6,almin[MENU2-1]%10);	//温度小数部分刷新
						delay_ms(KEY_SPEED2); //延时 毫秒
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}else{ //单击处理
					if(a==0){ //判断单击
						//单击后执行的程序放到此处
						i=701;//能让调时过程中时数一直显示
						if(almin[MENU2-1]==0)almin[MENU2-1]=10;
						almin[MENU2-1]--;
						BUZZER_BEEP1();//蜂鸣器音1
					}
				}
				a=0;c=0; //参数清0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //读触摸按键的电平
				MENU=210;
				ALFLASH_W(); //把数据写入到FLASH中
				BUZZER_BEEP1();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //读触摸按键的电平
				MENU=1;
				rup=1;//时间修改的标志位置1
				ALFLASH_W(); //把数据写入到FLASH中
				BUZZER_BEEP4();//蜂鸣器音1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
	}
}

/*********************************************************************************************
 * 杜洋工作室 www.DoYoung.net
 * 洋桃电子 www.DoYoung.net/YT 
*********************************************************************************************/
/*

【变量定义】
u32     a; //定义32位无符号变量a
u16     a; //定义16位无符号变量a
u8     a; //定义8位无符号变量a
vu32     a; //定义易变的32位无符号变量a
vu16     a; //定义易变的 16位无符号变量a
vu8     a; //定义易变的 8位无符号变量a
uc32     a; //定义只读的32位无符号变量a
uc16     a; //定义只读 的16位无符号变量a
uc8     a; //定义只读 的8位无符号变量a

#define ONE  1   //宏定义

delay_us(1); //延时1微秒
delay_ms(1); //延时1毫秒
delay_s(1); //延时1秒

GPIO_WriteBit(LEDPORT,LED1,(BitAction)(1)); //LED控制

*/



