#include <stdint.h>

#include "bsp.h"

#define RED_LED			(29u) 	/* PTE29 */
#define GREEN_LED		(5u)	/* PTD5 */
#define LEFT_BUTTON		(12u)	/* PTC12 */
#define RIGHT_BUTTON	(3u)	/* PTC3 */

void board_init(void) {
	SIM->SCGC5 |= (1 << 9) | (1 << 11) | (1 << 12) | (1 << 13);
    
	PORTC->PCR[LEFT_BUTTON] |= (0b1010 << 16) | (0b001 << 8) | (0b11 << 0);
	PORTC->PCR[RIGHT_BUTTON] |= (0b1010 << 16) | (0b001 << 8) | (0b11 << 0);
	PORTD->PCR[GREEN_LED] |= (0b001 << 8);
	PORTE->PCR[RED_LED] |= (0b001 << 8);
    
	GPIOD->PDDR |= (1 << GREEN_LED);
	GPIOE->PDDR |= (1 << RED_LED);
	GPIOC->PDDR &= ~(1 << LEFT_BUTTON) & ~(1 << RIGHT_BUTTON);

	NVIC->ISER[0] |= (1 << PORTC_IRQ_NUMBER);

	/* Config SysTick */
	SYSTICK->CTRL = (0b11);
	SYSTICK->LOAD = 1500000 - 1;
	SYSTICK->VAL = 0;
}

void _portc_handler(void) {
	if(PORTC->ISFR & (1 << LEFT_BUTTON)) {
		GPIOE->PTOR |= (1 << RED_LED);

		PORTC->ISFR |= (1 << LEFT_BUTTON);
	}
	else if(PORTC->ISFR & (1 << RIGHT_BUTTON)) {
		GPIOD->PTOR |= (1 << GREEN_LED);

		PORTC->ISFR |= (1 << RIGHT_BUTTON);
	}
}

void _systick_handler(void) {
	GPIOE->PTOR |= (1 << RED_LED);
	GPIOD->PTOR |= (1 << GREEN_LED);

	SYSTICK->VAL = 0;
}

int main(void) {
	board_init();

	while(1) {
		asm volatile("WFI");
	}
}