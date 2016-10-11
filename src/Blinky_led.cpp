/*
===============================================================================
 Name        : Blinky_led.cpp
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
#define LED_BLUE 16
#define LED_RED 7

void GPIO_init(int pin) {
	int port = 0;
	bool setting = true;

	LPC_GPIO_PORT->DIR[port] |= 1UL << pin;

	/* LEDs are off when the pin is in up state */	LPC_GPIO_PORT->B[port][pin] = setting;
}

void led_on(int pin){
	int port = 0;
	bool setting = true;

	/* LEDs are on when the pin is in down state */	LPC_GPIO_PORT->B[port][pin] = !setting;
}

void led_off(int pin){
	int port = 0;
	bool setting = true;

	LPC_GPIO_PORT->B[port][pin] = setting;
}

void delay() {
	// Avoid code optimization
	volatile static int i;
	for(i = 0; i <= 500000; i++);
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

    GPIO_init(LED_GREEN);
    GPIO_init(LED_BLUE);
    GPIO_init(LED_RED);
    // Enter an infinite loop
    while(true) {
        led_on(LED_GREEN);
        delay();
        led_off(LED_GREEN);
        delay();
        led_on(LED_BLUE);
        delay();
        led_off(LED_BLUE);
        delay();
        led_on(LED_RED);
        delay();
        led_off(LED_RED);
        delay();
    }

    // Unreachable code
    return 0 ;
}
