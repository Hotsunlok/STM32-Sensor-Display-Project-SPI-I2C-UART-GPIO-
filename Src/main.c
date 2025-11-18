#include "stm32f4xx.h"
#include <stdio.h>        //  printf() via UART
#include <stdint.h>
#include "st7735.h"      // LCD driver
#include "adxl345.h"     // ADXL345 accelerometer driver
#include "pushbutton.h"  // Button + LED logic
#include "buzzer.h"      // Buzzer control
#include "uart.h"       // UART2 printf output
#include "leds.h"       // LEDS  green and red


// Raw accelerometer values (16-bit signed)
int16_t x, y, z;

// Converted values in g (float)
float xg, yg, zg;

// ADXL345 receives 6 bytes for XYZ
extern uint8_t data_rec[6];

int main(void)
{
	// -----------------------------
    //  INITIALIZE ALL HARDWARE
    // -----------------------------
    ST7735_Init(); // Init LCD display  via SPI
    adxl_init();   // Init accelerometer via I2C
    GPIO_Init_Button_LED(); // Init LED + button GPIO
    Buzzer_Init(); // Init buzzer pin
    uart2_tx_init(); // Init UART2 for printf()
    LEDs_Init(); //Init red+green LED



    while (1)
    {
    	// -----------------------------
    	//  READ ACCELEROMETER VALUES
        // -----------------------------
        adxl_read_values(DATA_START_ADDR);

        // Combine MSB + LSB into signed 16-bit number
        x = (data_rec[1] << 8) | data_rec[0];
        y = (data_rec[3] << 8) | data_rec[2];
        z = (data_rec[5] << 8) | data_rec[4];

        // Convert raw → g value (scale factor = 0.0078 for ±4g)
        xg = x * 0.0078;
        yg = y * 0.0078;
        zg = z * 0.0078;

        // Update LED states (interact with push button)
        GPIO_Update_LED();


        // -----------------------------
        //  FLIP / NORMAL DETECTION
        // -----------------------------
        if (z < 0)      // If Z axis is negative → flipped upside down
        {
        	ST7735_FillScreen(RED);    // Full screen red
        	ST7735_DrawStringScaled(30, 75, "FLIPPED", BLACK, 2); // Big text
        	printf("FLIPPED\n");   // Send to PC via UART
            Buzzer_On();       // Buzzer ON
            LED_Red_On();  //Red LED turns on
            LED_Green_Off(); // Green LED turns off
        }
        else  // Normal position (Z positive)
        {
        	 ST7735_FillScreen(GREEN); // Full screen green in LCD
        	 ST7735_DrawStringScaled(30, 75, "NORMAL", BLACK, 2); // Big text in LCD
        	 printf("NORMAL\n");  // Send to PC via UART
             Buzzer_Off();    // buzzer off
             LED_Green_On();  // Green LED turns on
             LED_Red_Off();   // Red LED turns off
        }

        // -----------------------------
        //  DISPLAY RAW XYZ VALUES
        // ----------------------------
        char buf[20];

        // Clear the bottom rectangle (so numbers don't overlap)
        ST7735_FillRect(0, 105, 128, 40, BLACK);

        // Print X only number
        sprintf(buf, "X %d", x);
        ST7735_DrawString(5, 110, buf, WHITE);

        // Print Y only number
        sprintf(buf, "Y %d", y);
        ST7735_DrawString(5, 120, buf, WHITE);

        // Print Z only number
        sprintf(buf, "Z %d", z);
        ST7735_DrawString(5, 130, buf, WHITE);


        // -----------------------------
        //  SMALL DELAY
        // -----------------------------
        for (volatile int i = 0; i < 200000; i++);
    }

}


