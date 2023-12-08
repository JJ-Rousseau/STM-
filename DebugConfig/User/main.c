/*********************************************************************************************
ģ��������  ��������/���ҵ���
��������	������Ŀ��--��Ŀ1������ʱ����չ��Ŀ
��д�ˣ�	����	
��дʱ�䣺	2018��10��25��
Ӳ��֧�֣�	STM32F103C8   �ⲿ����8MHz RCC����������Ƶ72MHz��  

�޸���־������
1-201810251645 ��ʼ��̡���������ʾ����17��ѹ������
2-201811142351 �����8�����ӵ�Сʱ�������ý��棬�Ż���ע�͡�
3-201811180113 ��������ӵĵ�ʱ����Ч�����Ż���ע�ͣ����ӹ���ȫ����ɡ�
4-201811192314 ��������ӿ���2·�̵����ĳ���
5-201811220021 �����¶ȿ��Ƽ̵������򣬵�δ��ɵ��¶Ȳ��Եĳ��򲿷֡� 	
6-201811240150 ����¶ȿ��Ƽ̵�����ȫ�������Ż���ע�͡�����Ŀ1��ɡ�	
							
˵����
 # ��ģ�������STM32F103�ڲ���RCCʱ�����ã������������õδ�ʱ������ʱ������
 # �ɸ����Լ�����Ҫ���ӻ�ɾ����

*********************************************************************************************/
#include "stm32f10x.h" //STM32ͷ�ļ�
#include "sys.h"
#include "delay.h"
#include "rtc.h"
#include "TM1640.h"
#include "buzzer.h"
#include "flash.h" 

#include "lm75a.h"
#include "touch_key.h"
#include "relay.h"


