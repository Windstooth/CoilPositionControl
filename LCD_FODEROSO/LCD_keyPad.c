/*
 * lcd_keypad_shield.c
 *
 *
 * Implementação de funções para o projeto controlar o Display.
 * 
 * * Foram concebidas para um Arduino com microcontrolador ATMega328
 * utilizando um Shield de LCD e Keypad.
 * O controlador do LCD é um TC1602A-01T, que utliza padrão de
 * controle do display compatível com o HD44780.
 */ 


#include "global_defs.h"


/*
	Porta de dados do LCD.
	Usa o nibble superior para modo 4 bits.
*/
#define lcd_D_port     PORTD
#define lcd_D_ddr      DDRD

/*
	Porta de controle do LCD.
	Provê acesso aos pinos que controlam a sincronização da 
	comunicação	entre o ATmega e o HD44780.
	A máscara IE simboliza o bit de Enable, 
		quando alto , o HD44780 lerá o dado na porta de dados.
		quando baixo, o HD44780 ignorará a porta de dados.
	A máscara IR simboliza o bit de Registro,
		quando alto , o HD44780 gravará o dado lido como código de caracter,
		quando baixo, o HD44780 gravará o dado lido como comando a ser executado
	A máscara BL simboliza o bit de Backlight 
*/
#define lcd_I_port      PORTB
#define lcd_I_ddr       DDRB
#define lcd_E_bit       PORTB1
#define lcd_RS_bit      PORTB0
#define lcd_BL_bit      PORTB2
byte const lcd_IE_mask = 1 <<  lcd_E_bit;
byte const lcd_IR_mask = 1 <<  lcd_RS_bit;

/*
	Lista de comandos e seu código (do datasheet)
*/
byte const lcd_op_modo  = 0b00101000; // 4 bits, texto de 2 linhas, fonte de 8 linhas
byte const lcd_op_liga  = 0b00001100; //liga display, desliga recursos visuais
byte const lcd_op_edit  = 0b00001111; //liga display, liga recursos visuais
byte const lcd_op_clear = 0b00000001; //limpa memória para 0x20 (espaço) e endereça memória inicial
byte const lcd_op_home  = 0b00000010; //retorna cursor para início do display

byte const lcd_linha_stride = 64; //tamanho da linha na memória contígua interna 

byte lcd_inicializado = 0;  //incrementada em inicialização


////////////////////////////////////////////////////////////////
//	FUNÇÕES INTERNAS
////////////////////////////////////////////////////////////////

void lcd_modo_caracter(){
	lcd_I_port |= lcd_IR_mask;
}

void lcd_modo_comando(){
	lcd_I_port &= (~lcd_IR_mask);	
}

void lcd_habilita(){
	lcd_I_port |= lcd_IE_mask;
}

void lcd_desabilita(){
	lcd_I_port &= (~lcd_IE_mask);
}

void lcd_escrita_pulso(){
	lcd_habilita();
	_delay_us(5);
	lcd_desabilita();
}

void lcd_executa_delay(){
	_delay_us(48);
}

void lcd_envia_dado(byte dado){
	//escreve cada nibble sem alterar outros pinos
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		lcd_D_port &= (0x0F);
		lcd_D_port |= (dado & 0xF0);
		lcd_escrita_pulso();
		lcd_D_port &= (0x0F);
		lcd_D_port |= (0xF0 & ((dado<<4) & 0xF0));
		lcd_escrita_pulso();		
	}
	lcd_executa_delay();
}

void lcd_comando(byte comando){
	lcd_modo_comando();
	lcd_envia_dado(comando);
	if((comando == lcd_op_clear)||(comando == lcd_op_home))
	{
		_delay_ms(2.11); //Comandos clear e home necessitam maior delay. Valor fornecido pelo datasheet.
	}
}

void lcd_envia_caracter(char caracter){
	lcd_modo_caracter();
	lcd_envia_dado(caracter);
}

void lcd_cursor_pos(int8_t x, int8_t y){
	if((x>=40) || (x<0) || (y!=0 && y!=1))
	return;
	byte offset = (x + (y*lcd_linha_stride));
	lcd_comando(0x80 | offset); //carrega endereço para addr_counter
}

//retorna valor absolute de int16
uint16_t abs16(int16_t v){
	if(v<0)return -v;
	return v;
}

enum keypad_teclas conv_adc8bits_tecla(byte v)
{
	static const byte v_cm = 32;
	static const byte v_bx = 76;
	static const byte v_eq = 120;
	static const byte v_dr = 0;
	static const byte v_sl = 180;
	int16_t c = v;
	if(abs16(c-v_cm)<8)
	return keyp_up;
	if(abs16(c-v_bx)<8)
	return keyp_down;
	if(abs16(c-v_eq)<8)
	return keyp_left;
	if(abs16(c-v_dr)<8)
	return keyp_right;
	if(abs16(c-v_sl)<8)
	return keyp_select;
	return keyp_free;
}

////////////////////////////////////////////////////////////////
//	FUNÇÕES DE SETUP
////////////////////////////////////////////////////////////////

