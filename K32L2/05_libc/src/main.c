#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "bsp.h"

#define SW1 		(4)		// BUTTON2: PTA4
#define SW3 		(3) 	// BUTTON1: PTC3

#define LED_GREEN 	(5)		// LED1: PTD5
#define LED_RED 	(31) 	// LED2: PTE31

#define UART_TX 			(1) // PTA1
#define UART_RX 			(2) // PTA2

#define CORE_CLK_FREQ	(48000000)
#define UART_BAUDRATE	(115200)

static void setup_gpio(void) {
	SIM->SCGC5 |= (1U << 9) | (1U << 11) | (1U << 12) | (1U << 13) | (1 << 20);
    
	PORTA->PCR[SW1] = (0b1010 << 16) | (0b001U << 8) | (0b11U << 0);
	PORTC->PCR[SW3] = (0b1010 << 16) | (0b001U << 8) | (0b11U << 0);
	PORTD->PCR[LED_GREEN] = (0b001U << 8);
	PORTE->PCR[LED_RED] = (0b001U << 8);
    
	GPIOA->PDDR &= ~(1U << SW1);
	GPIOC->PDDR &= ~(1U << SW3);
	GPIOD->PDDR |= (1U << LED_GREEN);
	GPIOE->PDDR |= (1U << LED_RED);

	NVIC->ISER[0] |= (1U << PORTA_IRQ) | (1U << PORTC_PORTD_IRQ);	
}

static void setup_systick(void) {
	/* Config SysTick for 48MHz/16 */
	SYSTICK->CTRL = (0b11);
	SYSTICK->LOAD = 8000000/16 - 1;
	SYSTICK->VAL = 0;
}

static void setup_uart(void) {
	/* Choose MCGIRCLK to LPUART0 */
	SIM->SOPT2 = (0b11 << 26);

	/* Setup RX and TX pins */
	PORTA->PCR[UART_TX] = (0b010 << 8);
	PORTA->PCR[UART_RX] = (0b010 << 8);

	LPUART0->CTRL = 0;

	LPUART0->BAUD = (0b00011 << 24) | (8000000 / (115200 * (4)));

	/* Enable Transmitter and Receiver */
	LPUART0->CTRL = (1 << 19) | (1 << 18);
}

static void uart_write(char c) {
	while(!(LPUART0->STAT & (1 << 23)));

	LPUART0->DATA = c;
}

static char uart_read(void) {
	while(!(LPUART0->STAT & (1 << 21)));

	return LPUART0->DATA;
}

static void uart_print(const char *str) {
	while(*str != '\0') {
		uart_write(*str++);
	}
	while(!(LPUART0->STAT & (1 << 22)));
}

int main(void) {
	/* Enable HIRC, LIRC and Set core clock to 8MHz */
	MCG->C1 = (0b01 << 6) | (1 << 1) | (1 << 0);
	MCG->C2 = (1 << 0);
	MCG->MC = (1U << 7);

	setup_gpio();

	setup_uart();

	setup_systick();

	int a = 10;
	char buffer[20];

	sprintf(buffer, "Value: %d\n\r", a); 

	uart_print("Hello World\n\r");

	uart_print(buffer);

	while(1);
}

void SysTick_Handler(void) {
	GPIOD->PTOR |= (1 << LED_GREEN);
	GPIOE->PTOR |= (1 << LED_RED);

	SYSTICK->VAL = 0;
}

void PORTA_IRQHandler(void) {
	GPIOD->PTOR |= (1U << LED_GREEN);
	PORTA->ISFR |= (1U << SW1);
}

void PORTC_PORTD_IRQHandler(void) {
	GPIOE->PTOR |= (1U << LED_RED);
	PORTC->ISFR |= (1U << SW3);
}
