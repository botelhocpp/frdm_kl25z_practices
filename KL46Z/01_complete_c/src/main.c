#include <inttypes.h>
#include <stdint.h>

/* Register Access */

#define HWREG(addr)     (*((volatile uint32_t *) (addr)))

/* SIM */
#define SIM_ADDR 		(0x40047000u)
#define SIM_SCGC5 		(0x1038)

/* PORT */
#define PORTB_ADDR 		(0x4004A000u)
#define PORTD_ADDR 		(0x4004C000u)
#define PORT_PCR(x) 	(0x0004u * x)

/* GPIO */

#define GPIOB_ADDR 		(0x400FF040u)
#define GPIOD_ADDR 		(0x400FF0C0u)
#define GPIO_PSOR  		(0x0004u)
#define GPIO_PCOR  		(0x0008u)
#define GPIO_PDDR  		(0x0014u)

int main() {
	HWREG(SIM_ADDR + SIM_SCGC5) |= (1 << 12);
    
	HWREG(PORTD_ADDR + PORT_PCR(5))  |= (1 << 8);
    
	HWREG(GPIOD_ADDR + GPIO_PDDR) |= (1 << 5);
    
	HWREG(GPIOD_ADDR + GPIO_PCOR) |= (1 << 5);

	while(1);
}

__attribute__((naked, noreturn)) void _start(void) {
    extern uint32_t _sbss, _ebss, _sdata, _edata, _sdata_ram;
    
    for (uint32_t *src = &_sbss; src < &_ebss; src++) {
        *src = 0;
    }
    for (uint32_t *src = &_sdata, *dst = &_sdata_ram; src < &_edata; src++, dst++) {
        *src = *dst;
    }

    main();

    while(1);
}

extern void _estack(void);

__attribute__((section(".vectors"))) void (*tab[48])(void) = {
    _estack, 
    _start
};

__attribute__((section(".cfmconfig"))) uint32_t(cfm[4]) = {
    0xFFFFFFFF, 
    0xFFFFFFFF, 
    0xFFFFFFFF, 
    0xFFFFFFFE
};
