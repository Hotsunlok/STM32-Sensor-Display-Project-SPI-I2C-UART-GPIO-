#include "st7735.h"
#include "stm32f4xx.h"
// ===============================
// PIN CONFIGURATION (SPI2)
// ===============================
// PB10 = SPI2_SCK
// PC3 = SPI2_MOSI
// PB3 = LCD_CS
// PB4 = LCD_DC
// PB5 = LCD_RST
#define LCD_CS_LOW()  (GPIOB->ODR &= ~(1U << 3))
#define LCD_CS_HIGH() (GPIOB->ODR |= (1U << 3))
#define LCD_DC_LOW()  (GPIOB->ODR &= ~(1U << 4))
#define LCD_DC_HIGH() (GPIOB->ODR |= (1U << 4))
#define LCD_RST_LOW()  (GPIOB->ODR &= ~(1U << 5))
#define LCD_RST_HIGH() (GPIOB->ODR |= (1U << 5))
// ===============================
// 1. GPIO INITIALIZATION
// ===============================
void ST7735_GPIO_Init(void)
{
   // Enable GPIOB + GPIOC clock
   RCC->AHB1ENR |= (1U << 1); // GPIOB
   RCC->AHB1ENR |= (1U << 2); // GPIOC
   // --- PB3, PB4, PB5 = CS, DC, RST (OUTPUT) ---
   GPIOB->MODER &= ~((3U << (3*2)) | (3U << (4*2)) | (3U << (5*2)));
   GPIOB->MODER |=  ((1U << (3*2)) | (1U << (4*2)) | (1U << (5*2)));
   GPIOB->OTYPER &= ~((1U<<3) | (1U<<4) | (1U<<5));
   GPIOB->OSPEEDR |= ((3U << (3*2)) | (3U << (4*2)) | (3U << (5*2)));
   GPIOB->PUPDR &= ~((3U << (3*2)) | (3U << (4*2)) | (3U << (5*2)));
   // --- PB10 = SCK (AF5) ---
   GPIOB->MODER &= ~(3U << (10*2));
   GPIOB->MODER |= (2U << (10*2)); // AF mode
   GPIOB->AFR[1] |= (5U << ((10-8)*4)); // AF5 = SPI2
   // --- PC3 = MOSI (AF5) ---
   GPIOC->MODER &= ~(3U << (3*2));
   GPIOC->MODER |= (2U << (3*2)); // AF mode
   GPIOC->AFR[0] |= (5U << (3*4)); // AF5 = SPI2
}
// ===============================
// 2. SPI2 INITIALIZATION
// ===============================
void ST7735_SPI_Init(void)
{
   // Enable SPI2 clock
   RCC->APB1ENR |= (1U << 14); // SPI2EN
   // Disable SPI before config
   SPI2->CR1 &= ~(1U << 6);
   // Master mode
   SPI2->CR1 |= (1U << 2);
   // Baud prescaler = fPCLK / 2 (fast)
   SPI2->CR1 &= ~(1U << 3);
   SPI2->CR1 &= ~(1U << 4);
   SPI2->CR1 &= ~(1U << 5);
   // Clock polarity & phase (Mode 0)
   SPI2->CR1 &= ~(1U << 0); // CPHA = 0
   SPI2->CR1 &= ~(1U << 1); // CPOL = 0
   // Full duplex
   SPI2->CR1 &= ~(1U << 10);
   // MSB first
   SPI2->CR1 &= ~(1U << 7);
   // 8-bit data
   SPI2->CR1 &= ~(1U << 11);
   // Software slave management
   SPI2->CR1 |= (1U << 8) | (1U << 9);
   // Enable SPI2
   SPI2->CR1 |= (1U << 6);
}
// ===============================
// 3. SPI SEND BYTE
// ===============================
void ST7735_WriteByte(uint8_t byte)
{
   while(!(SPI2->SR & (1U << 1))); // Wait TXE
   SPI2->DR = byte;
   while(SPI2->SR & (1U << 7));    // Wait not busy
}
// ===============================
// 4. COMMAND AND DATA WRITERS
// ===============================
void ST7735_WriteCommand(uint8_t cmd)
{
   LCD_DC_LOW();
   LCD_CS_LOW();
   ST7735_WriteByte(cmd);
   LCD_CS_HIGH();
}
void ST7735_WriteData(uint8_t data)
{
   LCD_DC_HIGH();
   LCD_CS_LOW();
   ST7735_WriteByte(data);
   LCD_CS_HIGH();
}
// ===============================
// 5. HARDWARE RESET
// ===============================
void ST7735_Reset(void)
{
   LCD_RST_LOW();
   for(volatile int i=0; i<50000; i++);
   LCD_RST_HIGH();
   for(volatile int i=0; i<50000; i++);
}
// ============================================
// PART 2 — LCD INITIALIZATION + BASIC DRAWING
// ============================================
static uint8_t ColStart = 0;
static uint8_t RowStart = 0;
// ------------------------------------------------
// SET ADDRESS WINDOW
// ------------------------------------------------
void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
   ST7735_WriteCommand(0x2A); // CASET
   ST7735_WriteData(0x00);
   ST7735_WriteData(x0 + ColStart);
   ST7735_WriteData(0x00);
   ST7735_WriteData(x1 + ColStart);
   ST7735_WriteCommand(0x2B); // RASET
   ST7735_WriteData(0x00);
   ST7735_WriteData(y0 + RowStart);
   ST7735_WriteData(0x00);
   ST7735_WriteData(y1 + RowStart);
   ST7735_WriteCommand(0x2C); // RAMWR
}
// ------------------------------------------------
// PUSH A PIXEL (16-bit color)
// ------------------------------------------------
void ST7735_PushColor(uint16_t color)
{
   ST7735_WriteData(color >> 8); // MSB
   ST7735_WriteData(color & 0xFF); // LSB
}
// ------------------------------------------------
// DRAW 1 PIXEL
// ------------------------------------------------
void ST7735_DrawPixel(int16_t x, int16_t y, uint16_t color)
{
   if ((x < 0) || (x >= 128) || (y < 0) || (y >= 160))
       return;
   ST7735_SetAddressWindow(x, y, x, y);
   ST7735_PushColor(color);
}
// ------------------------------------------------
// FILL ENTIRE SCREEN
// ------------------------------------------------
void ST7735_FillScreen(uint16_t color)
{
   ST7735_SetAddressWindow(0, 0, 127, 159);
   for (int i = 0; i < 128 * 160; i++)
   {
       ST7735_PushColor(color);
   }
}
// ------------------------------------------------
// LCD INITIALIZATION SEQUENCE
// ------------------------------------------------
void ST7735_Init(void)
{
   ST7735_GPIO_Init();
   ST7735_SPI_Init();
   ST7735_Reset();
   // ======== SLEEP OUT ========
   ST7735_WriteCommand(0x11);
   for(volatile int i=0; i<300000; i++); // ~120ms delay
   // ======== COLOR MODE ========
   ST7735_WriteCommand(0x3A);
   ST7735_WriteData(0x05); // 16-bit color
   // ======== MEMORY ACCESS CONTROL ========
   ST7735_WriteCommand(0x36);
   ST7735_WriteData(0x00); // default rotation
   // ======== DISPLAY ON ========
   ST7735_WriteCommand(0x29);
}
// ============================================
// PART 3 — FONT + CHARACTER DRAWING + STRING
// ============================================
// ------------------------
// SIMPLE 5x7 FONT TABLE (compact, skips some symbols)
// ------------------------
static const uint8_t Font5x7[][5] = {
   // ASCII 32 to 47 (space to /)
   {0x00,0x00,0x00,0x00,0x00}, //
   {0x00,0x00,0x5F,0x00,0x00}, // !
   {0x00,0x07,0x00,0x07,0x00}, // "
   {0x14,0x7F,0x14,0x7F,0x14}, // #
   {0x24,0x2A,0x7F,0x2A,0x12}, // $
   {0x23,0x13,0x08,0x64,0x62}, // %
   {0x36,0x49,0x55,0x22,0x50}, // &
   {0x00,0x05,0x03,0x00,0x00}, // '
   {0x00,0x1C,0x22,0x41,0x00}, // (
   {0x00,0x41,0x22,0x1C,0x00}, // )
   {0x14,0x08,0x3E,0x08,0x14}, // *
   {0x08,0x08,0x3E,0x08,0x08}, // +
   {0x00,0x50,0x30,0x00,0x00}, // ,
   {0x08,0x08,0x08,0x08,0x08}, // -
   {0x00,0x60,0x60,0x00,0x00}, // .
   {0x20,0x10,0x08,0x04,0x02}, // /
   // Numbers 0–9
   {0x3E,0x51,0x49,0x45,0x3E}, // 0
   {0x00,0x42,0x7F,0x40,0x00}, // 1
   {0x42,0x61,0x51,0x49,0x46}, // 2
   {0x21,0x41,0x45,0x4B,0x31}, // 3
   {0x18,0x14,0x12,0x7F,0x10}, // 4
   {0x27,0x45,0x45,0x45,0x39}, // 5
   {0x3C,0x4A,0x49,0x49,0x30}, // 6
   {0x01,0x71,0x09,0x05,0x03}, // 7
   {0x36,0x49,0x49,0x49,0x36}, // 8
   {0x06,0x49,0x49,0x29,0x1E}, // 9
   // Uppercase A–Z
   {0x7E,0x11,0x11,0x11,0x7E}, // A
   {0x7F,0x49,0x49,0x49,0x36}, // B
   {0x3E,0x41,0x41,0x41,0x22}, // C
   {0x7F,0x41,0x41,0x22,0x1C}, // D
   {0x7F,0x49,0x49,0x49,0x41}, // E
   {0x7F,0x09,0x09,0x09,0x01}, // F
   {0x3E,0x41,0x49,0x49,0x7A}, // G
   {0x7F,0x08,0x08,0x08,0x7F}, // H
   {0x00,0x41,0x7F,0x41,0x00}, // I
   {0x20,0x40,0x41,0x3F,0x01}, // J
   {0x7F,0x08,0x14,0x22,0x41}, // K
   {0x7F,0x40,0x40,0x40,0x40}, // L
   {0x7F,0x02,0x0C,0x02,0x7F}, // M
   {0x7F,0x04,0x08,0x10,0x7F}, // N
   {0x3E,0x41,0x41,0x41,0x3E}, // O
   {0x7F,0x09,0x09,0x09,0x06}, // P
   {0x3E,0x41,0x51,0x21,0x5E}, // Q
   {0x7F,0x09,0x19,0x29,0x46}, // R
   {0x46,0x49,0x49,0x49,0x31}, // S
   {0x01,0x01,0x7F,0x01,0x01}, // T
   {0x3F,0x40,0x40,0x40,0x3F}, // U
   {0x1F,0x20,0x40,0x20,0x1F}, // V
   {0x3F,0x40,0x38,0x40,0x3F}, // W
   {0x63,0x14,0x08,0x14,0x63}, // X
   {0x07,0x08,0x70,0x08,0x07}, // Y
   {0x61,0x51,0x49,0x45,0x43}, // Z
   // Lowercase a–z
   {0x20,0x54,0x54,0x54,0x78}, // a
   {0x7F,0x48,0x44,0x44,0x38}, // b
   {0x38,0x44,0x44,0x44,0x20}, // c
   {0x38,0x44,0x44,0x48,0x7F}, // d
   {0x38,0x54,0x54,0x54,0x18}, // e
   {0x08,0x7E,0x09,0x01,0x02}, // f
   {0x0C,0x52,0x52,0x52,0x3E}, // g
   {0x7F,0x08,0x04,0x04,0x78}, // h
   {0x00,0x44,0x7D,0x40,0x00}, // i
   {0x20,0x40,0x44,0x3D,0x00}, // j
   {0x7F,0x10,0x28,0x44,0x00}, // k
   {0x00,0x41,0x7F,0x40,0x00}, // l
   {0x7C,0x04,0x18,0x04,0x78}, // m
   {0x7C,0x08,0x04,0x04,0x78}, // n
   {0x38,0x44,0x44,0x44,0x38}, // o
   {0x7C,0x14,0x14,0x14,0x08}, // p
   {0x08,0x14,0x14,0x18,0x7C}, // q
   {0x7C,0x08,0x04,0x04,0x08}, // r
   {0x48,0x54,0x54,0x54,0x20}, // s
   {0x04,0x3F,0x44,0x40,0x20}, // t
   {0x3C,0x40,0x40,0x20,0x7C}, // u
   {0x1C,0x20,0x40,0x20,0x1C}, // v
   {0x3C,0x40,0x30,0x40,0x3C}, // w
   {0x44,0x28,0x10,0x28,0x44}, // x
   {0x0C,0x50,0x50,0x50,0x3C}, // y
   {0x44,0x64,0x54,0x4C,0x44}, // z
   // Final symbols
   {0x00,0x08,0x36,0x41,0x00}, // {
   {0x00,0x00,0x7F,0x00,0x00}, // |
   {0x00,0x41,0x36,0x08,0x00}, // }
   {0x10,0x08,0x08,0x10,0x08}, // ~
   {0x00,0x00,0x00,0x00,0x00}  // DEL
};
// ---------------------------------------------
// DRAW ONE CHARACTER (5x7 font) - FIXED INDEXING
// ---------------------------------------------
void ST7735_DrawChar(int16_t x, int16_t y, char c, uint16_t color)
{
   if (c < 32 || c > 126) return;               // out of range
   // Skip unsupported symbols
   if ((c >= ':' && c <= '@') || (c >= '[' && c <= '`')) return;
   int idx = c - 32;                // start at space
   if (c > '9')  idx -= 7;          // skip :;<=>?@
   if (c > 'Z')  idx -= 6;          // skip [\]^_`
   const uint8_t *bitmap = Font5x7[idx];
   for (int col = 0; col < 5; ++col) {
       uint8_t line = bitmap[col];
       for (int row = 0; row < 8; ++row) {
           if (line & 1)
               ST7735_DrawPixel(x + col, y + row, color);
           line >>= 1;
       }
   }
}
// ---------------------------------------------
// DRAW A STRING
// ---------------------------------------------
void ST7735_DrawString(int16_t x, int16_t y, char *str, uint16_t color)
{
   while (*str) {
       ST7735_DrawChar(x, y, *str, color);
       x += 6;  // 5 pixels wide + 1 space
       str++;
   }
}

