#ifndef __TOUCH_KEY_H
#define __TOUCH_KEY_H	 
#include "sys.h"

#define TOUCH_KEYPORT	GPIOA	//����IO�ӿ���
#define TOUCH_KEY_A		GPIO_Pin_0	//����IO�ӿ�
#define TOUCH_KEY_B		GPIO_Pin_1	//����IO�ӿ�
#define TOUCH_KEY_C		GPIO_Pin_2	//����IO�ӿ�
#define TOUCH_KEY_D		GPIO_Pin_3	//����IO�ӿ�

#define KEY_SPEED1	100	  //������ʱ�䳤�ȣ���λ10mS��
#define KEY_SPEED2	80	  //�Ӽ���ֵ����ʾˢ���ٶ�

void TOUCH_KEY_Init(void);//��ʼ��

		 				    
#endif
