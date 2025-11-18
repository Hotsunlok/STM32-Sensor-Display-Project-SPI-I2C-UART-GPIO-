#include "stm32f4xx.h"
#include "buzzer.h"


#define GPIOAEN   (1U << 0)    /* Enable clock for GPIOA */
#define BUZZER_PIN (1U << 6)   /* Buzzer pin is PA6 */



void Buzzer_Init(void)
{
    // Enable GPIOA clock
    RCC->AHB1ENR |= GPIOAEN;

    // Set PA6 as OUTPUT mode(01)
    GPIOA->MODER |= (1U<<12); // Set bit 12 = 1
	GPIOA->MODER &= ~ (1U<<13); //set bit 13 =0


}



void Buzzer_On(void) // Drive PA6 high (buzzer on)
{
    GPIOA->ODR |= BUZZER_PIN;   // Set pa6 high
}




void Buzzer_Off(void) // Drive PA6 LOW (buzzer off)
{
    GPIOA->ODR &= ~BUZZER_PIN;
}
