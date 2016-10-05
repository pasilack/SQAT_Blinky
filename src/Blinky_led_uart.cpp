/*
===============================================================================
 Name        : Blinky_led_uart.cpp
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

#define LED_GREEN 17
#define BAUDRATE 38400
#define UART_STAT_CTSDEL   (1 << 5)
#define UART_STAT_RXBRKDEL (1 << 11)

void GPIO_init(int pin) {
	int port = 0;
	bool setting = true;

	LPC_GPIO_PORT->DIR[port] |= 1UL << pin;

	/* LEDs are off when the pin is in up state */
	LPC_GPIO_PORT->B[port][pin] = setting;
}

void led_on(int pin){
	int port = 0;
	bool setting = true;

	/* LEDs are on when the pin is in down state */
	LPC_GPIO_PORT->B[port][pin] = !setting;
}

void led_off(int pin){
	int port = 0;
	bool setting = true;

	LPC_GPIO_PORT->B[port][pin] = setting;
}

void delay() {
	// Avoid code optimization
	volatile static int i;
	for(i = 0; i <= 100000; i++);
}

void UART_init(uint32_t baudrate)
{

	uint32_t err, uart_fra_multiplier, baudRateGenerator;
	uint32_t systemCoreClock = Chip_Clock_GetMainClockRate();

	LPC_SYSCON->SYSAHBCLKCTRL |=  (1 << 14);	// Enable clock for UART (p. 37)
	LPC_SYSCON -> PRESETCTRL &= ~(1 << 3);		// Assert UART reset (p. 30)
	LPC_SYSCON -> PRESETCTRL |= (1 << 3);		// Clear UART reset (p. 30)

	// Frame configuration
	LPC_USART0->CFG = UART_CFG_DATALEN_8  | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1 | UART_CFG_ENABLE;

	/* Calculate baudrate generator value */
	baudRateGenerator = systemCoreClock / (16 * baudrate);
	err = systemCoreClock - baudRateGenerator * 16 * baudrate;
	uart_fra_multiplier = (err * 0xFF) / (baudRateGenerator * 16 * baudrate);
	LPC_USART0->BRG = baudRateGenerator - 1;	/* baud rate */
	LPC_SYSCON->UARTFRGDIV = 0xFF;/* value 0xFF is always used */
	LPC_SYSCON->UARTFRGMULT = uart_fra_multiplier;
	LPC_SYSCON->UARTCLKDIV = 1;

	/* Clear the status bits */
	LPC_USART0->STAT = UART_STAT_CTSDEL | UART_STAT_RXBRKDEL;
}

void SWM_init()
{
	// Enable SWM clock
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);

	// TX in PI0_4
	// RX in PI0_0
	LPC_SWM->PINASSIGN[0] = 0xffff0004UL;	// (p. 127-128)

	// Enable fixed-pin functions
	LPC_SWM->PINENABLE0 = 0xffffffb3UL;		// (p. 132)

	// Disable SWM clock
	LPC_SYSCON->SYSAHBCLKCTRL &= (~(1<<7));
}

int receive_char_by_UART() {
	while(~LPC_USART0->STAT & UART_STAT_RXRDY);
	while(~LPC_USART0->STAT & UART_STAT_RXIDLE);
	uint32_t data = LPC_USART0->RXDATA;
	return (char) data << 24;
}

int main(void) {

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    // Set the LED to the state of "On"
    Board_LED_Set(0, true);
#endif
#endif

    SWM_init();
    GPIO_init(LED_GREEN);
    UART_init(BAUDRATE);

    // Enter an infinite loop
    while(true) {
    	receive_char_by_UART();
        led_on(LED_GREEN);
        delay();
        led_off(LED_GREEN);
    }

    // Unreachable code
    return 0 ;
}
