#include "stm32f4xx.h"
#include "pushbutton.h"

/* Enable clock for GPIOA and GPIOC */
#define GPIOAEN (1U << 0)
#define GPIOCEN (1U << 2)

/* Pin definitions */
#define LED_PIN (1U << 5)    // PA5 controls the LED
#define BTN_PIN (1U << 13)   // PC13 reads the pushbutton state

/***********************************************************
 * Function: GPIO_Init_Button_LED
 * Purpose : Initialize PA5 as output (LED) and PC13 as input (Button)
 ***********************************************************/
void GPIO_Init_Button_LED(void)
{

	/* 1. Enable clock access for GPIOA and GPIOC */
    RCC->AHB1ENR |= GPIOAEN; // Turn on clock for GPIOA
    RCC->AHB1ENR |= GPIOCEN; // Turn on clock for GPIOC

    // PA5 = output mode(01)
    GPIOA->MODER |=  (1U << 10); // Set bit 10 = 1
    GPIOA->MODER &= ~(1U << 11); // Set bit 11 = 0

    // PC13 = input mode (00)
    GPIOC->MODER &= ~(1U << 26);// Set bit 26 =0
    GPIOC->MODER &= ~(1U << 27);// Set bit 27 =0
}

void GPIO_Update_LED(void)
{
    if (GPIOC->IDR & BTN_PIN)
    {
    	/* Button NOT pressed → PC13 = 1
    	   Turn LED ON by setting PA5 */
        GPIOA->BSRR = LED_PIN;
    }
    else
    {
    	/* Button PRESSED → PC13 = 0
    	   Turn LED OFF using BSRR reset register */
        GPIOA->BSRR = (1U << 21);
    }
}

