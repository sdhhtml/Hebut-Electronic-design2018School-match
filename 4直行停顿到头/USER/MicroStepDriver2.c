/**
  ******************************************************************************
  * @file    MicroStepDriver.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   MSD��������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����STM32 F103-�Ե� ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "MicroStepDriver.h" 
#include "MicroStepDriver2.h" 
#include <stdio.h>
#include <math.h>

//ϵͳ�Ӽ��ٲ���
speedRampData2 srd2;
//��¼���������λ��
signed int stepPosition2 = 0;
//ϵͳ���������״̬
struct GLOBAL_FLAGS status2 = {FALSE, FALSE,TRUE};

/**

  * @brief  ��ʱ���ж����ȼ�����

  * @param  ��

  * @retval ��

  */
static void TIM_NVIC_Config(void)

{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
	// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; 	
	// ������ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	// ���������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**

  * @brief  ��ʼ����������õ�������

  * @param  ��

  * @retval ��

  */
static void MSD_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //���������� GPIO ��ʼ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    //���������� GPIO ��ʼ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB,GPIO_Pin_6);
    
    //���ʹ����� GPIO ��ʼ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOC,GPIO_Pin_3);
}


///*
// * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
// * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
// * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            ����
// *  TIM_CounterMode		   TIMx,x[6,7]û�У���������
// *  TIM_Period               ����
// *  TIM_ClockDivision        TIMx,x[6,7]û�У���������
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]����
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */

/* ----------------   PWM�ź� ���ں�ռ�ձȵļ���--------------- */
// ARR ���Զ���װ�ؼĴ�����ֵ
// CLK_cnt����������ʱ�ӣ����� Fck_int / (psc+1) = 72M/(psc+1)
// PWM �źŵ����� T = (ARR+1) * (1/CLK_cnt) = (ARR+1)*(PSC+1) / 72M
// ռ�ձ�P=CCR/(ARR+1)

static void TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure2;
	TIM_OCInitTypeDef  TIM_OCInitStructure2;
  // ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

    /*--------------------ʱ���ṹ���ʼ��-------------------------*/
	
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure2);
    // �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1�����ں����һ�����»����ж�
	TIM_TimeBaseStructure2.TIM_Period=9;	
	// ����CNT��������ʱ�� = Fck_int/(psc+1)
	TIM_TimeBaseStructure2.TIM_Prescaler= 71;	
	// ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�
	TIM_TimeBaseStructure2.TIM_ClockDivision=TIM_CKD_DIV1;		
	// ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure2.TIM_CounterMode=TIM_CounterMode_Up;		
	// �ظ���������ֵ�����ֵΪ255
	//TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// ��ʼ����ʱ��
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure2);

	/*--------------------����ȽϽṹ���ʼ��-------------------*/		
	
	// ����ΪPWMģʽ2
	TIM_OCInitStructure2.TIM_OCMode = TIM_OCMode_PWM2;
	// ���ʹ��
	TIM_OCInitStructure2.TIM_OutputState = TIM_OutputState_Enable;
	// �����������
	TIM_OCInitStructure2.TIM_OutputNState = TIM_OutputNState_Disable; 
	// ����ռ�ձȴ�С
	TIM_OCInitStructure2.TIM_Pulse = MSD_PULSE_TIM_PERIOD/2;
	// ���ͨ����ƽ��������
	TIM_OCInitStructure2.TIM_OCPolarity = TIM_OCPolarity_Low;
	// ���ͨ�����е�ƽ��������
	TIM_OCInitStructure2.TIM_OCIdleState = TIM_OCIdleState_Reset;
    
	TIM_OC4Init(TIM4, &TIM_OCInitStructure2);
    //ʹ��TIM1_CH1Ԥװ�ؼĴ���
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
    //ʹ��TIM1Ԥװ�ؼĴ���
  TIM_ARRPreloadConfig(TIM4, ENABLE); 
    
    //�����ж�Դ��ֻ�����ʱ���ж�
  TIM_UpdateRequestConfig(TIM4,TIM_UpdateSource_Regular);
	// ����жϱ�־λ
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    // ʹ���ж�
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	// ʹ�ܼ�����
	TIM_Cmd(TIM4, DISABLE);
}
/**

  * @brief  ��ʼ�������ص�����

  * @param  ��

  * @retval ��

  */
void MSD_Init2(void)
{
    MSD_GPIO_Config();
    
    TIM_NVIC_Config();

    TIM_Mode_Config();    
}
/**

  * @brief  ����������ֹͣ

  * @param  NewState��ʹ�ܻ��߽�ֹ

  * @retval ��

  */
