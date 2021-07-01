#ifndef __SYSTEM_FUNCTIONS_H
#define __SYSTEM_FUNCTIONS_H
#include "stm8l15x.h"
#include "diskio.h"
#include "ff.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "stm8l15x_conf.h"
#include "RTC.h"
/*Specific fatfs functions*/

#define  MISO_READ      GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)
#define  MOSI_SET       GPIOC->ODR |= GPIO_Pin_3;
#define  MOSI_RESET     GPIOC->ODR&= (uint8_t)(~GPIO_Pin_3);
#define  SCK_SET        GPIOC->ODR |= GPIO_Pin_4;
#define  SCK_RESET      GPIOC->ODR&= (uint8_t)(~GPIO_Pin_4);
#define  CS_LOW         GPIOC->ODR&= (uint8_t)(~GPIO_Pin_2);
#define  CS_HIGH        GPIOC->ODR |= GPIO_Pin_2;


/*Green led on discovery board switcher (PE7 GPIO)*/
#define  LED_ON         GPIOE->ODR |= GPIO_Pin_7;
#define  LED_OFF        GPIOE->ODR&= (uint8_t)(~GPIO_Pin_7);
/*Blue led on discovery board switcher (PC7 GPIO)*/
//#define  LEDERR_ON         GPIOC->ODR |= GPIO_Pin_7;
//#define  LEDERR_OFF        GPIOC->ODR&= (uint8_t)(~GPIO_Pin_7);

#define  SRCLR_HIGH     GPIOC->ODR |= GPIO_Pin_7;
#define  SRCLR_LOW      GPIOC->ODR&= (uint8_t)(~GPIO_Pin_7);
/*Perihial EN (B3 GPIO)*/
#define  PER_ON         GPIOB->ODR |= GPIO_Pin_3;
#define  PER_OFF        GPIOB->ODR&= (uint8_t)(~GPIO_Pin_3);
/*TDS GEN (D0 GPIO)*/
#define  PHOTO_PWR_OFF       GPIOD->ODR |= GPIO_Pin_0;
#define  PHOTO_PWR_ON        GPIOD->ODR&= (uint8_t)(~GPIO_Pin_0);


#define SDCARD_MINUTES              1       // Write data on SD card period in minutes 
#define EEPROM_MINUTES              1       // Write data on EEPROM period in minutes (1440 min means 10 bytes per day, EEPROM / BYTES = 1024/10 = 102 days)
#define HALT_MODE                   0       // halt mode compilation switch
#define INDICATION_LEVEL            2       // 0 - turn off all indication, 1 - only load indication, 2 - load and work indication
#define NUMBER_OF_ITTER             1
#define STEP                        1
uint32_t minutes_counter;
uint8_t write_flag;
typedef enum 
{
  MS5803 = 0,
  MS5540,
}SENSOR;

SENSOR s;
typedef struct 
{
  uint8_t               halt;
  uint16_t              indication_level;
  RTC_TimeTypeDef       start_time;
  uint16_t              step;
  uint8_t               number_of_iter;
  uint16_t              adc_tresh_up;
  uint16_t              adc_tresh_down;
  uint8_t               adc_conv;
}SETINGS;
SETINGS set;
typedef enum 
{
  WHAIT_SD = 0,
  READ_SD,
  SAVE_EEPROM,
  MAIN_LOOP,
  WRITE_TIME,
  INDICATION_ERR,
  READ_SETINGS,
  SLEEP_TIME,
} STATE_MACHINE;
uint8_t iters_counter;
RTC_TimeTypeDef next_time;
STATE_MACHINE status;
void tim4_ini();
void SPI_int(void);                   // hardware SPI initialization
void PWM_int(void);                   // hardware PWM for MCLK MS5540 initialization
void delay_10us_sp(uint32_t ms);      //software nop delay
void delay_ms_sp(uint32_t ms);        //software nop delay
void SoftSPI_send(uint8_t data);      //software SPI byte sender
uint8_t SoftSPI_recive(void);         //software SPI byte reciver
uint8_t parse_time(char *str, uint8_t *hours, uint8_t *minutes);
uint8_t parse_date(char *str, uint8_t *day, uint8_t *mounth, uint8_t *year);
void SoftSPI_send32(uint32_t data);
void GPIO_CLK_int(void);              // hardware GPIO initialization
void data_to_string(char *buffer, RTC_TimeTypeDef time_d, RTC_DateTypeDef data_d, uint16_t *vt_data);
void eeprom_to_string(char *buffer, DATETIME dt);
void get_name(char *file_name, RTC_DateTypeDef data_d);

#endif