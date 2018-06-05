#ifndef __BSP_ADVANCETIME2_H
#define __BSP_ADVANCETIME2_H


#include "stm32f10x.h"


#define CW  0
#define CCW 1

#define TRUE 1
#define FALSE 0

#define Pulse_width 20

//系统状态


extern struct GLOBAL_FLAGS status2;
extern int stepPosition2;

#define T1_FREQ 1000000     //定时器频率
#define FSPR    200         //步进电机单圈步数
#define SPR     (FSPR*100)  //100细分的步数
// 数学常数。 用于MSD_Move函数的简化计算
#define ALPHA (2*3.14159/SPR)                    // 2*pi/spr
#define A_T_x100 ((long)(ALPHA*T1_FREQ*100))     // (ALPHA / T1_FREQ)*100
#define T1_FREQ_148 ((int)((T1_FREQ*0.676)/100)) // divided by 100 and scaled by 0.676
#define A_SQ (long)(ALPHA*2*10000000000)         // 
#define A_x20000 (int)(ALPHA*20000)              // ALPHA*20000
    
//速度曲线状态
#define STOP  0
#define ACCEL 1
#define DECEL 2
#define RUN   3





// 当使用不同的定时器的时候，对应的GPIO是不一样的，这点要注意
// 这里我们使用定时器TIM2



#define DIR2(a)	if (a == CW)	\
					GPIO_ResetBits(GPIOB,GPIO_Pin_6);\
					else		\
					GPIO_SetBits(GPIOB,GPIO_Pin_6)
                    
/**************************函数声明********************************/

void MSD_Init2(void);
void MSD_ENA2(FunctionalState NewState);
void MSD_Move2(signed int step, unsigned int accel, unsigned int decel, unsigned int speed);

#endif	/* __BSP_ADVANCETIME_H */


