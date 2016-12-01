/*
 * Arquivo: PWM.cpp
 *
 * Propósito:
   Códigos para escrita de um pwm que controla a fonte de corrente,
  a qual fornece corrente para a bobina
  
  Obs: Timer1 é de 16b
*/

#include "global_defs.h"

void pwm_init()
{
	//Configura o pino como saída
	DDRB |= (1<<PWM_PIN);
	
	/*
	TCCR1A : COM1A1	COM1A0	COM1B1	COM1B0	    -     -     WGM11 	WGM10
	TCCR1B : ICNC1	ICES1      -     WGM13	 WGM12	 CS12	 CS11	CS10 
		Para se operar no modo Fast PWM  de 8bits (auto reload):
			WGM13=0	WGM12=1	WGM11=0	WGM10=1
		Para prescaler de Clock/8	
			CS12=0	CS11=1	CS10=0
		COM1A1 = 1 : O mcu irá zerar o pino de pwm ao chegar no valor de compração
					ou seja, começa com o pino em 1 e muda para zero quando chega
					no valor de compração estipulado pelo registrador OCR1A
					
	Obs: Foi escolhido um prescaler de 8 pois desejamos um pwm próximo a 10khz,
		e pode-se ver que com PS de 8, a freq do pwm é 16E6/(PS*255)=7.843KHz
	*/
	

	TCCR1A &= ~(0x03);
	TCCR1A |= ((1<<COM1A1) | (1<<WGM10));
	TCCR1B &= ~(0x1F);
	TCCR1B |= ((1<<WGM12) | (1<<CS11));
	
	OCR1A = 0; // inicia sem atuação
	
	//Habilita interrupção por overflow
	//TIMSK1  |= (1<<TOIE1);/
}



//Seta o duty cycle do pwm. 0=0V, 255=5V
void pwm_set (unsigned char duty)
{
	OCR1A=duty;
}