void MSD_ENA2(FunctionalState NewState)
{
    if(NewState)
    {
      //ENAʧ�ܣ���ֹ���������
      GPIO_SetBits(GPIOC,GPIO_Pin_3);
      //����ֹͣ��־λΪ��
      status2.out_ena = FALSE; 
      printf("\n\r��������ֹ������ѻ�״̬����ʱ���Ϊ�ޱ�������״̬�������ֶ���ת���");        
    }
    else
    {
      //ENAʹ��
      GPIO_ResetBits(GPIOC,GPIO_Pin_3);
      //����ֹͣ��־λΪ��
      status2.out_ena = TRUE; 
      printf("\n\r�������ָ����У���ʱ���Ϊ��������״̬����ʱ����ָ������������Ƶ��");         
    }
    
}
/*! \brief �Ը����Ĳ����ƶ��������
 *  ͨ��������ٵ�����ٶȣ��Ը����Ĳ�����ʼ����
 *  ������ٶȺͼ��ٶȺ�С������������ƶ���������û�ﵽ����ٶȾ�Ҫ��ʼ����
 *  \param step   �ƶ��Ĳ��� (����Ϊ˳ʱ�룬����Ϊ��ʱ��).
 *  \param accel  ���ٶ�,���ȡֵΪ100��ʵ��ֵΪ100*0.01*rad/sec^2=1rad/sec^2
 *  \param decel  ���ٶ�,���ȡֵΪ100��ʵ��ֵΪ100*0.01*rad/sec^2=1rad/sec^2
 *  \param speed  ����ٶ�,���ȡֵΪ100��ʵ��ֵΪ100*0.01*rad/sec=1rad/sec
 */
void MSD_Move2(signed int step, unsigned int accel, unsigned int decel, unsigned int speed)
{
    //�ﵽ����ٶ�ʱ�Ĳ���.
    unsigned int max_s_lim;
    //���뿪ʼ���ٵĲ���(�����û���ٵ�������ٶ�ʱ)��
    unsigned int accel_lim;

    // ���ݲ������������жϷ���
    if(step < 0)//��ʱ��
    {
        srd2.dir = CCW;
        step = -step;
    }
    else//˳ʱ��
    {
        srd2.dir = CW;
    }
    // ����������
    DIR2(srd2.dir);
    // ���õ��Ϊ���״̬
    //status.out_ena = TRUE;
    
    // ���ֻ�ƶ�һ��
    if(step == 1)
    {
        // ֻ�ƶ�һ��
        srd2.accel_count = -1;
        // ����״̬
        srd2.run_state = DECEL;
        // ����ʱ
        srd2.step_delay = 1000;
        // ���õ��Ϊ����״̬
        status2.running = TRUE;
        //���ö�ʱ����װֵ	
        TIM_SetAutoreload(TIM4,Pulse_width);
        //����ռ�ձ�Ϊ50%	
        TIM_SetCompare4(TIM4,Pulse_width>>1);
        //ʹ�ܶ�ʱ��	      
        TIM_Cmd(TIM4, ENABLE); 
     }
    // ������Ϊ����ƶ�
    else if(step != 0)
    {
    // ���ǵ��������û��ֲ�����ϸ�ļ��㼰�Ƶ�����

    // ��������ٶȼ���, ����õ�min_delay���ڶ�ʱ���ļ�������ֵ��
    // min_delay = (alpha / tt)/ w
    srd2.min_delay = A_T_x100 / speed;

    // ͨ�������һ��(c0) �Ĳ�����ʱ���趨���ٶȣ�����accel��λΪ0.01rad/sec^2
    // step_delay = 1/tt * sqrt(2*alpha/accel)
    // step_delay = ( tfreq*0.676/100 )*100 * sqrt( (2*alpha*10000000000) / (accel*100) )/10000
    srd2.step_delay = (T1_FREQ_148 * sqrt(A_SQ / accel))/100;

    // ������ٲ�֮��ﵽ����ٶȵ�����
    // max_s_lim = speed^2 / (2*alpha*accel)
    max_s_lim = (long)speed*speed/(long)(((long)A_x20000*accel)/100);
    // ����ﵽ����ٶ�С��0.5�������ǽ���������Ϊ0
    // ��ʵ�����Ǳ����ƶ�����һ�����ܴﵽ��Ҫ���ٶ�
    if(max_s_lim == 0)
    {
        max_s_lim = 1;
    }

    // ������ٲ�֮�����Ǳ��뿪ʼ����
    // n1 = (n1+n2)decel / (accel + decel)
    accel_lim = ((long)step*decel) / (accel+decel);
    // ���Ǳ����������1�����ܲ��ܿ�ʼ����.
    if(accel_lim == 0)
    {
        accel_lim = 1;
    }
    // ʹ�������������ǿ��Լ������һ�ο�ʼ���ٵ�λ��
    //srd.decel_valΪ����
    if(accel_lim <= max_s_lim)
    {
        srd2.decel_val = accel_lim - step;
    }
    else
    {
        srd2.decel_val = -(long)(max_s_lim*accel/decel);
    }
    // ��ֻʣ��һ�����Ǳ������
    if(srd2.decel_val == 0)
    {
        srd2.decel_val = -1;
    }

    // ���㿪ʼ����ʱ�Ĳ���
    srd2.decel_start = step + srd2.decel_val;

    // �������ٶȺ��������ǾͲ���Ҫ���м����˶�
    if(srd2.step_delay <= srd2.min_delay)
    {
        srd2.step_delay = srd2.min_delay;
        srd2.run_state = RUN;
    }
    else
   {
        srd2.run_state = ACCEL;
    }

    // ��λ���ٶȼ���ֵ
    srd2.accel_count = 0;
    status2.running = TRUE;
    //���ö�ʱ����װֵ	
    TIM_SetAutoreload(TIM4,Pulse_width);
    //����ռ�ձ�Ϊ50%	
    TIM_SetCompare4(TIM4,Pulse_width>>1);
    //ʹ�ܶ�ʱ��	      
    TIM_Cmd(TIM4, ENABLE); 
    }
}

