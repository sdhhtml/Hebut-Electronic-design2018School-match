/**
  ******************************************************************************
  * @file    MicroStepDriver.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   MSD控制驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火STM32 F103-霸道 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "MicroStepDriver.h" 
#include "MicroStepDriver1.h" 
#include <stdio.h>
#include <math.h>

//系统加减速参数
speedRampData1 srd1;
//记录步进电机的位置
signed int stepPosition1 = 0;
//系统电机、串口状态
struct GLOBAL_FLAGS status1 = {FALSE, FALSE,TRUE};

/**

  * @brief  定时器中断优先级配置

  * @param  无

  * @retval 无

  */
static void TIM_NVIC_Config(void)

{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
	// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn; 	
	// 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	// 设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}
/**

  * @brief  初始化电机驱动用到的引脚

  * @param  无

  * @retval 无

  */
static void MSD_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //电机脉冲输出 GPIO 初始化
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //电机方向输出 GPIO 初始化
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB,GPIO_Pin_7);
    
    //电机使能输出 GPIO 初始化
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOC,GPIO_Pin_2);
}


///*
// * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
// * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
// * 另外三个成员是通用定时器和高级定时器才有.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            都有
// *  TIM_CounterMode		   TIMx,x[6,7]没有，其他都有
// *  TIM_Period               都有
// *  TIM_ClockDivision        TIMx,x[6,7]没有，其他都有
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]才有
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */

/* ----------------   PWM信号 周期和占空比的计算--------------- */
// ARR ：自动重装载寄存器的值
// CLK_cnt：计数器的时钟，等于 Fck_int / (psc+1) = 72M/(psc+1)
// PWM 信号的周期 T = (ARR+1) * (1/CLK_cnt) = (ARR+1)*(PSC+1) / 72M
// 占空比P=CCR/(ARR+1)

static void TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure1;
	TIM_OCInitTypeDef  TIM_OCInitStructure1;	
  // 开启定时器时钟,即内部时钟CK_INT=72M
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);

    /*--------------------时基结构体初始化-------------------------*/
	
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure1);
    // 自动重装载寄存器的值，累计TIM_Period+1个周期后产生一个更新或者中断
	TIM_TimeBaseStructure1.TIM_Period=9;	
	// 驱动CNT计数器的时钟 = Fck_int/(psc+1)
	TIM_TimeBaseStructure1.TIM_Prescaler=71;	
	// 时钟分频因子 ，配置死区时间时需要用到
	TIM_TimeBaseStructure1.TIM_ClockDivision=TIM_CKD_DIV1;		
	// 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure1.TIM_CounterMode=TIM_CounterMode_Up;		
	// 重复计数器的值，最大值为255
	//TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// 初始化定时器
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure1);

	/*--------------------输出比较结构体初始化-------------------*/		

	// 配置为PWM模式2
	TIM_OCInitStructure1.TIM_OCMode = TIM_OCMode_PWM2;
	// 输出使能
	TIM_OCInitStructure1.TIM_OutputState = TIM_OutputState_Enable;
	// 互补输出禁能
	TIM_OCInitStructure1.TIM_OutputNState = TIM_OutputNState_Disable; 
	// 设置占空比大小
	TIM_OCInitStructure1.TIM_Pulse =9/2;
	// 输出通道电平极性配置
	TIM_OCInitStructure1.TIM_OCPolarity = TIM_OCPolarity_Low;
	// 输出通道空闲电平极性配置
	TIM_OCInitStructure1.TIM_OCIdleState = TIM_OCIdleState_Reset;
    
	TIM_OC3Init(TIM5, &TIM_OCInitStructure1);
    //使能TIM1_CH1预装载寄存器
	TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);
    //使能TIM1预装载寄存器
    TIM_ARRPreloadConfig(TIM5, ENABLE); 
    
    //设置中断源，只有溢出时才中断
    TIM_UpdateRequestConfig(TIM5,TIM_UpdateSource_Regular);
	// 清除中断标志位
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    // 使能中断
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	// 使能计数器
	TIM_Cmd(TIM5, DISABLE);
}
/**

  * @brief  初始化电机相关的外设

  * @param  无

  * @retval 无

  */
void MSD_Init1(void)
{
    MSD_GPIO_Config();
    
    TIM_NVIC_Config();

    TIM_Mode_Config();    
}
/**

  * @brief  驱动器紧急停止

  * @param  NewState：使能或者禁止

  * @retval 无

  */