void lcd_setup(){
	if(lcd_inicializado) return;
	//setup atômico, restaura interrupções ao final
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		lcd_D_ddr |= 0xF0; // ATmega escreve para nibble superior
		lcd_I_ddr |= (lcd_IE_mask | lcd_IR_mask); // ATmega escreve para bits de controle
		clearbit(&lcd_I_ddr, lcd_BL_bit); //pino de backlight em alta impedância (entrada)
	
		lcd_desabilita();
		_delay_ms(40); //tempo de espera recomendado em reset

		/*
			O estado inicial do LCD é desconhecido, mas uma sequência de
			chamadas para o modo de 8bits resulta no mesmo estado por 
			qualquer estado inicial de funcionamento.
			Uma vez garantido que o LCD está operando em 8bits, é possível
			enviar um comando para alterar para o modo de 4bits.
		*/
		lcd_modo_comando();  //ATmega sinaliza que vai enviar comando
		lcd_D_port &= (0x0F);
		lcd_D_port |= (0x30);//comando para selecionar modo 8 bits
		lcd_habilita();
		_delay_ms(5); //tempo de espera recomendado na primeira escrita
		lcd_desabilita();
	
		lcd_D_port &= (0x0F);
		lcd_D_port |= (0x30);//comando para selecionar modo 8 bits
		lcd_habilita();
		_delay_us(150); //tempo de espera recomendado  na segunda escrita
		lcd_desabilita();
	
		lcd_D_port &= (0x0F);
		lcd_D_port |= (0x30);//comando para selecionar modo 8 bits
		lcd_escrita_pulso();
		lcd_executa_delay();
	
		lcd_D_port &= (0x0F);
		lcd_D_port |= (0x20);//comando para selecionar modo 4 bits
		lcd_escrita_pulso();
		lcd_executa_delay();
			
		//sinaliza que LCD foi inicilizado		
		lcd_inicializado++;
			
		lcd_comando(lcd_op_modo);
		lcd_comando(lcd_op_liga);
		LCD_soft_reset();
		
	}
}

void keypad_leitura_setup(){
	ADMUX = 0;
	//escolhe canal
	ADMUX  &= 0xf8; //mux 0 (trivial já que ADMUX foi zerado)
	//enable
	setbit(&ADCSRA, ADEN);
	//ref = ivcc
	clearbit(&ADMUX, REFS1);
	setbit(&ADMUX, REFS0);
	//scale = 128 (pre-definido para 111)
	setbit(&ADCSRA, ADPS0);
	setbit(&ADCSRA, ADPS1);
	setbit(&ADCSRA, ADPS2);
	setbit(&ADCSRA, ADSC); //começa nova conversão
}

////////////////////////////////////////////////////////////////
//	FUNÇÕES EXPOSTAS
////////////////////////////////////////////////////////////////

void LCD_keypad_setup(){
	lcd_setup();
	keypad_leitura_setup();
};

void LCD_clear(){
	if(!lcd_inicializado) return;
	lcd_comando(lcd_op_clear);	
}

void LCD_soft_reset(){
	if(!lcd_inicializado) return;
	lcd_comando(lcd_op_clear);
	lcd_comando(lcd_op_home);
}

enum keypad_teclas keypad_tecla_media4(){
	static byte n = 0;
	static int16_t r[3]; //buffer de leituras anteriores
	static enum keypad_teclas tecla = keyp_free;
	byte k = 0;
	//verifica se uma nova conversão anterior está pronta
	if(readbit(&ADCSRA, ADIF))
	{
		//ler usando sincronia do buffer: byte baixo depois alto
		byte baixo = ADCL;
		byte alto  = ADCH;
		int16_t nr = (0x03 & alto); //filtra bits fora dos 10 de precisão (apenas dois bits válidos no byte alto)
		nr <<= 8;
		nr |= baixo;
		while(k<n)
		{
			if(abs16(nr-r[k])>8) break;
			k++;
		}
		if(k==3)
		{
			r[2] = ((r[0]+r[1]+r[2]+nr) >> 4);
			tecla = conv_adc8bits_tecla(r[2] & 0x00ff);
			n = 0;
		}
		else
		{
			if(k!=n)
			{
				n = 0;
			}
		}
		r[n] = nr;
		n++;
		setbit(&ADCSRA, ADSC); //requisita nova conversão
	}
	return tecla;
}

void LCD_send_text(char const *txt, byte x0, byte y0){
	if((!lcd_inicializado) || (x0>15) || (y0>1))
		return;
	byte pos = 0; //posição na string fornecida
	lcd_cursor_pos(x0, y0); //posição na tela
	lcd_modo_caracter();
	while (((x0+pos)<16) && (txt[pos]!=0) )
	{
		lcd_envia_dado(txt[pos++]);
	}
}

void clear_LCD()
{
	LCD_send_text("                ", 0, 0);
	LCD_send_text("                ", 0, 1);
}

void fill_LCD()
{
	//linha 0
	LCD_send_text(",Ctr=", 6, 0);
	LCD_send_text("mm", 14, 0);
	//linha 1
	LCD_send_text(",Pos=", 6, 1);
	LCD_send_text("mm", 14, 1);
}

void control_to_lcd(byte eco)
{
	static char buff[4]={0,0,0,0};
	buff[0] = (eco/100)      + '0';
	buff[1] = ((eco%100)/10) + '0';
	buff[2] = (eco%10)       + '0';
	LCD_send_text(buff, 11, 0);
}

void position_to_lcd(byte eco)
{
	static char buff[4]={0,0,0,0};
	buff[0] = (eco/100)      + '0';
	buff[1] = ((eco%100)/10) + '0';
	buff[2] = (eco%10)       + '0';
	LCD_send_text(buff, 11, 1);
}