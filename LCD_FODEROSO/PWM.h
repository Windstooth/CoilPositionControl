/*
 * PWM.h
 *
 * Created: 30/11/2016 22:37:28
 *  Author: ADMIN
 */ 


#ifndef PWM_H_
#define PWM_H_
#include "global_defs.h"

volatile char int_cont; //conta o número de interrupções
void pwm_init();
void pwm_set (unsigned char duty);

#endif /* PWM_H_ */