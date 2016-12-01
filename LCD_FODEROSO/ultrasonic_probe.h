/*
 * sonda.h
 *
 * Created: 30/11/2016 19:41:51
 *  Author: ADMIN
 */ 


#ifndef SONDA_H_
#define SONDA_H_
#include "global_defs.h"

/*
	Portas da Sonda.
	Essas defini��es devem estar de acordo com a conex�o f�sica no hardware.
*/
#define sonda_Porta_EchoLeitura PIND     //define porta utilizada pela sonda, para leitura (eco de resposta)
#define sonda_Porta_TriggerEscrita PORTC //define porta utilizada pela sonda, para escrita (gatilho de leitura)
#define sonda_Porta_EchoDDR     DDRD     //define registro para defini��o de pinos da porta da Sonda como de entrada/sa�da
#define sonda_Porta_TriggerDDR  DDRC     //define registro para defini��o de pinos da porta da Sonda como de entrada/sa�da
#define sonda_Pino_Echo    PORTD2  //define pino da porta da sonda utilizado para ler a respota de eco da sonda
#define sonda_Pino_Trigger PORTC1  //define pino da porta da sonda utilizado para requisitar medi��o

#define sonda_bit_IE PCIE2      // posi��o do bit em PCICR que representa a porta utilizada pela sonda (porta de eco)
#define sonda_mascara_IE PCMSK2 // m�scara de bits de interrup��o externa correspondente a porta da sonda
#define sonda_bit_mascara_IE PCINT2 //bit correspondente ao pino de eco na m�scara de bits de interrup��es da porta
#define sonda_vetor_IE PCINT2_vect  //vetor que cont�m a interrup��o correspondente a porta da sonda

extern volatile unsigned char probeCountEchoByte; //com cristal de 16Mhz, timer com escala 1/64, contaEco incrementa-se 14.5/cm
void probe_setup();
void probe_request();

#endif /* SONDA_H_ */