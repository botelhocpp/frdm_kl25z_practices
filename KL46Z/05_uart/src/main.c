#include <stdint.h>

#include "bsp.h"

#define UART0_TX 			(1) /* PTA1 */
#define UART0_RX 			(2) /* PTA2 */

#define IR_CLOCK_FREQ		(4000000)
#define CORE_CLOCK_FREQ		(48000000)
#define UART_CLOCK_FREQ		(IR_CLOCK_FREQ)
#define UART_BAUDRATE		(9600)

void setup_ir_clock(void) {
	/* Enable MCGIRCLK */
	MCG->C1 |= (1 << 1);

	/* Setup MCGIRCLK to 4MHz */
	MCG->C2 |= (1 << 0);

	/* Don't Divide MCGIRCLK */
	MCG->SC &= ~(0b111 << 1);
}

void setup_core_clock(void) {
	/* Select PLL/FLL as clock source */
    MCG->C1 &= ~(0b11 << 6);

    /* Select Internal Reference clock source for FLL */
    MCG->C1 |= (1 << 2);
     
    /* Select DCO range as Mid range (DCO generates the FLL Clock) */
    MCG->C4 |= (0b01 << 5);
     
    /* Select DCO frequency as 48Mhz */
    MCG->C4 |= (1 << 7);
}

void setup_peripheral_clock(void) {
	/* Activate UART0 clock */
	SIM->SCGC4 |= (1 << 10);

	/* Activate PORTA clock */
	SIM->SCGC5 |= (1 << 9);

	/* Choose MCGIRCLK to UART0 */
	SIM->SOPT2 |= (0b11 << 26);
}

void setup_clock(void) {
	setup_core_clock();

	setup_ir_clock();

	setup_peripheral_clock();
}

void setup_watchdog(void) {
	SIM->COPC = 0b1100;
}

void reset_watchdog(void) {
	SIM->SRVCOP = 0x55;
	SIM->SRVCOP = 0xAA;
}

void setup_uart(int uart_clock, int baudrate) {
	/* Setup RX and TX pins */
	PORTA->PCR[UART0_TX] |= (0b010 << 8);
	PORTA->PCR[UART0_RX] |= (0b010 << 8);

	// uint16_t sbr = uart_clock/(baudrate*16);
	uint16_t sbr = 4000000/(9600*16);

	UART0->BDH = (sbr >> 8) & 0xFF;
	UART0->BDL = sbr & 0xFF;

	/* Enable Transmitter and Receiver */
	UART0->C2 |= (1 << 2) | (1 << 3);
}

void uart_write(char c) {
	while(!(UART0->S1 & (1 << 7)));
	UART0->D = c;
}

void uart_print(const char *str) {
	while(*str != '\0') {
		uart_write(*str++);
	}
}

int main(void) {
	setup_clock();

	setup_watchdog();

	setup_uart(UART_CLOCK_FREQ, UART_BAUDRATE);

	uart_print("Hello\n\r");

	while(1) {
		reset_watchdog();
	}
}
