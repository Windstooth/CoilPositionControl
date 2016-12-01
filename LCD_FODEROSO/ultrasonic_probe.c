/***************************************************************************/
//                               Sonda
/***************************************************************************/

#include "global_defs.h"

//interrupção por mudança de estado da portaD
ISR(sonda_vetor_IE) {
	if (sonda_Porta_EchoLeitura & (1 << sonda_Pino_Echo)) {
		//sinal subiu, zera timer
		TCNT0 = 0;  //coloca timer em 0
		TCCR0B = 3; //roda com escala = 1/64
	}
	else {
		//sinal desceu, conta tempo
		TCCR0B = 0; //pára timer
		byte muitolonge = TIFR0 & (1 << TOV0); //verifica overflow do timer (pulso muito longo -> objeto muito longe)
		if(muitolonge)
			probeCountEchoByte = 0xff; //valor máximo
		else
			probeCountEchoByte = TCNT0;
		setbit(&TIFR0, TOV0); //zera flag overflow 
		                      //ela é zerada escrevendo UM (ao invés do intuitivo ZERO)
							            //esta informação está no datasheet
	}
}

void probe_setup()
{
	clearbit(  &sonda_Porta_EchoDDR, sonda_Pino_Echo); // Define pino de eco como  entrada
	setbit(&sonda_Porta_EchoLeitura, sonda_Pino_Echo); // Liga pull up de pino de eco
	
	setbit(&sonda_Porta_TriggerDDR, sonda_Pino_Trigger); // Define pino de trigger como saída
	
	setbit(&PCICR, sonda_bit_IE); //Habilita interrupção por mudança de estado
	setbit(&sonda_mascara_IE, sonda_bit_mascara_IE);//Habilita interrupção por mudança do pino da porta echo
	
	sei(); //habilita interrupções, globalmente
}

//função que envia pulso de trigger para requisitar medição pela sonda
void probe_request()
{
	setbit(&sonda_Porta_TriggerEscrita, sonda_Pino_Trigger); //liga trigger
	_delay_us(10); //largura mínima (datasheet da sonda)
	clearbit(&sonda_Porta_TriggerEscrita, sonda_Pino_Trigger); //desliga trigger
}
/////////////////////////////////////////////////////////////////////////////////
