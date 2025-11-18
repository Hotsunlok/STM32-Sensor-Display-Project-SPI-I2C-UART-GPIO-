#include "uart.h"



#define GPIOAEN          (1U<<0)   // Enable clock for GPIOA
#define UART2EN          (1U<<17)  // Enable clock for USART2

#define CR1_TE           (1U<<3)   // Transmitter enable
#define CR1_RE           (1U<<2)   // Receiver enable
#define CR1_UE           (1U<<13)  // USART enable
#define SR_TXE           (1U<<7)   // Transmit data register empty
#define SR_RXNE          (1U<<5)   // Read data register is not empty



#define SYS_FREQ         16000000
#define APB1_CLK         SYS_FREQ

#define UART_BAUDRATE    115200


// Function prototypes
static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeripClk, uint32_t BaudRate);
static uint16_t compute_uart_bd(uint32_t PeripClk, uint32_t BaudRate);


void uart2_write(int ch);


void uart2_write(int ch);  // forward declaration

int __io_putchar(int ch)
{
    uart2_write(ch);
    return ch;
}

void uart2_rxtx_init(void)
{
    /*************** Configure UART GPIO pin ****************/
    /* Enable clock access to GPIOA (PA2) */
    RCC->AHB1ENR |= GPIOAEN;

    /* Set PA2 mode to alternate function mode */
    GPIOA->MODER &= ~(1U << 4);
    GPIOA->MODER |=  (1U << 5);

    /* Set PA2 alternate function type to UART_TX (AF07) */
    GPIOA->AFR[0] |=  (1U << 8);
    GPIOA->AFR[0] |=  (1U << 9);
    GPIOA->AFR[0] |=  (1U << 10);
    GPIOA->AFR[0] &= ~(1U << 11);

    /* Set PA3 mode to alternate function mode */
    GPIOA->MODER &= ~(1U << 6);
    GPIOA->MODER |=  (1U << 7);
    /* Set PA3 alternate function type to UART_RX (AF07) */
    GPIOA->AFR[0] |=  (1U << 12);
    GPIOA->AFR[0] |=  (1U << 13);
    GPIOA->AFR[0] |=  (1U << 14);
    GPIOA->AFR[0] &= ~(1U << 15);

    /*************** Configure UART module ****************/
    /* Enable clock access to USART2 */
    RCC->APB1ENR |= UART2EN;

    /* Configure baud rate */
    uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

    /* Configure transfer direction (TX ,RX) */
    USART2->CR1 = (CR1_TE | CR1_RE );

    /* Enable UART module */
    USART2->CR1 |= CR1_UE;
}




/*******************************************************
 * Function: uart2_tx_init
 * Description: Configure PA2 as USART2_TX (AF7) and initialize UART2
 *******************************************************/
void uart2_tx_init(void)
{
    /*************** Configure UART GPIO pin ****************/
    /* Enable clock access to GPIOA (PA2) */
    RCC->AHB1ENR |= GPIOAEN;

    /* Set PA2 mode to alternate function mode */
    GPIOA->MODER &= ~(1U << 4);
    GPIOA->MODER |=  (1U << 5);

    /* Set PA2 alternate function type to UART_TX (AF07) */
    GPIOA->AFR[0] |=  (1U << 8);
    GPIOA->AFR[0] |=  (1U << 9);
    GPIOA->AFR[0] |=  (1U << 10);
    GPIOA->AFR[0] &= ~(1U << 11);

    /*************** Configure UART module ****************/
    /* Enable clock access to USART2 */
    RCC->APB1ENR |= UART2EN;

    /* Configure baud rate */
    uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

    /* Configure transfer direction (TX only) */
    USART2->CR1 = CR1_TE;

    /* Enable UART module */
    USART2->CR1 |= CR1_UE;
}

char uart_read(void)
{
	/* Wait until receive data register is not empty */
	while(!(USART2->SR & SR_RXNE)){}

	/*Read data*/
	return USART2->DR;
}






/*******************************************************
 * Function: uart2_write
 * Description: Send a single character through UART2
 *******************************************************/
void uart2_write(int ch)
{
    /* Wait until transmit data register is empty */
    while(!(USART2->SR & SR_TXE)){}

    /* Write to transmit data register */
    USART2->DR = (ch & 0xFF);
}

/*******************************************************
 * Function: uart_set_baudrate
 * Description: Set USART baud rate using computed BRR value
 *******************************************************/
static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeripClk, uint32_t BaudRate)
{
    USARTx->BRR = compute_uart_bd(PeripClk, BaudRate);
}

/*******************************************************
 * Function: compute_uart_bd
 * Description: Compute baud rate register (BRR) value
 *******************************************************/
static uint16_t compute_uart_bd(uint32_t PeripClk, uint32_t BaudRate)
{
    return ((PeripClk + (BaudRate / 2U)) / BaudRate);
}