void MSD_ENA1(FunctionalState NewState)
{
    if(NewState)
    {
      //ENA失能，禁止驱动器输出
      GPIO_SetBits(GPIOC,GPIO_Pin_2);
      //紧急停止标志位为真
      status1.out_ena = FALSE; 
      printf("\n\r驱动器禁止输出（脱机状态）此时电机为无保持力矩状态，可以手动旋转电机");        
    }
    else
    {
      //ENA使能
      GPIO_ResetBits(GPIOC,GPIO_Pin_2);
      //紧急停止标志位为假
      status1.out_ena = TRUE; 
      printf("\n\r驱动器恢复运行，此时电机为保持力矩状态，此时串口指令可以正常控制电机");         
    }
    
}
/*! \brief 以给定的步数移动步进电机
 *  通过计算加速到最大速度，以给定的步数开始减速
 *  如果加速度和减速度很小，步进电机会移动很慢，还没达到最大速度就要开始减速
 *  \param step   移动的步数 (正数为顺时针，负数为逆时针).
 *  \param accel  加速度,如果取值为100，实际值为100*0.01*rad/sec^2=1rad/sec^2
 *  \param decel  减速度,如果取值为100，实际值为100*0.01*rad/sec^2=1rad/sec^2
 *  \param speed  最大速度,如果取值为100，实际值为100*0.01*rad/sec=1rad/sec
 */
void MSD_Move1(signed int step, unsigned int accel, unsigned int decel, unsigned int speed)
{
    //达到最大速度时的步数.
    unsigned int max_s_lim;
    //必须开始减速的步数(如果还没加速到达最大速度时)。
    unsigned int accel_lim;

    // 根据步数的正负来判断方向
    if(step < 0)//逆时针
    {
        srd1.dir = CCW;
        step = -step;
    }
    else//顺时针
    {
        srd1.dir = CW;
    }
    // 输出电机方向
    DIR1(srd1.dir);
    // 配置电机为输出状态
    //status.out_ena = TRUE;
    
    // 如果只移动一步
    if(step == 1)
    {
        // 只移动一步
        srd1.accel_count = -1;
        // 减速状态
        srd1.run_state = DECEL;
        // 短延时
        srd1.step_delay = 1000;
        // 配置电机为运行状态
        status1.running = TRUE;
        //设置定时器重装值	
        TIM_SetAutoreload(TIM5,Pulse_width);
        //设置占空比为50%	
        TIM_SetCompare3(TIM5,Pulse_width>>1);
        //使能定时器	      
        TIM_Cmd(TIM5, ENABLE); 
     }
    // 步数不为零才移动
    else if(step != 0)
    {
    // 我们的驱动器用户手册有详细的计算及推导过程

    // 设置最大速度极限, 计算得到min_delay用于定时器的计数器的值。
    // min_delay = (alpha / tt)/ w
    srd1.min_delay = A_T_x100 / speed;

    // 通过计算第一个(c0) 的步进延时来设定加速度，其中accel单位为0.01rad/sec^2
    // step_delay = 1/tt * sqrt(2*alpha/accel)
    // step_delay = ( tfreq*0.676/100 )*100 * sqrt( (2*alpha*10000000000) / (accel*100) )/10000
    srd1.step_delay = (T1_FREQ_148 * sqrt(A_SQ / accel))/100;

    // 计算多少步之后达到最大速度的限制
    // max_s_lim = speed^2 / (2*alpha*accel)
    max_s_lim = (long)speed*speed/(long)(((long)A_x20000*accel)/100);
    // 如果达到最大速度小于0.5步，我们将四舍五入为0
    // 但实际我们必须移动至少一步才能达到想要的速度
    if(max_s_lim == 0)
    {
        max_s_lim = 1;
    }

    // 计算多少步之后我们必须开始减速
    // n1 = (n1+n2)decel / (accel + decel)
    accel_lim = ((long)step*decel) / (accel+decel);
    // 我们必须加速至少1步才能才能开始减速.
    if(accel_lim == 0)
    {
        accel_lim = 1;
    }
    // 使用限制条件我们可以计算出第一次开始减速的位置
    //srd.decel_val为负数
    if(accel_lim <= max_s_lim)
    {
        srd1.decel_val = accel_lim - step;
    }
    else
    {
        srd1.decel_val = -(long)(max_s_lim*accel/decel);
    }
    // 当只剩下一步我们必须减速
    if(srd1.decel_val == 0)
    {
        srd1.decel_val = -1;
    }

    // 计算开始减速时的步数
    srd1.decel_start = step + srd1.decel_val;

    // 如果最大速度很慢，我们就不需要进行加速运动
    if(srd1.step_delay <= srd1.min_delay)
    {
        srd1.step_delay = srd1.min_delay;
        srd1.run_state = RUN;
    }
    else
   {
        srd1.run_state = ACCEL;
    }

    // 复位加速度计数值
    srd1.accel_count = 0;
    status1.running = TRUE;
    //设置定时器重装值	
    TIM_SetAutoreload(TIM5,Pulse_width);
    //设置占空比为50%	
    TIM_SetCompare3(TIM5,Pulse_width>>1);
    //使能定时器	      
    TIM_Cmd(TIM5, ENABLE); 
    }
}

