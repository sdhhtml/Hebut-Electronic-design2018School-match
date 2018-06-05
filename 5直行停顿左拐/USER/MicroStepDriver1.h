 #ifndef __BSP_ADVANCETIME1_H
#define __BSP_ADVANCETIME1_H


#include "stm32f10x.h"


#define CW  0
#define CCW 1

#define TRUE 1
#define FALSE 0

#define Pulse_width 20

//系统状态


extern struct GLOBAL_FLAGS status1;
extern int stepPosition1;

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
#define            MSD_PULSE_TIM1                    TIM5
#define            MSD_PULSE_TIM_APBxClock_FUN1      RCC_APB1PeriphClockCmd
#define            MSD_PULSE_TIM_CLK1                RCC_APB1Periph_TIM5
// 定时器输出PWM通道，PA0是通道1
#define            MSD_PULSE_OCx_Init1               TIM_OC3Init
#define            MSD_PULSE_OCx_PreloadConfig1      TIM_OC3PreloadConfig
// 定时器中断
#define            MSD_PULSE_TIM_IRQ1                TIM5_IRQn
#define            MSD_PULSE_TIM_IRQHandler1         TIM5_IRQHandler

// PWM 信号的频率 F = TIM_CLK/{(ARR+1)*(PSC+1)}
#define            MSD_PULSE_TIM_PERIOD1             (10-1)
#define            MSD_PULSE_TIM_PSC1                (72-1)

// 步进电机脉冲输出通道
#define            MSD_PULSE_GPIO_CLK1               RCC_APB2Periph_GPIOA
#define            MSD_PULSE_PORT1                   GPIOA
#define            MSD_PULSE_PIN1                   GPIO_Pin_2

// 步进电机方向控制
#define            MSD_DIR_GPIO_CLK1                 RCC_APB2Periph_GPIOB
#define            MSD_DIR_PORT1                     GPIOB
#define            MSD_DIR_PIN1                      GPIO_Pin_7

// 步进电机输出使能引脚
#define            MSD_ENA_GPIO_CLK1                 RCC_APB2Periph_GPIOC
#define            MSD_ENA_PORT1                     GPIOC
#define            MSD_ENA_PIN1                      GPIO_Pin_2


#define DIR1(a)	if (a == CW)	\
					GPIO_ResetBits(MSD_DIR_PORT1,MSD_DIR_PIN1);\
					else		\
					GPIO_SetBits(MSD_DIR_PORT1,MSD_DIR_PIN1)
                    
/**************************函数声明********************************/

void MSD_Init1(void);
void MSD_ENA1(FunctionalState NewState);
void MSD_Move1(signed int step, unsigned int accel, unsigned int decel, unsigned int speed);

#endif	/* __BSP_ADVANCETIME_H */


