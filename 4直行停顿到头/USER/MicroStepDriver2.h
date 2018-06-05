#ifndef __BSP_ADVANCETIME2_H
#define __BSP_ADVANCETIME2_H


#include "stm32f10x.h"


#define CW  0
#define CCW 1

#define TRUE 1
#define FALSE 0

#define Pulse_width 20

//ϵͳ״̬


extern struct GLOBAL_FLAGS status2;
extern int stepPosition2;

#define T1_FREQ 1000000     //��ʱ��Ƶ��
#define FSPR    200         //���������Ȧ����
#define SPR     (FSPR*100)  //100ϸ�ֵĲ���
// ��ѧ������ ����MSD_Move�����ļ򻯼���
#define ALPHA (2*3.14159/SPR)                    // 2*pi/spr
#define A_T_x100 ((long)(ALPHA*T1_FREQ*100))     // (ALPHA / T1_FREQ)*100
#define T1_FREQ_148 ((int)((T1_FREQ*0.676)/100)) // divided by 100 and scaled by 0.676
#define A_SQ (long)(ALPHA*2*10000000000)         // 
#define A_x20000 (int)(ALPHA*20000)              // ALPHA*20000
    
//�ٶ�����״̬
#define STOP  0
#define ACCEL 1
#define DECEL 2
#define RUN   3





// ��ʹ�ò�ͬ�Ķ�ʱ����ʱ�򣬶�Ӧ��GPIO�ǲ�һ���ģ����Ҫע��
// ��������ʹ�ö�ʱ��TIM2



#define DIR2(a)	if (a == CW)	\
					GPIO_ResetBits(GPIOB,GPIO_Pin_6);\
					else		\
					GPIO_SetBits(GPIOB,GPIO_Pin_6)
                    
/**************************��������********************************/

void MSD_Init2(void);
void MSD_ENA2(FunctionalState NewState);
void MSD_Move2(signed int step, unsigned int accel, unsigned int decel, unsigned int speed);

#endif	/* __BSP_ADVANCETIME_H */


