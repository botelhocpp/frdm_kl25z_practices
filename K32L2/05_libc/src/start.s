.syntax unified

.equ SIM_COPC, 0x40048100

# Vector table
.align 2
.section .isr_vector, "a"
    .long   __StackTop                                      
    .long   Reset_Handler                                   
    .long   NMI_Handler                                     
    .long   HardFault_Handler                               
    .long   0                                               
    .long   0                                               
    .long   0                                               
    .long   0                                               
    .long   0                                               
    .long   0                                               
    .long   0                                               
    .long   SVC_Handler                                     
    .long   0                                               
    .long   0                                               
    .long   PendSV_Handler                                  
    .long   SysTick_Handler                                 
                                                            
    .long   DMA0_IRQHandler                                 
    .long   DMA1_IRQHandler                                 
    .long   DMA2_IRQHandler                                 
    .long   DMA3_IRQHandler                                 
    .long   Reserved20_IRQHandler                           
    .long   FTFA_IRQHandler                                 
    .long   LVD_LVW_IRQHandler                              
    .long   LLWU_IRQHandler                                 
    .long   I2C0_IRQHandler                                 
    .long   I2C1_IRQHandler                                 
    .long   SPI0_IRQHandler                                 
    .long   SPI1_IRQHandler                                 
    .long   UART0_IRQHandler                                
    .long   UART1_IRQHandler                                
    .long   UART2_IRQHandler                                
    .long   ADC0_IRQHandler                                 
    .long   CMP0_IRQHandler                                 
    .long   TPM0_IRQHandler                                 
    .long   TPM1_IRQHandler                                 
    .long   TPM2_IRQHandler                                 
    .long   RTC_IRQHandler                                  
    .long   RTC_Seconds_IRQHandler                          
    .long   PIT_IRQHandler                                  
    .long   I2S0_IRQHandler                           
    .long   USB0_IRQHandler                                 
    .long   DAC0_IRQHandler                                 
    .long   TSI0_IRQHandler                                 
    .long   MCG_IRQHandler                                  
    .long   LPTMR0_IRQHandler                               
    .long   SLCD_IRQHandler
    .long   PORTA_IRQHandler 
    .long   PORTC_PORTD_IRQHandler

# Vector table
.section .FlashConfig, "a"
    .long 0xFFFFFFFF
    .long 0xFFFFFFFF
    .long 0xFFFFFFFF
    .long 0xFFFFFFFE

.section .text
.thumb_func
.align 2
.global Reset_Handler
Reset_Handler:
    cpsid i

    eors r0, r0, r0

    # Disable Watchdogs
    ldr r1, =SIM_COPC
    str r0, [r1] 

    # Zero .bss
    ldr r1, =__bss_start__
    ldr r2, =__bss_end__
_zero_bss:
    cmp r1, r2
    bge _copy_data
    str r0, [r1]
    adds r1, r1, #4
    b _zero_bss

    # Copy .data
    ldr r1, =__data_start__
    ldr r2, =__DATA_ROM
    ldr r3, =__data_end__
_copy_data:
    cmp r1, r3
    bge _call_main
    ldr r3, [r2]
    str r3, [r1]
    adds r1, r1, #4
    adds r2, r2, #4
    b _copy_data

_call_main:
    bl __libc_init_array

    cpsie i

    bl main
    b .

.global _delay_ms
_delay_ms:
    ldr r1, =7000
    muls r1, r1, r0
_delay_ms_loop: 
    subs r1, r1, #1
    bne _delay_ms_loop
    mov pc, lr   

.align	1
.thumb_func
.weak _dummy_isr
_dummy_isr:
    b .

.macro def_dummy_isr    isr_name
    .weak \isr_name
    .set  \isr_name, _dummy_isr
.endm

def_dummy_isr    NMI_Handler
def_dummy_isr    HardFault_Handler
def_dummy_isr    SVC_Handler
def_dummy_isr    PendSV_Handler
def_dummy_isr    SysTick_Handler
def_dummy_isr    DMA0_IRQHandler
def_dummy_isr    DMA1_IRQHandler
def_dummy_isr    DMA2_IRQHandler
def_dummy_isr    DMA3_IRQHandler
def_dummy_isr    Reserved20_IRQHandler
def_dummy_isr    FTFA_IRQHandler
def_dummy_isr    LVD_LVW_IRQHandler
def_dummy_isr    LLWU_IRQHandler
def_dummy_isr    I2C0_IRQHandler
def_dummy_isr    I2C1_IRQHandler
def_dummy_isr    SPI0_IRQHandler
def_dummy_isr    SPI1_IRQHandler
def_dummy_isr    UART0_IRQHandler
def_dummy_isr    UART1_IRQHandler
def_dummy_isr    UART2_IRQHandler
def_dummy_isr    ADC0_IRQHandler
def_dummy_isr    CMP0_IRQHandler
def_dummy_isr    TPM0_IRQHandler
def_dummy_isr    TPM1_IRQHandler
def_dummy_isr    TPM2_IRQHandler
def_dummy_isr    RTC_IRQHandler
def_dummy_isr    RTC_Seconds_IRQHandler
def_dummy_isr    PIT_IRQHandler
def_dummy_isr    I2S0_IRQHandler
def_dummy_isr    USB0_IRQHandler
def_dummy_isr    DAC0_IRQHandler
def_dummy_isr    TSI0_IRQHandler
def_dummy_isr    MCG_IRQHandler
def_dummy_isr    LPTMR0_IRQHandler
def_dummy_isr    SLCD_IRQHandler
def_dummy_isr    PORTA_IRQHandler
def_dummy_isr    PORTC_PORTD_IRQHandler