/**

  * @brief  根据运动方向判断步进电机的运行位置

  * @param  inc 运动方向

  * @retval 无

  */
void MSD_StepCounter1(signed char inc)
{
  //根据方向判断电机位置
  if(inc == CCW)
  {
    stepPosition1--;
  }
  else
  {
    stepPosition1++;
  }
}
/**

  * @brief  产生脉冲定时器的中断响应程序，每走一步都会计算运动状态

  * @param  无

  * @retval 无

  */
void TIM5_IRQHandler(void)
{
  // 保存下一个延时周期
  unsigned int new_step_delay;
  // 加速过程中最后一次延时.
  static int last_accel_delay;
  // 移动步数计数器
  static unsigned int step_count = 0;
  // 记录new_step_delay中的余数，提高下一步计算的精度
  static signed int rest = 0;

if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
{
    /* Clear MSD_PULSE_TIM Capture Compare1 interrupt pending bit*/
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);

    MSD_PULSE_TIM1->CCR3=srd1.step_delay >> 1;//周期的一半
    MSD_PULSE_TIM1->ARR=srd1.step_delay;
    //如果禁止输出，电机则停止运动
    if(status1.out_ena != TRUE)
    {
        srd1.run_state = STOP;
    }
  switch(srd1.run_state) {
    case STOP:
      step_count = 0;
      rest = 0;
      TIM5->CCER &= ~(1<<12); //禁止输出
      TIM_Cmd(TIM5, DISABLE);
      status1.running = FALSE;
      break;

    case ACCEL:
      TIM5->CCER |= 1<<12; //使能输出
      MSD_StepCounter1(srd1.dir);
      step_count++;
      srd1.accel_count++;
      new_step_delay = srd1.step_delay - (((2 * (long)srd1.step_delay) 
                       + rest)/(4 * srd1.accel_count + 1));
      rest = ((2 * (long)srd1.step_delay)+rest)%(4 * srd1.accel_count + 1);
      //检查是够应该开始减速
      if(step_count >= srd1.decel_start) {
        srd1.accel_count = srd1.decel_val;
        srd1.run_state = DECEL;
      }
      //检查是否到达期望的最大速度
      else if(new_step_delay <= srd1.min_delay) {
        last_accel_delay = new_step_delay;
        new_step_delay = srd1.min_delay;
        rest = 0;
        srd1.run_state = RUN;
      }
      break;

    case RUN:
      TIM5->CCER |= 1<<12; //使能输出
      MSD_StepCounter1(srd1.dir);
      step_count++;
      new_step_delay = srd1.min_delay;
      //检查是否需要开始减速
      if(step_count >= srd1.decel_start) {
        srd1.accel_count = srd1.decel_val;
        //以最后一次加速的延时作为开始减速的延时
        new_step_delay = last_accel_delay;
        srd1.run_state = DECEL;
      }
      break;

    case DECEL:
      TIM5->CCER |= 1<<12; //使能输出
      MSD_StepCounter1(srd1.dir);
      step_count++;
      srd1.accel_count++;
      new_step_delay = srd1.step_delay - (((2 * (long)srd1.step_delay) 
                       + rest)/(4 * srd1.accel_count + 1));
      rest = ((2 * (long)srd1.step_delay)+rest)%(4 * srd1.accel_count + 1);
      //检查是否为最后一步
      if(srd1.accel_count >= 0){
        srd1.run_state = STOP;
      }
      break;
  }
  srd1.step_delay = new_step_delay;
  }
	if(stepPosition1%100==0)printf("步进电机1当前位置:%d\r\n",stepPosition1);
	if(stepPosition1>6359)stepPosition1=0;
	if(stepPosition1<(-6359))stepPosition1=0;
	
}

/*********************************************END OF FILE**********************/