int main (void){//������
	u8 MENU;//�˵�ֵ
	u16 i;
	u16 t;
	u8 h,k; //�¶��ж��ý��濪�ر�־λ
	u8 m; //���Ӵ�����־λ��Ϊ1ʱ������0ʱ������
	u8 a=0,c=0; //ͨ�õı���
	u8 MENU2; //������������Ų˵�����1��8��
	u8 rup;//ʱ����±�־λ
	u8 buffer[3]; //�¶����ݼĴ���
	RCC_Configuration(); //ϵͳʱ�ӳ�ʼ��
	RELAY_Init();//�̵�����ʼ�� 
	delay_ms(200); //��ʱ����
	RTC_Config();  //RTC��ʼ��
	TM1640_Init(); //TM1640��ʼ��
	I2C_Configuration();//I2C��ʼ��
	LM75A_GetTemp(buffer); //��ȡLM75A���¶�����
	TOUCH_KEY_Init();//������ʼ��
	BUZZER_Init();//��������ʼ��
	BUZZER_BEEP2();//��������1
	MENU=0;	 //�˵���ʼֵΪ0
	rup=0; //RTCʱ�����ñ�־λ��Ϊ1ʱ����RTCʱ��
	t=0; //���ֱ�����ʼ��
	m=0;
	h=0;
	k=0;
	RELAY_1(0);//�̵����Ŀ��Ƴ���0�̵����ſ�,1�̵������ϣ�
	RELAY_2(0);//�̵����Ŀ��Ƴ���0�̵����ſ�,1�̵������ϣ�
	//��FLASH����
	for(i=0;i<10;i++){  //��8���������ݴ�FLASH������
 		t = FLASH_R(FLASH_START_ADDR+i*2);//��ָ��ҳ�ĵ�ַ��FLASH
		alhour[i]=t/0x100; //��FLASH������16λ���ַ���2��8λ����
		almin[i]=t%0x100;
	}
	if(alhour[0]>24){ //�жϣ��������1��Сʱֵ����24�ͳ�ʼ�����ӵ���������
		for(i=0;i<10;i++){
			if(i<8)	alhour[i]=24; //���ӵ�Сʱ�����ӵ�24СʱΪ�����ӣ�
			else alhour[i]=0;
			almin[i]=0;  //���ӵķ���
		}
		ALFLASH_W(); //�����ӵ���������д�뵽FLASH
	}
	while(1){  //��ѭ��
//������ˢ�����������л���
		if(rup==1){	//�ж�����ʱ���־λ
			rup=0; //��־λ��0
			RTC_Set(ryear,rmon,rday,rhour,rmin,rsec); //����ʱ�䣬������д�ص�RTC
		}
		if(MENU<3 || MENU >8){ //����˵�ֵ����RTCʱ�����ò˵�ʱ�ɸ���ʱ��
			if(MENU==0)LM75A_GetTemp(buffer); //��ȡLM75A���¶�����
			RTC_Get(); //����RTCʱ��
		}
		//��ʾ�����л��Ŀ��Ƴ���
		if(MENU<3){//ֻ���л���3���˵�����ã������ڵ�ʱ�˵�
			t++;
			if(t==1)MENU=1;	 //����t��ֵ�л��˵�
			if(t==5000)MENU=0;
			if(t==8000)MENU=2;
			if(t>=11000)t=0; //t��ֵ�������0
		}
		if(MENU<3){ //��ʾ�л���3���˵�
			if(MENU==0){
				TM1640_display(0,ryear%100/10);	//�� 
				TM1640_display(1,ryear%100%10);
				TM1640_display(2,21);  // -
				TM1640_display(3,rmon/10);//��
				TM1640_display(4,rmon%10);	
				TM1640_display(5,21); // -
				TM1640_display(6,rday/10); //��
				TM1640_display(7,rday%10);
			}else if(MENU==1){
				TM1640_display(0,20);	//��
				TM1640_display(1,20);
				TM1640_display(2,rhour/10); //ʱ
				TM1640_display(3,rhour%10+10);
				TM1640_display(4,rmin/10);	//��
				TM1640_display(5,rmin%10+10);
				TM1640_display(6,rsec/10); //��
				TM1640_display(7,rsec%10);
			}else{
				TM1640_display(0,20);	//��
				TM1640_display(1,20);
				TM1640_display(2,20); 
				if(!buffer[0])TM1640_display(3,20);  //������
				else TM1640_display(3,21);
				TM1640_display(4,buffer[1]%100/10);	//�¶� 
				TM1640_display(5,buffer[1]%10+10);
				TM1640_display(6,buffer[2]%100/10); //
				TM1640_display(7,22); //c
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){ //�����������ĵ�ƽ
				MENU=0;
				BUZZER_BEEP1();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){ //�����������ĵ�ƽ
				MENU=1;
				BUZZER_BEEP1();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //�����������ĵ�ƽ
				MENU=2;
				BUZZER_BEEP1();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //�����������ĵ�ƽ
				MENU=3;
				BUZZER_BEEP1();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
//����ʱ�˵����֡�
		if(MENU==3){ //SEt ����RTCʱ��Ĳ˵�����3��8
			TM1640_display(0,23);	//SEt
			TM1640_display(1,24);
			TM1640_display(2,25); 
			TM1640_display(3,20);  //��
			TM1640_display(4,20);	
			TM1640_display(5,20);
			TM1640_display(6,20); //
			TM1640_display(7,20); //
			BUZZER_BEEP3();//��������1
			MENU=100;
			MENU2=0;
		}
		if(MENU==100){ //���ܲ˵��л�
			if(MENU2==0){ //��ʾSET
				TM1640_display(0,23);	//SEt
				TM1640_display(1,24);
				TM1640_display(2,25); 
			}
			if(MENU2>=1 && MENU2<9){ //��ʾAL1��8 ����ֵ
				TM1640_display(0,26);	//AL1
				TM1640_display(1,27);
				TM1640_display(2,MENU2); //��ʾ���ӵ���ţ�MENU2��
			}
			if(MENU2>=9 && MENU2<=10){ //��ʾ�¶ȿ���TC1��2
				TM1640_display(0,25);	//TC1
				TM1640_display(1,22);
				TM1640_display(2,MENU2-8); //��ʾ�¶ȵ���ţ�MENU2��
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						MENU2++; //������ż�1
						if(MENU2>10)MENU2=0;	//�����Ŵ���8��ص�0
						if(MENU2==0){ //����ʱ����ʾ����
							TM1640_display(0,23);	//SEt
							TM1640_display(1,24);
							TM1640_display(2,25); 
						}
						if(MENU2>=1){
							TM1640_display(0,26);	//AL1
							TM1640_display(1,27);
							TM1640_display(2,MENU2); 
						}
						delay_ms(KEY_SPEED2); //��ʱ���룬��ʱֵ�ɾ������������ݼӼ��ٶ�
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						MENU2++;  //������ż�1
						if(MENU2>10)MENU2=0;//�����Ŵ���8��ص�0
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
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
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						if(MENU2==0)MENU2=11;
						MENU2--;
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //�����������ĵ�ƽ
				MENU=101+MENU2;	 //�����������������Ӧ�����ò˵�
				BUZZER_BEEP1();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //�����������ĵ�ƽ
				MENU=0; //����������
				BUZZER_BEEP4();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
		//�Խ���������Ŀ����ת
		if(MENU==101)MENU=4; //����RTC���ò˵�SET
		if(MENU>=102 && MENU<110)MENU=150; //�ܵ��������ò˵�AL1~8
		if(MENU>=110 && MENU<112)MENU=210; //�¶ȿ��Ʋ���TC1��2
		//RTC��ʱ�����￪ʼ
		if(MENU==4){ //����RTC ��
			i++; //������ʾ��˸�ļ�ʱ��
			if(i>700){
				TM1640_display(0,ryear%100/10);	//��  2018 
				TM1640_display(1,ryear%100%10);
			}else{
				TM1640_display(0,20);	//�� 
				TM1640_display(1,20);
			}
			if(i>1400)i=0; //�����ʾ�������޺���0
			TM1640_display(2,21);  // -
			TM1640_display(3,rmon/10);//��
			TM1640_display(4,rmon%10);	
			TM1640_display(5,21); // -
			TM1640_display(6,rday/10); //��
			TM1640_display(7,rday%10);
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						ryear++; //��ֵ��1
						if(ryear>2099)ryear=2000;
						TM1640_display(0,ryear%100/10);	//��  2018 
						TM1640_display(1,ryear%100%10);
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						ryear++;
						if(ryear>2099)ryear=2000;
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						ryear--;
						if(ryear<2000)ryear=2099;
						TM1640_display(0,ryear%100/10);	//��  2018 
						TM1640_display(1,ryear%100%10);
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						ryear--;
						if(ryear<2000)ryear=2099;
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //�����������ĵ�ƽ
				MENU=5;
				rup=1;//ʱ���޸ĵı�־λ��1
				BUZZER_BEEP1();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //�����������ĵ�ƽ
				MENU=0;
				rup=1;//ʱ���޸ĵı�־λ��1
				BUZZER_BEEP4();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
		if(MENU==5){ //����RTC ��
			i++;
			TM1640_display(0,ryear%100/10);	//��  2018 
			TM1640_display(1,ryear%100%10);
			TM1640_display(2,21);  // -
			if(i>700){
				TM1640_display(3,rmon/10);//��
				TM1640_display(4,rmon%10);	
			}else{
				TM1640_display(3,20);	//�� 
				TM1640_display(4,20);
			}
			if(i>1400)i=0;
			TM1640_display(5,21); // -
			TM1640_display(6,rday/10); //��
			TM1640_display(7,rday%10);
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						rmon++;
						if(rmon>12)rmon=1;
						TM1640_display(3,rmon/10);//��
						TM1640_display(4,rmon%10);	
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						rmon++;
						if(rmon>12)rmon=1;
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						rmon--;
						if(rmon<1)rmon=12;
						TM1640_display(3,rmon/10);//��
						TM1640_display(4,rmon%10);	
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						rmon--;
						if(rmon<1)rmon=12;
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //�����������ĵ�ƽ
				MENU=6;
				rup=1;//ʱ���޸ĵı�־λ��1
				BUZZER_BEEP1();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //�����������ĵ�ƽ
				MENU=0;
				rup=1;//ʱ���޸ĵı�־λ��1
				BUZZER_BEEP4();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
		if(MENU==6){ //����RTC ��
			i++;
			TM1640_display(0,ryear%100/10);	//��  2018 
			TM1640_display(1,ryear%100%10);
			TM1640_display(2,21);  // -
			TM1640_display(3,rmon/10);//��
			TM1640_display(4,rmon%10);	
			TM1640_display(5,21); // -
			if(i>700){
				TM1640_display(6,rday/10); //��
				TM1640_display(7,rday%10);
			}else{
				TM1640_display(6,20);	//�� 
				TM1640_display(7,20);
			}
			if(i>1400)i=0;
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						rday++;
						if(rday>31)rday=1;
						TM1640_display(6,rday/10); //��
						TM1640_display(7,rday%10);
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						rday++;
						if(rday>31)rday=1;
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						rday--;
						if(rday<1)rday=31;
						TM1640_display(6,rday/10); //��
						TM1640_display(7,rday%10);
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						rday--;
						if(rday<1)rday=31;
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //�����������ĵ�ƽ
				MENU=7;
				rup=1;//ʱ���޸ĵı�־λ��1
				BUZZER_BEEP1();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //�����������ĵ�ƽ
				MENU=0;
				rup=1;//ʱ���޸ĵı�־λ��1
				BUZZER_BEEP4();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
		if(MENU==7){ //����RTC Сʱ
			i++;
			TM1640_display(0,20);	//��
			TM1640_display(1,20);
			if(i>700){
				TM1640_display(2,rhour/10); //ʱ
				TM1640_display(3,rhour%10+10);
			}else{
				TM1640_display(2,20);	//�� 
				TM1640_display(3,20);
			}
			TM1640_display(4,rmin/10);	//��
			TM1640_display(5,rmin%10+10);
			TM1640_display(6,rsec/10); //��
			TM1640_display(7,rsec%10);
			if(i>1400)i=0; //���������ٶ�

			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						rhour++;
						if(rhour>23)rhour=0;
						TM1640_display(2,rhour/10); //ʱ
						TM1640_display(3,rhour%10+10);
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						rhour++;
						if(rhour>23)rhour=0;
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						if(rhour==0)rhour=24;
						rhour--;
						TM1640_display(2,rhour/10); //ʱ
						TM1640_display(3,rhour%10+10);
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						if(rhour==0)rhour=24;
						rhour--;
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //�����������ĵ�ƽ
				MENU=8;
				rup=1;//ʱ���޸ĵı�־λ��1
				BUZZER_BEEP1();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //�����������ĵ�ƽ
				MENU=1;
				rup=1;//ʱ���޸ĵı�־λ��1
				BUZZER_BEEP4();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
		if(MENU==8){ //����RTC ��
			i++;
			TM1640_display(0,20);	//��
			TM1640_display(1,20);
			TM1640_display(2,rhour/10); //ʱ
			TM1640_display(3,rhour%10+10);
			if(i>700){
				TM1640_display(4,rmin/10);	//��
				TM1640_display(5,rmin%10+10);
			}else{
				TM1640_display(4,20);	//�� 
				TM1640_display(5,20);
			}
			TM1640_display(6,rsec/10); //��
			TM1640_display(7,rsec%10);
			if(i>1400)i=0; //���������ٶ�
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						rmin++;
						if(rmin>59)rmin=0;
						rsec=0;
						TM1640_display(4,rmin/10);	//��
						TM1640_display(5,rmin%10+10);
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						rmin++;
						if(rmin>59)rmin=0;
						rsec=0;
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						if(rmin==0)rmin=60;
						rmin--;
						rsec=0;
						TM1640_display(4,rmin/10);	//��
						TM1640_display(5,rmin%10+10);
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						if(rmin==0)rmin=60;
						rmin--;
						rsec=0;
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //�����������ĵ�ƽ
				MENU=4;
				rup=1;//ʱ���޸ĵı�־λ��1
				BUZZER_BEEP1();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //�����������ĵ�ƽ
				MENU=1;
				rup=1;//ʱ���޸ĵı�־λ��1
				BUZZER_BEEP4();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
//��8���������ò��֡�
		if(MENU==150){ //��������Сʱ
			i++;
			TM1640_display(0,26);	//AL1��8
			TM1640_display(1,27);
			TM1640_display(2,MENU2); 
			if(i>700){
				if(alhour[MENU2-1]==24){
					TM1640_display(4,28); //OFF
					TM1640_display(5,29);
					TM1640_display(6,29);
				}else{
					TM1640_display(4,alhour[MENU2-1]/10); //ʱ
					TM1640_display(5,alhour[MENU2-1]%10+10);
				}
			}else{
				if(alhour[MENU2-1]==24){
					TM1640_display(4,20); //OFF��˸������ʾ 
					TM1640_display(5,20);
					TM1640_display(6,20);
				}else{
					TM1640_display(4,20);	//�� 
					TM1640_display(5,20);
				}
			}
			if(alhour[MENU2-1]==24){
				TM1640_display(7,20);	//������
			}else{
				TM1640_display(6,almin[MENU2-1]/10);	//��
				TM1640_display(7,almin[MENU2-1]%10);
			}
			if(i>1400)i=0; //���������ٶ�
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						alhour[MENU2-1]++;
						if(alhour[MENU2-1]>24)alhour[MENU2-1]=0;
						TM1640_display(4,alhour[MENU2-1]/10); //ʱ
						TM1640_display(5,alhour[MENU2-1]%10+10);
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						alhour[MENU2-1]++;
						if(alhour[MENU2-1]>24)alhour[MENU2-1]=0;
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						if(alhour[MENU2-1]==0)alhour[MENU2-1]=25;
						alhour[MENU2-1]--;
						TM1640_display(4,alhour[MENU2-1]/10); //ʱ
						TM1640_display(5,alhour[MENU2-1]%10+10);
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						if(alhour[MENU2-1]==0)alhour[MENU2-1]=25;
						alhour[MENU2-1]--;
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //�����������ĵ�ƽ
				if(alhour[MENU2-1]!=24)MENU=151;
				BUZZER_BEEP1();//��������1
				ALFLASH_W(); //������д�뵽FLASH��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //�����������ĵ�ƽ
				MENU=1;
				rup=1;//ʱ���޸ĵı�־λ��1
				ALFLASH_W(); //������д�뵽FLASH��
				BUZZER_BEEP4();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
		if(MENU==151){ //�������ӷ���
			i++;
			TM1640_display(0,26);	//AL1��8
			TM1640_display(1,27);
			TM1640_display(2,MENU2); 
			TM1640_display(4,alhour[MENU2-1]/10); //ʱ
			TM1640_display(5,alhour[MENU2-1]%10+10);
			if(i>700){
				TM1640_display(6,almin[MENU2-1]/10);	//��
				TM1640_display(7,almin[MENU2-1]%10);
			}else{
				TM1640_display(6,20);	//�� 
				TM1640_display(7,20);
			}
			if(i>1400)i=0; //���������ٶ�
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						almin[MENU2-1]++;
						if(almin[MENU2-1]>59)almin[MENU2-1]=0;
						TM1640_display(6,almin[MENU2-1]/10);	//��
						TM1640_display(7,almin[MENU2-1]%10);
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						almin[MENU2-1]++;
						if(almin[MENU2-1]>59)almin[MENU2-1]=0;
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						if(almin[MENU2-1]==0)almin[MENU2-1]=60;
						almin[MENU2-1]--;
						TM1640_display(6,almin[MENU2-1]/10);	//��
						TM1640_display(7,almin[MENU2-1]%10);
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						if(almin[MENU2-1]==0)almin[MENU2-1]=60;
						almin[MENU2-1]--;
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //�����������ĵ�ƽ
				MENU=150;
				ALFLASH_W(); //������д�뵽FLASH��
				BUZZER_BEEP1();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //�����������ĵ�ƽ
				MENU=1;
				rup=1;//ʱ���޸ĵı�־λ��1
				ALFLASH_W(); //������д�뵽FLASH��
				BUZZER_BEEP4();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
//�����Ӵ����֡�
		if(MENU<3 && m==0){ //���Ӵ�����ʱ�жϳ���
			for(a=0;a<8;a++){ //ѭ���ж�8����������
				if(alhour[a]==rhour && almin[a]==rmin && rsec==0){	//�ж�ͬһ�����ӵ�Сʱ�ͷ���
					m=1;   //ִ�����ӵ�ʱ�Ĵ���
					c=a; //�ѵ�ǰ������Ŵ���һ��ר�üĴ���
					i=0;t=0; //��������0
					if(c==4||c==5||c==6||c==7){
						BUZZER_BEEP4();//��������1
						if(c==4)RELAY_1(1);//�̵����Ŀ��Ƴ���0�̵����ſ�,1�̵������ϣ�
						if(c==5)RELAY_1(0);//�̵����Ŀ��Ƴ���0�̵����ſ�,1�̵������ϣ�
						if(c==6)RELAY_2(1);//�̵����Ŀ��Ƴ���0�̵����ſ�,1�̵������ϣ�
						if(c==7)RELAY_2(0);//�̵����Ŀ��Ƴ���0�̵����ſ�,1�̵������ϣ�
						MENU=1; 
					}else{
						MENU=200; //�������Ӵ������
					}
				}
			}
		}
		if(m==1){ //���ӱ�־λ��ʱ�����������ʱ�䳤��
			t++; //���Ӽ�����1
			if(t>1000){ //�˵�ѭ��N�κ��˳����������������ʱ����
				MENU=1;	//�˳����ӻص�������
				m=0; //���ӱ�־λ��0
				t=0; //��������0
			}
		}
		if(MENU==200){ //���Ӵ����������
			BUZZER_BEEP4();//��������1	
			i++; //��˸������
			if(i>1){
				TM1640_display(0,26);	//AL1��8
				TM1640_display(1,27);
				TM1640_display(2,c+1); 
				TM1640_display(4,alhour[c]/10); //ʱ
				TM1640_display(5,alhour[c]%10+10);
				TM1640_display(6,almin[c]/10);	//��
				TM1640_display(7,almin[c]%10);
			}else{
				TM1640_display(0,20);	//�� 
				TM1640_display(1,20);
				TM1640_display(2,20);
				TM1640_display(3,20);
				TM1640_display(4,20);
				TM1640_display(5,20);
				TM1640_display(6,20);
				TM1640_display(7,20);
			}
			if(i>3)i=0; //���������ٶ�
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){ //�����������ĵ�ƽ
				MENU=1;	//�˳����ӻص�������
				m=0; //���ӱ�־λ��0
				BUZZER_BEEP1();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){ //�����������ĵ�ƽ
				MENU=1;
				m=0; //���ӱ�־λ��0
				BUZZER_BEEP1();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //�����������ĵ�ƽ
				MENU=1;
				m=0;
				BUZZER_BEEP1();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //�����������ĵ�ƽ
				MENU=1;
				m=0;
				BUZZER_BEEP1();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
//���¶ȿ��Ʋ��֡�
		if(MENU<3){ //���Ӵ�����ʱ�жϳ���
			for(a=8;a<10;a++){ //ѭ���ж�8����������
				if((alhour[a]*10+almin[a])>=(buffer[1]%100*10+(buffer[2]%100/10))&&alhour[a]!=0){ //�ж������¶��뵱ǰ�¶ȵĴ�С
					if(a==8 && h!=1){ //TC1
						h=1; //���濪�ر�־λ
						BUZZER_BEEP4();//��������1
						RELAY_1(0);//�̵����Ŀ��Ƴ���0�̵����ſ�,1�̵������ϣ�
						MENU=1; //�ص������� 
					}
					if(a==9 && k!=1){ //TC2
						k=1; //���濪�ر�־λ
						BUZZER_BEEP4();//��������1
						RELAY_2(1);//�̵����Ŀ��Ƴ���0�̵����ſ�,1�̵������ϣ�
						MENU=1; //�ص������� 
					}
				}
				if((alhour[a]*10+almin[a])<(buffer[1]%100*10+(buffer[2]%100/10))&&alhour[a]!=0){ //�ж������¶��뵱ǰ�¶ȵĴ�С
					if(a==8&&h!=2){ //TC1
						h=2; //���濪�ر�־λ
						BUZZER_BEEP4();//��������1
						RELAY_1(1);//�̵����Ŀ��Ƴ���0�̵����ſ�,1�̵������ϣ�
						MENU=1; //�ص�������
					}
					if(a==9 && k!=2){ //TC2
						k=2; //���濪�ر�־λ
						BUZZER_BEEP4();//��������1
						RELAY_2(0);//�̵����Ŀ��Ƴ���0�̵����ſ�,1�̵������ϣ�
						MENU=1; //�ص�������
					}
				}
			}
		}
//���¶����ò��֡�
		if(MENU==210){ //�����¶�����
			i++;
			TM1640_display(0,25);	//TC1��2
			TM1640_display(1,22);
			TM1640_display(2,MENU2-8); 
			if(i>700){
				if(alhour[MENU2-1]==0){ //���¶�ֵΪ0 ����ʾOFF
					TM1640_display(4,28); //OFF
					TM1640_display(5,29);
					TM1640_display(6,29);
					TM1640_display(7,20); //
				}else{
//					if(!buffer[0])TM1640_display(3,20);  //������
//					else TM1640_display(3,21);
					TM1640_display(4,alhour[MENU2-1]%100/10);	//�¶����� 
					TM1640_display(5,alhour[MENU2-1]%10+10);
					TM1640_display(7,22); //c
				}
			}else{
				if(alhour[MENU2-1]==0){ //���¶�ֵΪ0 ����ʾOFF����˸ ��
					TM1640_display(4,20); //OFF��˸������ʾ 
					TM1640_display(5,20);
					TM1640_display(6,20);
				}else{
					TM1640_display(4,20);	//��ʾ�¶���������˸ �� 
					TM1640_display(5,20);
				}
			}
			if(alhour[MENU2-1]!=0){ //����¶���ʾΪOFF ����ʾС��ֵ
				TM1640_display(6,almin[MENU2-1]%10); //
			}
			if(i>1400)i=0; //���������ٶ�
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						alhour[MENU2-1]++;
						if(alhour[MENU2-1]>60)alhour[MENU2-1]=0;
						TM1640_display(4,alhour[MENU2-1]/10); //�¶�ֵ��������ˢ��
						TM1640_display(5,alhour[MENU2-1]%10+10);
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						alhour[MENU2-1]++;
						if(alhour[MENU2-1]>60)alhour[MENU2-1]=0;
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						if(alhour[MENU2-1]==0)alhour[MENU2-1]=61;
						alhour[MENU2-1]--;
						TM1640_display(4,alhour[MENU2-1]/10); //�¶�ֵ��������ˢ��
						TM1640_display(5,alhour[MENU2-1]%10+10);
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						if(alhour[MENU2-1]==0)alhour[MENU2-1]=61;
						alhour[MENU2-1]--;
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //�����������ĵ�ƽ
				if(alhour[MENU2-1]!=0)MENU=211;
				BUZZER_BEEP1();//��������1
				ALFLASH_W(); //������д�뵽FLASH��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //�����������ĵ�ƽ
				MENU=1;
				rup=1;//ʱ���޸ĵı�־λ��1
				ALFLASH_W(); //������д�뵽FLASH��
				BUZZER_BEEP4();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
		if(MENU==211){ //�����¶ȵ�С��
			i++;
			TM1640_display(0,25);	//TC1��2
			TM1640_display(1,22);
			TM1640_display(2,MENU2-8);
			TM1640_display(7,22); //c
//			if(!buffer[0])TM1640_display(3,20);  //������
//			else TM1640_display(3,21);
			TM1640_display(4,alhour[MENU2-1]%100/10);	//�¶� 
			TM1640_display(5,alhour[MENU2-1]%10+10);
			if(i>700){
				TM1640_display(6,almin[MENU2-1]%10); //
			}else{
				TM1640_display(6,20);	//�� 
			}
			if(i>1400)i=0; //���������ٶ�
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						almin[MENU2-1]++;
						if(almin[MENU2-1]>9)almin[MENU2-1]=0; //ֵ������
						TM1640_display(6,almin[MENU2-1]%10);	//�¶�С������ˢ��
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						almin[MENU2-1]++;
						if(almin[MENU2-1]>9)almin[MENU2-1]=0; //ֵ������
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//�жϳ��̼�
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEY_SPEED1){ //ѭ���жϳ�������ʱ��ת
					c++;delay_ms(10); //�����жϵļ�ʱ
				}
				if(c>=KEY_SPEED1){ //��������
					//������ִ�еĳ���ŵ��˴�
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						if(almin[MENU2-1]==0)almin[MENU2-1]=10;
						almin[MENU2-1]--;
						TM1640_display(6,almin[MENU2-1]%10);	//�¶�С������ˢ��
						delay_ms(KEY_SPEED2); //��ʱ ����
						BUZZER_BEEP1();//��������1
					}
				}else{ //��������
					if(a==0){ //�жϵ���
						//������ִ�еĳ���ŵ��˴�
						i=701;//���õ�ʱ������ʱ��һֱ��ʾ
						if(almin[MENU2-1]==0)almin[MENU2-1]=10;
						almin[MENU2-1]--;
						BUZZER_BEEP1();//��������1
					}
				}
				a=0;c=0; //������0
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //�����������ĵ�ƽ
				MENU=210;
				ALFLASH_W(); //������д�뵽FLASH��
				BUZZER_BEEP1();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	 		}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //�����������ĵ�ƽ
				MENU=1;
				rup=1;//ʱ���޸ĵı�־λ��1
				ALFLASH_W(); //������д�뵽FLASH��
				BUZZER_BEEP4();//��������1
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	 		}
		}
	}
}

/*********************************************************************************************
 * �������� www.DoYoung.net
 * ���ҵ��� www.DoYoung.net/YT 
*********************************************************************************************/
/*

���������塿
u32     a; //����32λ�޷��ű���a
u16     a; //����16λ�޷��ű���a
u8     a; //����8λ�޷��ű���a
vu32     a; //�����ױ��32λ�޷��ű���a
vu16     a; //�����ױ�� 16λ�޷��ű���a
vu8     a; //�����ױ�� 8λ�޷��ű���a
uc32     a; //����ֻ����32λ�޷��ű���a
uc16     a; //����ֻ�� ��16λ�޷��ű���a
uc8     a; //����ֻ�� ��8λ�޷��ű���a

#define ONE  1   //�궨��

delay_us(1); //��ʱ1΢��
delay_ms(1); //��ʱ1����
delay_s(1); //��ʱ1��

GPIO_WriteBit(LEDPORT,LED1,(BitAction)(1)); //LED����

*/



