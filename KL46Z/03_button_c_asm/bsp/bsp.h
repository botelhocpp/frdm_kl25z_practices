#ifndef BSP_H
#define BSP_H

#include <stdint.h>

/* Constants */

#define PORTC_IRQ_NUMBER 	(31)

/* Chip Peripherals Structs */

typedef struct {
	uint32_t SOPT1;
	uint32_t SOPT1CFG;      
	uint8_t RESERVED_0[4092];
	uint32_t SOPT2;
	uint8_t RESERVED_1[4];
	uint32_t SOPT4;
	uint32_t SOPT5;
	uint8_t RESERVED_2[4];
	uint32_t SOPT7;  
	uint8_t RESERVED_3[8];
	uint32_t SDID;
	uint8_t RESERVED_4[12];
	uint32_t SCGC4;
	uint32_t SCGC5; 
	uint32_t SCGC6;
	uint32_t SCGC7;
	uint32_t CLKDIV1;
	uint8_t RESERVED_5[4];
	uint32_t FCFG1;
	uint32_t FCFG2;
	uint8_t RESERVED_6[4];
	uint32_t UIDMH;                     
	uint32_t UIDML;              
	uint32_t UIDL;     
	uint8_t RESERVED_7[156];
	uint32_t COPC; 
	uint32_t SRVCOP;
} sim_t;

typedef struct {
	uint32_t PDOR;
	uint32_t PSOR;
	uint32_t PCOR;
	uint32_t PTOR;
	uint32_t PDIR;
	uint32_t PDDR;
} gpio_t;

typedef struct {
	uint32_t PCR[32];
	uint32_t GPCLR;
	uint32_t GPCHR;
	uint8_t RESERVED_0[24];
	uint32_t ISFR;
} port_t;

/* Core Peripherals Structs */

typedef struct {
	uint32_t ISER[1U];      
	uint32_t RESERVED0[31U];
	uint32_t ICER[1U];
	uint32_t RSERVED1[31U];
	uint32_t ISPR[1U];    
	uint32_t RESERVED2[31U];
	uint32_t ICPR[1U];     
	uint32_t RESERVED3[95U];
	uint32_t IP[8U];  
}  nvic_t;

typedef struct {
	uint32_t CTRL;
	uint32_t LOAD;
	uint32_t VAL;
	uint32_t CALIB;
} systick_t;

/* Chip Peripherals Access Layer */

#define NVIC 		((nvic_t *) 	(0xE000E100u))
#define SYSTICK 	((systick_t *) 	(0xE000E010u))

/* Core Peripherals Access Layer */

#define SIM 		((sim_t *) 		(0x40047000u))

#define PORTA 		((port_t *) 	(0x40049000u))
#define PORTB 		((port_t *) 	(0x4004A000u))
#define PORTC 		((port_t *) 	(0x4004B000u))
#define PORTD 		((port_t *) 	(0x4004C000u))
#define PORTE 		((port_t *) 	(0x4004D000u))

#define GPIOA 		((gpio_t *) 	(0x400FF000u))
#define GPIOB 		((gpio_t *) 	(0x400FF040u))
#define GPIOC 		((gpio_t *) 	(0x400FF080u))
#define GPIOD 		((gpio_t *) 	(0x400FF0C0u))
#define GPIOE 		((gpio_t *) 	(0x400FF100u))

#endif
