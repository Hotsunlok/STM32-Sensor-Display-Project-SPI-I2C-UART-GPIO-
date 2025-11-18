// st7735.h
#ifndef ST7735_H
#define ST7735_H
#include <stdint.h>
#include "stm32f4xx.h" // Needed for GPIO & SPI register definitions
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
void ST7735_GPIO_Init(void);
void ST7735_SPI_Init(void);
void ST7735_Reset(void);
void ST7735_WriteByte(uint8_t byte);
void ST7735_WriteCommand(uint8_t cmd);
void ST7735_WriteData(uint8_t data);
void ST7735_Init(void);
void ST7735_FillScreen(uint16_t color);
void ST7735_DrawPixel(int16_t x, int16_t y, uint16_t color);
void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void ST7735_PushColor(uint16_t color);
void ST7735_DrawChar(int16_t x, int16_t y, char c, uint16_t color);
void ST7735_DrawString(int16_t x, int16_t y, char *str, uint16_t color);
void ST7735_FillRect(int x, int y, int w, int h, uint16_t color);
void ST7735_DrawStringScaled(int x, int y, char *str, uint16_t color, uint8_t scale);
#endif

