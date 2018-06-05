#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "string.h"
#include "ov7725.h"
#include "ov7670.h"
#include "tpad.h"
#include "timer.h"
#include "exti.h"
#include "usmart.h"
#include "beep.h"
#include "Encoder.h"
#include "Motor.h"
extern u16 timer;    
extern u8 flag;

extern int right_numb;
/************************************************
 ALIENTEKս��STM32������ʵ��35
 ����ͷOV7725��OV7670 ʵ��
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/


#define  OV7725 1
#define  OV7670 2

//����OV7725��������װ��ʽԭ��,OV7725_WINDOW_WIDTH�൱��LCD�ĸ߶ȣ�OV7725_WINDOW_HEIGHT�൱��LCD�Ŀ��
//ע�⣺�˺궨��ֻ��OV7725��Ч
#define  OV7725_WINDOW_WIDTH		320 // <=320
#define  OV7725_WINDOW_HEIGHT		240 // <=240
//u16 img_buf[240][320];
u16 img_buf_r[47][33];
u16 img_buf_g[47][33];
//u16 img_buf_b[80][100];
u16 img_buf[47][33];
u32 And_r;
u32 R_s;
u32 And_g;
u32 G_s;
u32 And_b;
u32 B_s;
u8 flag;
u32 Average_r;
u32 Average_g;
u32 Average_b;
u8 a,b;
const u8*LMODE_TBL[6]={"Auto","Sunny","Cloudy","Office","Home","Night"};//6�ֹ���ģʽ	    
const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7����Ч 
extern u8 ov_sta;	//��exit.c�� �涨��
extern u8 ov_frame;	//��timer.c���涨�� 

//����LCD��ʾ(OV7725)
void OV7725_camera_refresh(void)
{
	u32 i,j;

 	u16 color;	 
	if(ov_sta)//��֡�жϸ���
	{
		LCD_Scan_Dir(U2D_L2R);//���ϵ���,������
		LCD_Set_Window((lcddev.width-OV7725_WINDOW_WIDTH)/2,(lcddev.height-OV7725_WINDOW_HEIGHT)/2,OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT);//����ʾ�������õ���Ļ����
		if(lcddev.id==0X1963)
			LCD_Set_Window((lcddev.width-OV7725_WINDOW_WIDTH)/2,(lcddev.height-OV7725_WINDOW_HEIGHT)/2,OV7725_WINDOW_HEIGHT,OV7725_WINDOW_WIDTH);//����ʾ�������õ���Ļ����
		LCD_WriteRAM_Prepare();     //��ʼд��GRAM	
		OV7725_RRST=0;				//��ʼ��λ��ָ�� 
		OV7725_RCK_L;
		OV7725_RCK_H;
		OV7725_RCK_L;
		OV7725_RRST=1;				//��λ��ָ����� 
		OV7725_RCK_H; 
		for(i=0;i<OV7725_WINDOW_HEIGHT;i++)//240
		{
			for(j=0;j<OV7725_WINDOW_WIDTH;j++)//320
			{
				OV7725_RCK_L;
				color=GPIOC->IDR&0XFF;	//?����y?Y
				OV7725_RCK_H; 
				color<<=8;  
				OV7725_RCK_L;
				color|=GPIOC->IDR&0XFF;	//?����y?Y
				OV7725_RCK_H; 
				if((i>186)&&(j>153)&&(i<234)&&(j<187)){
				img_buf[i-187][j-154]=color;
				}
					
				LCD->LCD_RAM=color;
		

						
			}}
		for(i=0;i<47;i++)
		{
			for(j=0;j<33;j++)
			{
					
				img_buf_r[i][j]=(img_buf[i][j]&0xF800)>>11;
		   	img_buf_g[i][j]=(img_buf[i][j]&0x07e0)>>5;
				img_buf_g[i][j]= img_buf_g[i][j]/2;
	//			img_buf_b[i][j]=(img_buf[i][j]&0x001f);
			
				And_r+=img_buf_r[i][j];
				R_s++;
				And_g+=img_buf_g[i][j];
				G_s++;				
	//			And_b+=img_buf_b[i][j];
	//			B_s++;
		}}
		Average_r=And_r/R_s;
		Average_g=And_g/G_s;
	//	Average_b=And_b/B_s;
		And_r=0;R_s=0;		
		And_g=0;G_s=0;
//		And_b=0;B_s=0;
		
		LCD_ShowNum(100,160,Average_r,4,24);
		LCD_ShowNum(100,200,Average_g,4,24);
//		LCD_ShowNum(60,240,Average_b,4,24);

			if((Average_r>30)&&(Average_r>Average_b))
			{
					flag = 1; //��ɫ
			}
			else if((Average_g>20)&&(Average_r>10)&&(Average_g>Average_r))
			{flag=2;}
    else

{
		flag = 0; 
}	
 
		
		
 		ov_sta=0;					//����֡�жϱ��
		ov_frame++; 
		LCD_Scan_Dir(DFT_SCAN_DIR);	//�ָ�Ĭ��ɨ�跽�� 
	} 
}



 int main(void)
{	 
	u8 sensor=1;
//	u8 key;
 	u8 i=0;	    
	u8 msgbuf[15];//��Ϣ������
	u8 tm=0;
	u8 lightmode=0,effect=0;
	s8 saturation=0,brightness=0,contrast=0;
	 
	delay_init();	    	 	//��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 		//���ڳ�ʼ��Ϊ 115200
 	usmart_dev.init(72);		//��ʼ��USMART		
 	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();					//��ʼ������
	LCD_Init();			   		//��ʼ��LCD  
	TPAD_Init(6);				//����������ʼ�� 
 	POINT_COLOR=RED;			//��������Ϊ��ɫ 
	BEEP_Init();
	Motor_init();
	Encoder_exti();
	LCD_ShowString(30,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(30,70,200,16,16,"OV7725_OV7670 TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/11/1"); 
	LCD_ShowString(30,130,200,16,16,"KEY0:Light Mode");
	LCD_ShowString(30,150,200,16,16,"KEY1:Saturation");
	LCD_ShowString(30,170,200,16,16,"KEY2:Brightness");
	LCD_ShowString(30,190,200,16,16,"KEY_UP:Contrast");
	LCD_ShowString(30,210,200,16,16,"TPAD:Effects");	 
  LCD_ShowString(30,230,200,16,16,"OV7725_OV7670 Init...");	
	
	OV7725_Window_Set(OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT,1);
//	while(1)//��ʼ��OV7725_OV7670
//	{
		if(OV7725_Init()==0)
		{
			sensor=OV7725;}
//			LCD_ShowString(30,230,200,16,16,"OV7725 Init OK       ");
//#if 0	
//			while(1)
//			{
//				key=KEY_Scan(0);
//				if(key==KEY0_PRES)
//				{
//					OV7725_Window_Set(OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT,0);//QVGAģʽ���
//					break;
//				}
//				else if(key==KEY1_PRES)
//				{
//					OV7725_Window_Set(OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT,1);//VGAģʽ���
//					break;
//				}
//				i++;
//				if(i==100)LCD_ShowString(30,250,210,16,16,"KEY0:QVGA  KEY1:VGA"); //��˸��ʾ��ʾ��Ϣ
//				if(i==200)
//				{	
//					LCD_Fill(30,250,210,250+16,WHITE);
//					i=0; 
//				}
//				delay_ms(5);
//			}	
//#endif			
OV7725_Window_Set(OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT,1);
			OV7725_Window_Set(OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT,1);//VGAģʽ���
			OV7725_Light_Mode(lightmode);
			OV7725_Color_Saturation(saturation);
			OV7725_Brightness(brightness);
			OV7725_Contrast(contrast);
			OV7725_Special_Effects(effect);
			OV7725_CS=0;
//			break;
		


	TIM6_Int_Init(10000,7199);	//10Khz����Ƶ��,1�����ж�									  
	EXTI8_Init();				//ʹ�ܶ�ʱ������				
	LCD_Clear(BLACK);

		
//		if(sensor==OV7725)
			OV7725_camera_refresh();//������ʾ

 

 forward();
 //while((right_numb<70));
 delay_nms(630);
 stop();
// delay_nms(1000);
// stop();
 BEEP=1;
 delay_nms(500);
 BEEP=0;
 delay_nms(100);
while(flag!=2){OV7725_camera_refresh();}
	
//	while((right_numb<220)){forward();}
forward();
delay_nms(1270);
	 stop();
  BEEP=1;
 delay_nms(500);
 BEEP=0;
//delau_nms(5000);
	while(1);
}









