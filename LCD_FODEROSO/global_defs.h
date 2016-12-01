#ifndef GLOBAL_HEADERS_H_
#define GLOBAL_HEADERS_H_
//#define F_CPU 16000000 este valor é defino pré-parser
	#include <stdint.h> //tipod de largura fixa (8, 16 bits)
	#include <avr/io.h>      //mapeamento de portas
	#include <util/delay.h>  //loops de espera em unidades de tempo
	#include <util/atomic.h> //blocos de execução initerruptos
	#include <avr/interrupt.h> //controle de interrupções

typedef unsigned char byte;

#include "LCD_keyPad.h"
#include "ultrasonic_probe.h"
#include "PWM.h"

//recebem endereço de porta e índice de bit
void   setbit(volatile uint8_t *porta, byte bit);
void clearbit(volatile uint8_t *porta, byte bit);
byte  readbit(volatile uint8_t *porta, byte bit);

void timer2_init();

////////extern volatile byte probeCountEchoByte; //com cristal de 16Mhz, timer com escala 1/64, vai de 0 à 255 em 14.5 cm

//pino relé
#define relay_port PORTC
#define relay_pin PORTC3//PORTD7 // pino 7 do arduino

//pino pwm
#define PWM_PIN PORTD3//PB1

//timer2
#define prescale_timer2 1

//predefinições de posição para bobina (em cm)
#define CONTROL_POSITION_MIDPOINT 7
#define CONTROL_POSITION_ERROR .51
#define CONTROL_MAXIMUM_OUTPUT 1

extern volatile unsigned char probeCountEchoByte; //com cristal de 16Mhz, timer com escala 1/64, contaEco incrementa-se 14.5/cm

#endif /* GLOBAL_HEADERS_H_ */