/**

  * @brief  �����˶������жϲ������������λ��

  * @param  inc �˶�����

  * @retval ��

  */
void MSD_StepCounter2(signed char inc)
{
  //���ݷ����жϵ��λ��
  if(inc == CCW)
  {
    stepPosition2--;
  }
  else
  {
    stepPosition2++;
  }
}
/**

  * @brief  �������嶨ʱ�����ж���Ӧ����ÿ��һ����������˶�״̬

  * @param  ��

  * @retval ��

  */
void TIM4_IRQHandler(void)
{
  // ������һ����ʱ����
  unsigned int new_step_delay;
  // ���ٹ��������һ����ʱ.
  static int last_accel_delay;
  // �ƶ�����������
  static unsigned int step_count = 0;
  // ��¼new_step_delay�е������������һ������ľ���
  static signed int rest = 0;

if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
{
    /* Clear MSD_PULSE_TIM Capture Compare1 interrupt pending bit*/
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

    TIM4->CCR3=srd2.step_delay >> 1;//���ڵ�һ��
    TIM4->ARR=srd2.step_delay;
    //�����ֹ����������ֹͣ�˶�
    if(status2.out_ena != TRUE)
    {
        srd2.run_state = STOP;
    }
  switch(srd2.run_state) {
    case STOP:
      step_count = 0;
      rest = 0;
      TIM4->CCER &= ~(1<<12); //��ֹ���
      TIM_Cmd(TIM4, DISABLE);
      status2.running = FALSE;
      break;

    case ACCEL:
      TIM4->CCER |= 1<<12; //ʹ�����
      MSD_StepCounter2(srd2.dir);
      step_count++;
      srd2.accel_count++;
      new_step_delay = srd2.step_delay - (((2 * (long)srd2.step_delay) 
                       + rest)/(4 * srd2.accel_count + 1));
      rest = ((2 * (long)srd2.step_delay)+rest)%(4 * srd2.accel_count + 1);
      //����ǹ�Ӧ�ÿ�ʼ����
      if(step_count >= srd2.decel_start) {
        srd2.accel_count = srd2.decel_val;
        srd2.run_state = DECEL;
      }
      //����Ƿ񵽴�����������ٶ�
      else if(new_step_delay <= srd2.min_delay) {
        last_accel_delay = new_step_delay;
        new_step_delay = srd2.min_delay;
        rest = 0;
        srd2.run_state = RUN;
      }
      break;

    case RUN:
      TIM4->CCER |= 1<<12; //ʹ�����
      MSD_StepCounter2(srd2.dir);
      step_count++;
      new_step_delay = srd2.min_delay;
      //����Ƿ���Ҫ��ʼ����
      if(step_count >= srd2.decel_start) {
        srd2.accel_count = srd2.decel_val;
        //�����һ�μ��ٵ���ʱ��Ϊ��ʼ���ٵ���ʱ
        new_step_delay = last_accel_delay;
        srd2.run_state = DECEL;
      }
      break;

    case DECEL:
      TIM4->CCER |= 1<<12; //ʹ�����
      MSD_StepCounter2(srd2.dir);
      step_count++;
      srd2.accel_count++;
      new_step_delay = srd2.step_delay - (((2 * (long)srd2.step_delay) 
                       + rest)/(4 * srd2.accel_count + 1));
      rest = ((2 * (long)srd2.step_delay)+rest)%(4 * srd2.accel_count + 1);
      //����Ƿ�Ϊ���һ��
      if(srd2.accel_count >= 0){
        srd2.run_state = STOP;
      }
      break;
  }
  srd2.step_delay = new_step_delay;
  }
	if(stepPosition2%100==0)printf("�������2��ǰλ��:%d\r\n",stepPosition2);
	if(stepPosition2>6359)stepPosition2=0;
	if(stepPosition2<(-6359))stepPosition2=0;
}

/*********************************************END OF FILE**********************/
