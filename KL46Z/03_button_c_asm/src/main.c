#include <stdint.h>

#include "bsp.h"

#define RED_LED			(29u) 	/* PTE29 */
#define GREEN_LED		(5u)	/* PTD5 */
#define LEFT_BUTTON		(12u)	/* PTC12 */
#define RIGHT_BUTTON	(3u)	/* PTC3 */

void board_init(void) {
	SIM->SCGC5 |= (1 << 9) | (1 << 11) | (1 << 12) | (1 << 13);
    
	PORTC->PCR[LEFT_BUTTON] |= (0b001 << 8) | (0b11 << 0);
	PORTC->PCR[RIGHT_BUTTON] |= (0b001 << 8) | (0b11 << 0);
	PORTD->PCR[GREEN_LED] |= (0b001 << 8);
	PORTE->PCR[RED_LED] |= (0b001 << 8);
    
	GPIOD->PDDR |= (1 << GREEN_LED);
	GPIOE->PDDR |= (1 << RED_LED);
	GPIOC->PDDR &= ~(1 << LEFT_BUTTON) & ~(1 << RIGHT_BUTTON);
}

int main(void) {
	board_init();

	while(1) {
		if(!(GPIOC->PDIR & (1 << LEFT_BUTTON))) {
			GPIOE->PCOR |= (1 << RED_LED);
		}
		else {
			GPIOE->PSOR |= (1 << RED_LED);
		}

		if(!(GPIOC->PDIR & (1 << RIGHT_BUTTON))) {
			GPIOD->PCOR |= (1 << GREEN_LED);
		}
		else {
			GPIOD->PSOR |= (1 << GREEN_LED);
		}
	}
}