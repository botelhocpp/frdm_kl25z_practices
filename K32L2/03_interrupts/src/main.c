#include <stdint.h>

#include "bsp.h"

#define SW1 		(4)		// BUTTON2: PTA4
#define SW3 		(3) 	// BUTTON1: PTC3

#define LED_GREEN 	(5)		// LED1: PTD5
#define LED_RED 	(31) 	// LED2: PTE31

int main(void) {
	SIM->SCGC5 |= (1U << 9) | (1U << 11) | (1U << 12) | (1U << 13);
    
	PORTA->PCR[SW1] = (0b1010 << 16) | (0b001U << 8) | (0b11U << 0);
	PORTC->PCR[SW3] = (0b1010 << 16) | (0b001U << 8) | (0b11U << 0);
	PORTD->PCR[LED_GREEN] = (0b001U << 8);
	PORTE->PCR[LED_RED] = (0b001U << 8);
    
	GPIOA->PDDR &= ~(1U << SW1);
	GPIOC->PDDR &= ~(1U << SW3);
	GPIOD->PDDR |= (1U << LED_GREEN);
	GPIOE->PDDR |= (1U << LED_RED);

	NVIC->ISER[0] |= (1U << PORTA_IRQ) | (1U << PORTC_PORTD_IRQ);

	/* Config SysTick for 48MHz/16 */
	SYSTICK->CTRL = (0b11);
	SYSTICK->LOAD = 3000000 - 1;
	SYSTICK->VAL = 0;

	while(1) {
		asm volatile("wfi");
	}
}

void _systick_handler(void) {
	GPIOD->PTOR |= (1 << LED_GREEN);
	GPIOE->PTOR |= (1 << LED_RED);

	SYSTICK->VAL = 0;
}

void _porta_handler(void) {
	GPIOD->PTOR |= (1U << LED_GREEN);
	PORTA->ISFR |= (1U << SW1);
}

void _portc_portd_handler(void) {
	GPIOE->PTOR |= (1U << LED_RED);
	PORTC->ISFR |= (1U << SW3);
}
