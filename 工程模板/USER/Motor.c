#include "Motor.h"
#include "delay.h"
u8 pwmval_left  = 0;                          //�������ٱ��� 
u8 pwmval_right = 0;                          //�ҵ�����ٱ��� 

u8 pwmval_left_init  = 10;                    //�����ٶ�ֵ��������0��20֮�����  
u8 pwmval_right_init = 10;                    //�ҵ���ٶ�ֵ��������0��20֮�����   

u8 right_pwm = 1;	                          //�������ٿ���   
u8 left_pwm  = 1;			                  //�ҵ�����ٿ���
void Motor_init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;				 //LED0-->PB.5 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA
 GPIO_ResetBits(GPIOA,GPIO_Pin_3);						 //PA3 �����
 GPIO_ResetBits(GPIOA,GPIO_Pin_4);						 //PA4 �����
 GPIO_ResetBits(GPIOA,GPIO_Pin_5);						 //PA5 �����
 GPIO_ResetBits(GPIOA,GPIO_Pin_6);						 //PA6 �����
}
void forward(void)                      //С��ǰ�����ƺ���
{
 IN1 = 1;
 IN2 = 0;
 
 IN3 = 1;
 IN4 = 0;  
}
void back(void)			                    //С�����˿��ƺ���
{ 
 IN1 = 0;
 IN2 = 1;
 
 IN3 = 0;
 IN4 = 1; 
}

void left_turn(void)	                  //����ת
{  
 IN1 = 0;
 IN2 = 0; 

 IN3 = 1;
 IN4 = 0;
}

void right_turn(void)		                //����ת  
{ 
 IN1 = 1;
 IN2 = 0;
 
 IN3 = 0;
 IN4 = 0;    
}

void stop(void)			                    //ͣ��
{ 
 IN1 = 0;
 IN2 = 0;   
 
 IN3 = 0;
 IN4 = 0; 
}

void circle_left(void)		              //ԭ������תȦ
{ 
 IN1 = 0;
 IN2 = 1; 
 
 IN3 = 1;
 IN4 = 0; 
}

void circle_right(void)		 	            //ԭ������תȦ 
{
 IN1 = 1;
 IN2 = 0;    
 
 IN3 = 0;
 IN4 = 1;    
}
void left_moto(void)                    //�������ٺ���
{  
 if(left_pwm)
 {
  if(pwmval_left <= pwmval_left_init)
  {
   IN1 = 1; 
  }
  else 
  {
   IN1 = 0;
  }
  if(pwmval_left >= 20)
  {
   pwmval_left = 0;
  }
 }
 else    
 {
  IN1 = 0;                      
 }
}

void right_moto(void)                   //�ҵ�����ٺ���
{ 
 if(right_pwm)
 { 
  if(pwmval_right <= pwmval_right_init)		  
  {
   IN3 = 1; 							   
  }
  else if(pwmval_right > pwmval_right_init)
  {
   IN3 = 0;
  }
  if(pwmval_right >= 20)
  {
   pwmval_right = 0;
  }
 }
 else    
 {
  IN3 = 0;	                                  	    
 }
}
void SxForward(void)//����
{
	delay_ms(2000);

//	TIM3->CR1&=0xFE;                            //���ζ�ʱ��3
	TIM_Cmd(TIM3,DISABLE); 
  pwmval_right = 0;
  pwmval_left  = 0;
  pwmval_left_init  = 0;                  
  pwmval_right_init = 10;  
  //TIM3->CR1|=0x01;                            //ʹ�ܶ�ʱ��3
	TIM_Cmd(TIM3,ENABLE);
	delay_ms(2000);
  //��������С��ǰ��
//  TIM3->CR1&=0xFE;                            //���ζ�ʱ��3
	TIM_Cmd(TIM3,DISABLE); 
	pwmval_right = 0;
  pwmval_left  = 0;
  pwmval_left_init  = 10;                  
  pwmval_right_init = 10;  
 //	TIM3->CR1|=0x01;                            //ʹ�ܶ�ʱ��3
	TIM_Cmd(TIM3,ENABLE);
	delay_ms(2000);

//	TIM3->CR1&=0xFE;                            //���ζ�ʱ��3
TIM_Cmd(TIM3,DISABLE); 
  pwmval_right = 0;
  pwmval_left  = 0;
  pwmval_left_init  = 10;                  
  pwmval_right_init = 0;
//  TIM3->CR1|=0x01;                            //ʹ�ܶ�ʱ��3	
TIM_Cmd(TIM3,ENABLE);
  delay_ms(2000);
  //��������С��ǰ��
//	TIM3->CR1&=0xFE;                            //���ζ�ʱ��3
TIM_Cmd(TIM3,DISABLE); 
  pwmval_right = 0;
  pwmval_left  = 0;
  pwmval_left_init  = 10;                  
  pwmval_right_init = 10; 
//  TIM3->CR1|=0x01;                            //ʹ�ܶ�ʱ��3	
TIM_Cmd(TIM3,ENABLE);
}
void SjxForword(void)//�Ľ���
{
 forward();                                   //����С��ǰ��
 delay_ms(1500);                       
 stop();                                      //����С��ֹͣ
 delay_ms(500);
                    	 
 
 circle_left();                               //����С����ת
 delay_ms(400);                         
 stop();                                      //����С��ֹͣ
 delay_ms(500);


 forward();                                   //����С��ǰ��
 delay_ms(1500);                        
 stop();                                      //����С��ֹͣ
 delay_ms(500);

	 
 circle_left();                               //����С����ת
 delay_ms(400);                          
 stop();                                      //����С��ֹͣ
 delay_ms(500);


 forward();                                   //����С��ǰ��
 delay_ms(1500);                        
 stop();                                      //����С��ֹͣ
 delay_ms(500);

 
 circle_left();                               //����С����ת
 delay_ms(400);                        
 stop();                                      //����С��ֹͣ
 delay_ms(500);


 forward();                                   //����С��ǰ��
 delay_ms(1500);                        
 stop();                                      //����С��ֹͣ
 delay_ms(500);

	 
 circle_left();                               //����С����ת
 delay_ms(400);                        
 stop();                                      //����С��ֹͣ
 delay_ms(500);
 }

