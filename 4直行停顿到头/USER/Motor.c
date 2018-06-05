#include "Motor.h"
#include "delay.h"
u8 pwmval_left  = 0;                          //左电机调速变量 
u8 pwmval_right = 0;                          //右电机调速变量 

u8 pwmval_left_init  = 10;                    //左电机速度值，可以在0到20之间调节  
u8 pwmval_right_init = 10;                    //右电机速度值，可以在0到20之间调节   

u8 right_pwm = 1;	                          //左电机调速开关   
u8 left_pwm  = 1;			                  //右电机调速开关
void Motor_init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PB,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;				 //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA
 GPIO_ResetBits(GPIOA,GPIO_Pin_3);						 //PA3 输出低
 GPIO_ResetBits(GPIOA,GPIO_Pin_4);						 //PA4 输出低
 GPIO_ResetBits(GPIOA,GPIO_Pin_5);						 //PA5 输出低
 GPIO_ResetBits(GPIOA,GPIO_Pin_6);						 //PA6 输出低
}
void forward(void)                      //小车前进控制函数
{
 IN1 = 1;
 IN2 = 0;
 
 IN3 = 1;
 IN4 = 0;  
}
void back(void)			                    //小车后退控制函数
{ 
 IN1 = 0;
 IN2 = 1;
 
 IN3 = 0;
 IN4 = 1; 
}

void left_turn(void)	                  //向左转
{  
 IN1 = 0;
 IN2 = 0; 

 IN3 = 1;
 IN4 = 0;
}

void right_turn(void)		                //向右转  
{ 
 IN1 = 1;
 IN2 = 0;
 
 IN3 = 0;
 IN4 = 0;    
}

void stop(void)			                    //停车
{ 
 IN1 = 0;
 IN2 = 0;   
 
 IN3 = 0;
 IN4 = 0; 
}

void circle_left(void)		              //原地向左转圈
{ 
 IN1 = 0;
 IN2 = 1; 
 
 IN3 = 1;
 IN4 = 0; 
}

void circle_right(void)		 	            //原地向右转圈 
{
 IN1 = 1;
 IN2 = 0;    
 
 IN3 = 0;
 IN4 = 1;    
}
void left_moto(void)                    //左电机调速函数
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

void right_moto(void)                   //右电机调速函数
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
void SxForward(void)//蛇形
{
	delay_ms(2000);

//	TIM3->CR1&=0xFE;                            //屏蔽定时器3
	TIM_Cmd(TIM3,DISABLE); 
  pwmval_right = 0;
  pwmval_left  = 0;
  pwmval_left_init  = 0;                  
  pwmval_right_init = 10;  
  //TIM3->CR1|=0x01;                            //使能定时器3
	TIM_Cmd(TIM3,ENABLE);
	delay_ms(2000);
  //控制智能小车前进
//  TIM3->CR1&=0xFE;                            //屏蔽定时器3
	TIM_Cmd(TIM3,DISABLE); 
	pwmval_right = 0;
  pwmval_left  = 0;
  pwmval_left_init  = 10;                  
  pwmval_right_init = 10;  
 //	TIM3->CR1|=0x01;                            //使能定时器3
	TIM_Cmd(TIM3,ENABLE);
	delay_ms(2000);

//	TIM3->CR1&=0xFE;                            //屏蔽定时器3
TIM_Cmd(TIM3,DISABLE); 
  pwmval_right = 0;
  pwmval_left  = 0;
  pwmval_left_init  = 10;                  
  pwmval_right_init = 0;
//  TIM3->CR1|=0x01;                            //使能定时器3	
TIM_Cmd(TIM3,ENABLE);
  delay_ms(2000);
  //控制智能小车前进
//	TIM3->CR1&=0xFE;                            //屏蔽定时器3
TIM_Cmd(TIM3,DISABLE); 
  pwmval_right = 0;
  pwmval_left  = 0;
  pwmval_left_init  = 10;                  
  pwmval_right_init = 10; 
//  TIM3->CR1|=0x01;                            //使能定时器3	
TIM_Cmd(TIM3,ENABLE);
}
void SjxForword(void)//四角形
{
 forward();                                   //智能小车前进
 delay_ms(1500);                       
 stop();                                      //智能小车停止
 delay_ms(500);
                    	 
 
 circle_left();                               //智能小车左转
 delay_ms(400);                         
 stop();                                      //智能小车停止
 delay_ms(500);


 forward();                                   //智能小车前进
 delay_ms(1500);                        
 stop();                                      //智能小车停止
 delay_ms(500);

	 
 circle_left();                               //智能小车左转
 delay_ms(400);                          
 stop();                                      //智能小车停止
 delay_ms(500);


 forward();                                   //智能小车前进
 delay_ms(1500);                        
 stop();                                      //智能小车停止
 delay_ms(500);

 
 circle_left();                               //智能小车左转
 delay_ms(400);                        
 stop();                                      //智能小车停止
 delay_ms(500);


 forward();                                   //智能小车前进
 delay_ms(1500);                        
 stop();                                      //智能小车停止
 delay_ms(500);

	 
 circle_left();                               //智能小车左转
 delay_ms(400);                        
 stop();                                      //智能小车停止
 delay_ms(500);
 }

