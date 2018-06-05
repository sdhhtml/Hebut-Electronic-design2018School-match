#ifndef __MOTOR_H
#define __MOTOR_H
#include "sys.h"
void Motor_init(void);
void forward(void);
void back(void);
void left_turn(void);
void right_turn(void);
void stop(void);
void circle_left(void);
void circle_right(void);
void left_moto(void);
void right_moto(void);
void SxForward(void);//ÉßÐÎ
void SjxForword(void);//ËÄ½ÇÐÎ
#define IN1 PAout(3)                         
#define IN2 PAout(4)                          
#define IN3 PAout(5)                        
#define IN4 PAout(6)  
#endif
