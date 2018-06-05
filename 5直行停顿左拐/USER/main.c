#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "key.h"
#include "led.h"	 	
#include "stdlib.h"
#include "string.h"
#include "exti.h"
#include "timer.h"
#include "lcd.h"
#include "Motor.h"
#include "Infrared.h"
#include "Encoder.h"
#include "Infrared.h"
#include "Telecontrol.h"
#include "beep.h"
extern u16 timer;    
extern u8 flag;
extern int left_numb;
extern int right_numb;
int main(void)
{	 

	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
//	usmart_dev.init(72); 	//��ʼ��USMART	1
	LED_Init();			     //LED�˿ڳ�ʼ��  
	KEY_Init();					//��ʼ������
	BEEP_Init();
//	Driver_Init();			//��������ʼ��
//	Driver_Init1();			//��������ʼ��
//	TIM8_OPM_RCR_Init(999,72-1); //1MHz����Ƶ��  ������+�ظ�����ģʽ	   
//	TIM1_OPM_RCR_Init(999,72-1);

//	EXTIX_Init();
	LCD_Init();
  Motor_init();
	Telecontrol_exti();
  Encoder_exti();


//	TIM3_Int_Init(9,7199);//10Ϊ1ms  

//	LCD_Init();
	//POINT_COLOR=RED;
//	TIM_Cmd(TIM3, ENABLE);;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;ssssssssssssssssssssssssssssssssssssssssssssssss
// TIM_Cmd(TIM3, DISABLE);
 forward();//1
 while((left_numb<100)&&(right_numb<70));
 stop();
// delay_nms(1000);
// stop();
 BEEP=1;
 delay_nms(500);
 BEEP=0;
 
 
 while(right_numb<120){forward();}
 stop();
 BEEP=1;
 delay_nms(500);
 BEEP=0;
 while(right_numb<139){circle_left();}
 stop();
 BEEP=1;
 delay_nms(500);
 BEEP=0;
// while(right_numb<270){forward();}//ʵ��
 while(right_numb<200){forward();}
 stop();
 BEEP=1;
 delay_nms(500);
 BEEP=0;
// while(right_numb<){forward();}
// Infrared_init();
 
// BLACK_LED_Init();
 
	while(1) 
	{		 	  
//right_turn();
//		circle_right();
/*	  if(flag == 1)
  {
   process();
   flag = 0;
  }
*/	 	
/*		if(BZ_LEFT==0)LED0=0;else LED0=1;
		if(BZ_RIGHT==0)LED1=0;else LED1=1;
*/		
	//	BLACK_LED=!BLACK_LED;
		delay_ms(1000);
		
   }
	}
