/***************************************************************************/
//                               Sonda
/***************************************************************************/

#include "global_defs.h"

//interrup��o por mudan�a de estado da portaD
ISR(sonda_vetor_IE) {
	if (sonda_Porta_EchoLeitura & (1 << sonda_Pino_Echo)) {
		//sinal subiu, zera timer
		TCNT0 = 0;  //coloca timer em 0
		TCCR0B = 3; //roda com escala = 1/64
	}
	else {
		//sinal desceu, conta tempo
		TCCR0B = 0; //p�ra timer
		byte muitolonge = TIFR0 & (1 << TOV0); //verifica overflow do timer (pulso muito longo -> objeto muito longe)
		if(muitolonge)
			probeCountEchoByte = 0xff; //valor m�ximo
		else
			probeCountEchoByte = TCNT0;
		setbit(&TIFR0, TOV0); //zera flag overflow 
		                      //ela � zerada escrevendo UM (ao inv�s do intuitivo ZERO)
							            //esta informa��o est� no datasheet
	}
}

void probe_setup()
{
	clearbit(  &sonda_Porta_EchoDDR, sonda_Pino_Echo); // Define pino de eco como  entrada
	setbit(&sonda_Porta_EchoLeitura, sonda_Pino_Echo); // Liga pull up de pino de eco
	
	setbit(&sonda_Porta_TriggerDDR, sonda_Pino_Trigger); // Define pino de trigger como sa�da
	
	setbit(&PCICR, sonda_bit_IE); //Habilita interrup��o por mudan�a de estado
	setbit(&sonda_mascara_IE, sonda_bit_mascara_IE);//Habilita interrup��o por mudan�a do pino da porta echo
	
	sei(); //habilita interrup��es, globalmente
}

//fun��o que envia pulso de trigger para requisitar medi��o pela sonda
void probe_request()
{
	setbit(&sonda_Porta_TriggerEscrita, sonda_Pino_Trigger); //liga trigger
	_delay_us(10); //largura m�nima (datasheet da sonda)
	clearbit(&sonda_Porta_TriggerEscrita, sonda_Pino_Trigger); //desliga trigger
}
/////////////////////////////////////////////////////////////////////////////////
