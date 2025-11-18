#include "stm32f4xx.h"
#include "leds.h"

#define GPIOAEN    (1U << 0)

#define LED_GREEN   (1U << 8)   // PA8
#define LED_RED     (1U << 9)   // PA9

/***********************************************************
 * Function: LEDs_Init
 * Purpose : Configure PA8 and PA9 as output pins
 ***********************************************************/
void LEDs_Init(void)
{
    /* Enable GPIOA clock */
    RCC->AHB1ENR |= GPIOAEN;

    /* Set PA8 as output mode (01) */
    GPIOA->MODER |=  (1U << 16);   // MODER8 bit0 = 1
    GPIOA->MODER &= ~(1U << 17);   // MODER8 bit1 = 0

    /* Set PA9 as output mode (01)*/
    GPIOA->MODER |=  (1U << 18);   // MODER9 bit0 = 1
    GPIOA->MODER &= ~(1U << 19);   // MODER9 bit1 = 0
}

/***********************************************************
 * Turn LEDs ON/OFF
 ***********************************************************/
void LED_Green_On(void)   //led green turns on (PA8 high)
{
	GPIOA->ODR |=  LED_GREEN;
}


void LED_Green_Off(void)  //led green turns off (PA8 off)
{
	GPIOA->ODR &= ~LED_GREEN;
}

void LED_Red_On(void)     //led red turns on (PA9 high)
{
	GPIOA->ODR |=  LED_RED;
}


void LED_Red_Off(void)    //led red turns off (PA9 off)
{
	GPIOA->ODR &= ~LED_RED;
}



