#include <stdint.h>

#include "bsp.h"

#define RED_LED				(29u) 	/* PTE29 */
#define GREEN_LED			(5u)	/* PTD5 */
#define LEFT_BUTTON			(12u)	/* PTC12 */
#define RIGHT_BUTTON		(3u)	/* PTC3 */
#define UART0_TX 			(1) 	/* PTA1 */
#define UART0_RX 			(2) 	/* PTA2 */

#define IR_CLOCK_FREQ		(4000000)
#define CORE_CLOCK_FREQ		(48000000)
#define UART_CLOCK_FREQ		(IR_CLOCK_FREQ)
#define UART_BAUDRATE		(9600)

void setup_board(void) {
	PORTC->PCR[LEFT_BUTTON] |= (0b1010 << 16) | (0b001 << 8) | (0b11 << 0);
	PORTC->PCR[RIGHT_BUTTON] |= (0b1010 << 16) | (0b001 << 8) | (0b11 << 0);
	PORTD->PCR[GREEN_LED] |= (0b001 << 8);
	PORTE->PCR[RED_LED] |= (0b001 << 8);
    
	GPIOD->PDDR |= (1 << GREEN_LED);
	GPIOE->PDDR |= (1 << RED_LED);
	GPIOC->PDDR &= ~(1 << LEFT_BUTTON) & ~(1 << RIGHT_BUTTON);

	NVIC->ISER[0] |= (1 << PORTC_IRQ_NUMBER);
}

void setup_systick(void) {
	SYSTICK->CTRL = (0b11);
	SYSTICK->LOAD = 1500000 - 1;
	SYSTICK->VAL = 0;
}

void setup_ir_clock(void) {
	/* Enable MCGIRCLK */
	MCG->C1 |= (1 << 1);

	/* Setup MCGIRCLK to 4MHz */
	MCG->C2 |= (1 << 0);

	/* Don't Divide MCGIRCLK */
	MCG->SC &= ~(0b111 << 1);

	/* Wait MCGIRCLK update */
	while(!(MCG->S & (1 << 0)));
}

void setup_core_clock(void) {
	/* Select PLL/FLL as clock source */
    MCG->C1 &= ~(0b11 << 6);

	/* Select FLL */
	MCG->C6 &= ~(1 << 6);

    /* Select Internal Reference clock source for FLL */
    MCG->C1 |= (1 << 2);

	/* Wait IRCLK to be selected as FLL input clock */
	while(!(MCG->S & (1 << 4)));
     
    /* Select DCO range as Mid range (DCO generates the FLL Clock) */
    MCG->C4 |= (0b01 << 5);
     
    /* Select DCO frequency as 48Mhz */
    MCG->C4 |= (1 << 7);
}

void setup_peripheral_clock(void) {
	/* Activate UART0 clock */
	SIM->SCGC4 |= (1 << 10);

	/* Activate PORTA clock */
	SIM->SCGC5 |= (1 << 9) | (1 << 11) | (1 << 12) | (1 << 13);

	/* Choose MCGIRCLK to UART0 */
	SIM->SOPT2 |= (0b11 << 26);
}

void setup_clock(void) {
	setup_core_clock();

	setup_ir_clock();

	setup_peripheral_clock();
}

void enable_watchdog(void) {
	SIM->COPC = 0b1100;
}

void disable_watchdog(void) {
	SIM->COPC = 0;
}

void reset_watchdog(void) {
	SIM->SRVCOP = 0x55;
	SIM->SRVCOP = 0xAA;
}

void setup_uart(int uart_clock, int baudrate) {
	/* Setup RX and TX pins */
	PORTA->PCR[UART0_TX] |= (0b010 << 8);
	PORTA->PCR[UART0_RX] |= (0b010 << 8);

	uint16_t sbr = uart_clock/(baudrate*16);

	UART0->BDH = (sbr >> 8) & 0xFF;
	UART0->BDL = sbr & 0xFF;

	/* Enable Transmitter and Receiver */
	UART0->C2 |= (1 << 2) | (1 << 3);
}

void uart_write(char c) {
	while(!(UART0->S1 & (1 << 7)));
	UART0->D = c;
}

char uart_read(void) {
	while(!(UART0->S1 & (1 << 5)));
	return (char) UART0->D;
}

void uart_print(const char *str) {
	while(*str != '\0') {
		uart_write(*str++);
	}
}

void uart_scan(char *str, int n) {
	int i;
	for(i = 0; i < n; i++) {
		str[i] = uart_read();

		if(str[i] == '\0') {
			return;
		}
	}
	str[i] = '\0';
}

int main(void) {
	disable_watchdog();

	setup_clock();

	setup_systick();

	setup_board();

	setup_uart(UART_CLOCK_FREQ, UART_BAUDRATE);

	uart_print("Hello 4\n\r");

	while(1) {
		char c = uart_read();

		if(c == '0') {
			uart_print("Zero!\n\r");
		}
		else {
			uart_print("Not Zero!\n\r");
		}
	}
}

void PORTC_IRQHandler(void) {
	if(PORTC->ISFR & (1 << LEFT_BUTTON)) {
		GPIOE->PTOR |= (1 << RED_LED);

		PORTC->ISFR |= (1 << LEFT_BUTTON);
	}
	else if(PORTC->ISFR & (1 << RIGHT_BUTTON)) {
		GPIOD->PTOR |= (1 << GREEN_LED);

		PORTC->ISFR |= (1 << RIGHT_BUTTON);
	}
}

void SysTick_Handler(void) {
	GPIOE->PTOR |= (1 << RED_LED);
	GPIOD->PTOR |= (1 << GREEN_LED);

	SYSTICK->VAL = 0;
}
