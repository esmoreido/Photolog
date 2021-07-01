#ifndef __RTC_TIM4_H
#define __RTC_TIM4_H
#include "stm8l15x.h"
#include "stm8l15x_conf.h"

typedef struct 
{
  uint32_t timestamp;
  int16_t temp;
  uint32_t pres;
}DATETIME;

RTC_InitTypeDef   RTC_InitStr;
RTC_TimeTypeDef   RTC_TimeStr;
RTC_DateTypeDef   RTC_DateStr;
RTC_AlarmTypeDef  RTC_AlarmStr;
void LSE_StabTime(void);
void RTC_config(void);
void go_to_halt(uint8_t eternal_halt);
void go_out_halt(void);
uint8_t bcd_decimal(uint8_t hex);
uint8_t  decimal_bcd (uint8_t Decimal);
void convertUnixTimeToDate(uint32_t t, RTC_DateTypeDef *date, RTC_TimeTypeDef *time);
uint32_t convertDateToUnixTime( RTC_DateTypeDef date, RTC_TimeTypeDef time);
  #endif