void ST7735_FillRect(int x, int y, int w, int h, uint16_t color)
{
    for (int i = x; i < x + w; i++)
    {
        for (int j = y; j < y + h; j++)
        {
            ST7735_DrawPixel(i, j, color);
        }
    }
}
void ST7735_DrawStringScaled(int x, int y, char *str, uint16_t color, uint8_t scale)
{
    while (*str)
    {
        char c = *str;
        str++;

        // --- Correct font indexing (same as DrawChar) ---
        if (c < 32 || c > 126) continue;
        if ((c >= ':' && c <= '@') || (c >= '[' && c <= '`')) continue;

        int idx = c - 32;
        if (c > '9') idx -= 7;
        if (c > 'Z') idx -= 6;

        // Draw each character scaled
        for (int col = 0; col < 5; col++)
        {
            uint8_t line = Font5x7[idx][col];

            for (int row = 0; row < 8; row++)
            {
                if (line & 1)
                {
                    for (int dx = 0; dx < scale; dx++)
                    {
                        for (int dy = 0; dy < scale; dy++)
                        {
                            ST7735_DrawPixel(
                                x + col * scale + dx,
                                y + row * scale + dy,
                                color
                            );
                        }
                    }
                }
                line >>= 1;
            }
        }

        x += 6 * scale;
    }
}



