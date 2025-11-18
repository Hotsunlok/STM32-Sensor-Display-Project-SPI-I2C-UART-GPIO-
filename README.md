# STM32-Sensor-Display-Project-SPI-I2C-UART-GPIO-
Bare-metal C project using the STM32F401RE Nucleo board. Reads ADXL345 data via I2C, shows values on an ST7735 TFT using SPI, and uses GPIO for button, LEDs, and buzzer. UART prints simple state messages to the PC COM port.

---

## 🧩 **Development Board**

This project is built using the **ST Nucleo-F401RE** board, part of STMicroelectronics’ Nucleo-64 series.
It provides full access to the STM32F401RE ARM Cortex-M4 microcontroller, with onboard ST-Link for easy flashing and debugging.

**Board image (to be added):**

```
![Nucleo-F401RE](images/nucleo_f401re.jpg)
```


---

## ⚙️ **Bare-Metal Implementation (No HAL Libraries)**

All code in this project is written using **bare-metal C**, directly manipulating:

**No HAL or LL drivers are used.**
