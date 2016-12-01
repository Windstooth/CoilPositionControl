/*
 * LCD_keyPad.h
 *
 * Created: 30/11/2016 18:54:18
 *  Author: ADMIN
 */ 


#ifndef LCD_KEYPAD_H_
#define LCD_KEYPAD_H_

#include "global_defs.h"

//teclas possíveis do keypad
//(convenientemente de mesmo valor que respectivos comandos)
enum keypad_teclas{
	keyp_free=0,
	keyp_up=1,
	keyp_down=2,
	keyp_right=3,
	keyp_left=4,
    keyp_select=5 };

//iniciliza LCD e leitura usando configuração para pinagem do shield
void LCD_keypad_setup();
void LCD_clear();
void LCD_soft_reset();
void LCD_send_text(char const *txt, byte x0, byte y0);
enum keypad_teclas keypad_tecla_media4();

void clear_LCD();
void fill_LCD();
void position_to_lcd(byte eco);

#endif /* LCD_KEYPAD_H_ */