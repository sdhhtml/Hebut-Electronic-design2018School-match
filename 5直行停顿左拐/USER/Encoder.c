#include "stm32f10x.h"
#include "Encoder.h"
#include "sys.h" 
#include "usart.h"
#include "lcd.h"
#include "delay.h"
#include "lcd.h"
extern u32 left_numb;
extern u32 right_numb;

u32 left_speed;
u32 right_speed;
void Encoder_init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF, ENABLE);	 //使能PB,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	    		 //LED1-->PE.5 端口配置, 推挽输出
 GPIO_Init(GPIOF, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
// GPIO_SetBits(GPIOE,GPIO_Pin_0); 						 //PE.5 输出高 
	
}
void Encoder_exti(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	Encoder_init();
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟
	
	
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOF,GPIO_PinSource0);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;	//KEY2
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	  
	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource1);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
  	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//使能按键WK_UP所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
		
		NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//使能按键WK_UP所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
}
void process(void)
{
 left_speed  = (left_numb/20)*20.5;
 right_speed = (right_numb/20)*20.5;

 left_numb  = 0;
 right_numb = 0;
	LCD_ShowNum(60,80,left_speed,4,24);
	LCD_ShowNum(60,120,right_speed,4,24);
	//printf("left=%d cm\r\n",left_speed);
	//printf("rignt=%d cm\r\n",right_speed);
	
 /*
 OLED_ShowString(20,10,"SPEED-TEST");                             
 OLED_ShowString(0,30,"left:    cm/s");
 OLED_ShowNumber(40,30,(u16)left_speed,4,12);
 OLED_ShowString(0,50,"right:   cm/s");	
 OLED_ShowNumber(40,50,(u16)right_speed,4,12);
 OLED_Refresh_Gram();	                        //刷新
	*/

}
