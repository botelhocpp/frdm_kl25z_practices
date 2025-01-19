#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "bsp.h"

#define LED_GREEN 				(5)		// LED1: PTD5

#define UART_TX 				(1) // PTA1
#define UART_RX 				(2) // PTA2

#define CORE_CLK_FREQ			(48000000)
#define LPUART_CLK_FREQ			(8000000)
#define UART_BAUDRATE			(115200)

#define LIGHT_SENSOR_CHANNEL	(3)

static void setup_clock(void) {
	/* Enable VREF */
	SIM->SCGC4 = (1 << 20);

	/* Enable PORTA-E and LPUART0 */
	SIM->SCGC5 = (1U << 9) | (1U << 11) | (1U << 12) | (1U << 13) | (1 << 20);

	/* Enable ADC0 */
	SIM->SCGC6 = (1U << 27);

	/* Choose MCGIRCLK to LPUART0 */
	SIM->SOPT2 = (0b11 << 26);

	/* Enable HIRC, LIRC and Set core clock to 48MHz */
	MCG->C1 = (0b00 << 6) | (1 << 1) | (1 << 0);
	MCG->C2 = (1 << 0);
	MCG->MC = (1U << 7);
}

static void setup_gpio(void) {
	PORTD->PCR[LED_GREEN] = (0b001U << 8);
	GPIOD->PDDR |= (1 << LED_GREEN);
}

static void setup_systick(void) {
	/* Config SysTick for 48MHz/16 */
	SYSTICK->CTRL = (0b11);
	SYSTICK->LOAD = 48000000/16 - 1;
	SYSTICK->VAL = 0;
}

static void setup_uart(void) {
	/* Setup RX and TX pins */
	PORTA->PCR[UART_TX] = (0b010 << 8);
	PORTA->PCR[UART_RX] = (0b010 << 8);

	LPUART0->CTRL = 0;

	LPUART0->BAUD = (0b00011 << 24) | (LPUART_CLK_FREQ / (UART_BAUDRATE * (4)));

	/* Enable Transmitter and Receiver */
	LPUART0->CTRL = (1 << 19) | (1 << 18);
}

static void setup_adc(void) {
	/* Config ADC */
	ADC0->SC1[0] = (0b11111);
	ADC0->CFG1 = (0b11 << 5) | (1 << 4) | (0b10 << 2) | (0b01); // (BUSCLK/2)/8
	ADC0->CFG2 = 0;
	ADC0->SC2 = (0b01);
	ADC0->SC3 = (1 << 2) | (0b11);
}

static uint16_t adc_read(int channel) {
	ADC0->SC1[0] = (channel);

	while(!(ADC0->SC1[0] & (1U << 7)));

	return ADC0->R[0];
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

extern void _delay_ms(int ms);

int main(void) {
	setup_clock();

	setup_gpio();

	setup_uart();

	setup_adc();

	char buffer[50] = {0}; 

	while(1) {
		uint16_t adc_raw = adc_read(LIGHT_SENSOR_CHANNEL);

		int light_percentage = 100 - (100.0 * adc_raw)/1024;

		itoa(light_percentage, buffer, 10);

		uart_print("Light Percentage: ");
		uart_print(buffer);

		if(light_percentage < 2) {
			uart_print("[LIGHT ON]");
			GPIOD->PCOR |= (1 << LED_GREEN);
		}
		else {
			uart_print("[LIGHT OFF]");
			GPIOD->PSOR |= (1 << LED_GREEN);
		}

		uart_print("\n\r");

		_delay_ms(1000);
	}
}
