
#include "global_defs.h"

void setbit(volatile uint8_t *porta, byte bit)
{
	*porta |= 1 << bit;
}

void clearbit(volatile uint8_t *porta, byte bit)
{	
	*porta &= ~(1 << bit);
}

byte readbit(volatile uint8_t *porta, byte bit)
{   
	return ((*porta) & (1<<bit)) != 0 ? 1 : 0;
}


/* ////////////////////////////////////
 *		TIMER 2 -> controle de período
 *////////////////////////////////
extern volatile uint8_t total_overflow = 0;

// initialize timer, interrupt and variable
void timer2_init()
{
	// initialize counter
	TCNT2 = 0;
	
	// enable overflow interrupt
	TIMSK2 |= (1 << TOIE2);
	
	// enable global interrupts
	sei();
	
	// initialize overflow counter variable
	total_overflow = 0;
}

// TIMER0 overflow interrupt service routine
// called whenever TCNT0 overflows
ISR(TIMER2_OVF_vect)
{
	// keep a track of number of overflows
	total_overflow++;
}

float get_time()
{
	float x;
	x = (TCNT2 + total_overflow*((2)^8))/F_CPU;
	TCNT2 = 0;
	total_overflow = 0;
	return x;
}