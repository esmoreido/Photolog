#ifndef __MS5803_H
#define __MS5803_H
#include "stm8l15x.h"
#include "stm8l15x_conf.h"

#define SENSOR_CMD_RESET      0x1E
#define SENSOR_CMD_ADC_READ   0x00
#define SENSOR_CMD_ADC_CONV   0x40
#define SENSOR_CMD_ADC_D1     0x00
#define SENSOR_CMD_ADC_D2     0x10
#define SENSOR_CMD_ADC_256    0x00
#define SENSOR_CMD_ADC_512    0x02
#define SENSOR_CMD_ADC_1024   0x04
#define SENSOR_CMD_ADC_2048   0x06
#define SENSOR_CMD_ADC_4096   0x08

uint16_t sensorCoefficients[8];
float     deltaTemp;    // These three variable are used for the conversion.
float     sensorOffset;
float     sensitivity;

float press;
float temp;

void resetsensor(void);
void MS5803_init(void);
uint16_t coefficient_read(uint8_t index);
uint32_t d1_read(void);
uint32_t d2_read(void);
void MS5803_read(void);
#endif