
/*
 * Arquivo: main.c
 *
 * Propósito:
   Arquivo principal do projeto das cadeiras de sistemas de controle e micro-
  controladores da UFRGS no período de 2016/2.
   Este programa foi desenvolvido com o intuito de controlar a posição de uma
  bobina ao longo de hastes ferromagneticas, as quais, juntamente com ímãs de 
  neodímio, formam um circuito magnético fechado, permitindo o controle da 
  posição da bobina através da manipulação da corrente entregue à mesma.
 *
 * 
 *
 * Autores : Luciano Bongiorni 
			 Bruno Zanchetta
			 Ícaro Dupont
 */ 

#include "global_defs.h"


//variável de acesso a contagem da sonda
extern volatile unsigned char probeCountEchoByte=0; //com cristal de 16Mhz, timer com escala 1/64, contaEco incrementa-se 14.5/cm
int main(void)
{
	float position = 0,control = CONTROL_POSITION_MIDPOINT; //em cm
	float sgn_error0 = 0, sgn_error1,control_signal0 = 0,control_signal1,T_clk = 0, T_clk_past = 0,T_40 = 0,T_50 = 0,K_1 = 0,K_2 = 0,K_3 = 0;;
	byte position_in_mm = 0, control_in_mm = control*10, step_control = 1, duty_cycle;//, mode_flag = 0;
	byte LCD_key = 0,LCD_key_last = 0;
	
	//inicializações necessárias
	LCD_keypad_setup();
	probe_setup();
	//pwm_init();
	clear_LCD();
	fill_LCD();
	timer2_init();
	setbit(&DDRD,relay_pin);// configura pino do relé como saída

    while (1) 
    {
		LCD_key_last = LCD_key;
		LCD_key = keypad_tecla_media4();
		
		probe_request();
		position = ((probeCountEchoByte)*14.5)/255;
		
		//executa a ação específica baseado no botão pressionado SE HOUVER ALTERAÇÃO DO MESMO
		if((LCD_key!=LCD_key_last)&&(LCD_key!=keyp_free))
		{
			switch(LCD_key)
			{
				case keyp_up:		control += step_control; break;
				case keyp_down:		control += -step_control; break;
				case keyp_right:	step_control++; break;
				case keyp_left:		if(step_control > control) step_control--; break;
				case keyp_select:	control = CONTROL_POSITION_MIDPOINT; break;
				default:			break;
			}
		}
		
		//determina a direção de movimento necessária
		sgn_error1 = sgn_error0;
		if(position > control){
			setbit(&relay_port,relay_pin);
			sgn_error0 = position - control;
		}
		else{
			clearbit(&relay_port,relay_pin);
			sgn_error0 = control - position;
		}
		
		
		//determina o duty do pwm do controlador
		T_clk_past = T_clk;
		T_clk = get_time();
		if(sgn_error0 < CONTROL_POSITION_ERROR)
			control_signal0 = 0;
		else{
			//T_clk = período em segundos entre samples, para uso na definição da função de controle, medido com o timer
			//define o controlador, e recalcula as constantes dependentes do período de sample caso o mesmo mude
			if((T_clk_past =! T_clk)) {
				T_40 = 40*T_clk;
				T_50 = 50*T_clk;
				K_1 = (2 - T_50)/(T_50 + 2);
				K_2 = 0.06*(T_40 - 2)/(T_50 + 2);
				K_3 = 0.06*(T_40 + 2)/(T_50 + 2);
			}
			control_signal1 = control_signal0;
			control_signal0 = K_1*control_signal1 + K_2*sgn_error1 + K_3*sgn_error0;
			//duty_cycle = control_signal; //possivelmente precisará de uma variável multiplicando
			if(control_signal0 > CONTROL_MAXIMUM_OUTPUT)
				control_signal0 = CONTROL_MAXIMUM_OUTPUT; //se certificando de que o limite é respeitado
		}

		// ??? pos_bobina = joystick_volt;
		duty_cycle = ((control_signal0*255.0)/5);  //duty_cycle = (char)((pos_bobina*255.0)/5.0);
			
		//PWM
		//pwm_set (duty_cycle);
		
		//display
		control_in_mm = (byte)(control*10);
		control_to_lcd(control_in_mm);
		position_in_mm = (byte)(position*10);
		position_to_lcd(position_in_mm);
     }
}