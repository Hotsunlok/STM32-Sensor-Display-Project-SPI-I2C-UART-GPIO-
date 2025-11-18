
#ifndef ADXL345_H_
#define ADXL345_H_
#include <stdint.h>

#include "i2c.h"



#define  DEVID_R            0X00
#define DEVICE_ADDR         (0X53)
#define DATA_FORMAT_R       (0X31)
#define POWER_CTL_R         (0X2D)
#define DATA_START_ADDR     (0X32)
#define DATA_FORMAT_R       (0X31)


#define FOUR_G              (0X01)
#define RESET               (0x00)
#define SET_MEASURE_B       (0x08)   //0000 1000






void adxl_init(void);
void adxl_read_values(uint8_t reg);


#endif /* ADXL345_H_ */
