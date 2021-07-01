#include "RTC.h"
#include "ADC.h"
#include "System_functions.h"

 uint8_t bcd_decimal(uint8_t hex)
{
    uint8_t dec = ((hex & 0xF0) >> 4) * 10 + (hex & 0x0F);
    return dec;
}      
uint8_t  decimal_bcd (uint8_t Decimal)
{
   return (((Decimal/10) << 4) | (Decimal % 10));
}

void convertUnixTimeToDate(uint32_t t, RTC_DateTypeDef *date, RTC_TimeTypeDef *time)
 {
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    uint32_t e;
    uint32_t f;
  
    //Negative Unix time values are not supported
    if(t < 1)
       t = 0;

    //Retrieve hours, minutes and seconds
    time->RTC_Seconds = t % 60;
    t /= 60;
    time->RTC_Minutes = t % 60;
    t /= 60;
    time->RTC_Hours = t % 24;
    t /= 24;
  
    //Convert Unix time to date
    a = (uint32_t) ((4 * (uint32_t)t + 102032) / 146097 + 15);
    b = (uint32_t) ((uint32_t)t + 2442113 + (uint32_t)a - ((uint32_t)a / 4));
    c = (20 * (uint32_t)b - 2442) / 7305;
    d = (uint32_t)b - 365 * (uint32_t)c - ((uint32_t)c / 4);
    e = (uint32_t)d * 1000 / 30601;
    f = (uint32_t)d - (uint32_t)e * 30 - (uint32_t)e * 601 / 1000;
  
    //January and February are counted as months 13 and 14 of the previous year
    if(e <= 13)
    {
       c -= 4716;
       e -= 1;
    }
    else
    {
       c -= 4715;
       e -= 13;
    }
  
    //Retrieve year, month and day
    date->RTC_Year = c-2000;
    date->RTC_Month = (RTC_Month_TypeDef)e;
    date->RTC_Date = f;
  
    //Calculate day of week
    //date->RTC_WeekDay = computeDayOfWeek(c, e, f);
 }

uint32_t convertDateToUnixTime( RTC_DateTypeDef date, RTC_TimeTypeDef time)
 {
    uint16_t y;
    uint16_t m;
    uint16_t d;
    //Year
    y = date.RTC_Year+2000;
    //Month of year
    m = date.RTC_Month;
    //Day of month
    d = date.RTC_Date;
  
    //January and February are counted as months 13 and 14 of the previous year
    if(m <= 2)
    {
       m += 12;
       y -= 1;
    }
  
    //Convert years to days
    uint32_t t = (uint32_t)(365 * (uint32_t)y) + (uint32_t)(y / 4) - (uint32_t)(y / 100) + (uint32_t)(y / 400);
    //Convert months to days
    t += (30 *(uint32_t) m) + (3 * ((uint32_t)m + 1) / 5) + (uint32_t)d;
    //Unix time starts on January 1st, 1970
    t -= 719561;
    //Convert days to seconds
    t *= 86400;
    //Add hours, minutes and seconds
    t += (3600 * (uint32_t)time.RTC_Hours) + (60 * (uint32_t)time.RTC_Minutes) + (uint32_t)time.RTC_Seconds;
  
    //Return Unix time
    return t;
 }

void LSE_StabTime(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
  /* Configure TIM4 to generate an update event each 1 s */
  TIM4_TimeBaseInit(TIM4_Prescaler_16384, 123);
  /* Clear update flag */
  TIM4_ClearFlag(TIM4_FLAG_Update);
  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);
  while ( TIM4_GetFlagStatus(TIM4_FLAG_Update) == RESET );
  TIM4_ClearFlag(TIM4_FLAG_Update);
  /* Disable TIM4 */
  TIM4_Cmd(DISABLE);
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, DISABLE);
};


void Calendar_Init(void)
{
  RTC_InitStr.RTC_HourFormat = RTC_HourFormat_24;
  RTC_InitStr.RTC_AsynchPrediv = 0x7F;
  RTC_InitStr.RTC_SynchPrediv = 0x00FF;
  RTC_Init(&RTC_InitStr);
  RTC_DateStructInit(&RTC_DateStr);
  RTC_DateStr.RTC_WeekDay =RTC_Weekday_Thursday;
  RTC_DateStr.RTC_Date =1;
  RTC_DateStr.RTC_Month =RTC_Month_March;
  RTC_DateStr.RTC_Year = 18;
  RTC_SetDate(RTC_Format_BIN, &RTC_DateStr);
  RTC_TimeStructInit(&RTC_TimeStr);
  RTC_TimeStr.RTC_Hours   = 7;
  RTC_TimeStr.RTC_Minutes = 30;
  RTC_TimeStr.RTC_Seconds = 00;
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStr);
  RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);/* Configures the RTC wakeup timer_step = RTCCLK/16 = LSE/16 = 488.28125 us */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);  /* Enable wake up unit Interrupt */
  RTC_WakeUpCmd(DISABLE);
  RTC_SetWakeUpCounter(1); 
};
/*Wake up initialisation*/
void go_out_halt(void)
{
    GPIO_CLK_int();
    
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE); 
    TIM4_Cmd(ENABLE);
    
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
    TIM2_Cmd(ENABLE);
    
    CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE);
    SPI_int();
    SPI_Cmd(SPI1, ENABLE);
    
    //ADC_int();
    
}
/*Disable all and go to active halt*/
void go_to_halt(uint8_t eternal_halt)
{  
   TIM1_Cmd(DISABLE);
   TIM2_CtrlPWMOutputs(DISABLE);
   TIM2_Cmd(DISABLE);
   TIM3_Cmd(DISABLE);
   TIM4_Cmd(DISABLE);
   ADC_Cmd(ADC1, DISABLE);
   PWR_PVDCmd(DISABLE);

   /* Switch to LSI as system clock source */
   /* system clock prescaler: 1*/
   /* Set STM8 in low power */
   PWR_UltraLowPowerCmd(ENABLE);
 
   GPIO_Init(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);
   GPIO_Init(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 |GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);
   GPIO_Init(GPIOC, GPIO_Pin_0 | GPIO_Pin_1  |GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);
   GPIO_Init(GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);
   GPIO_Init(GPIOE, GPIO_Pin_All, GPIO_Mode_Out_PP_Low_Slow);
   GPIO_Init(GPIOF, GPIO_Pin_All, GPIO_Mode_Out_PP_Low_Slow);
   GPIO_Init(GPIOD,  GPIO_Pin_0, GPIO_Mode_Out_PP_High_Slow);
   
   CLK_PeripheralClockConfig(CLK_Peripheral_LCD, DISABLE);
   TIM4_ClearITPendingBit(TIM4_IT_Update);
   TIM4_ClearITPendingBit(TIM4_IT_Trigger);
   EXTI_ClearITPendingBit(EXTI_IT_Pin7);
   /*
   RTC_ITConfig(RTC_IT_WUT, DISABLE);  
   RTC_WakeUpCmd(DISABLE);
   CLK_RTCClockConfig(CLK_RTCCLKSource_Off, CLK_RTCCLKDiv_1);
   CLK_PeripheralClockConfig(CLK_Peripheral_RTC, DISABLE);
   */
   RTC_ClearITPendingBit(RTC_IT_WUT);
   RTC_ClearITPendingBit(RTC_IT_ALRA);
   RTC_ClearITPendingBit(RTC_IT_TAMP);
   RTC_ClearFlag(RTC_FLAG_WUTF);
   halt();
   
/*  
   TIM1_Cmd(DISABLE);
   TIM2_CtrlPWMOutputs(DISABLE);
   TIM2_Cmd(DISABLE);
   TIM3_Cmd(DISABLE);
   TIM4_Cmd(DISABLE);
   ADC_Cmd(ADC1, DISABLE);
   PWR_PVDCmd(DISABLE);

   CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
   CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_LSI);
   CLK_SYSCLKSourceSwitchCmd(ENABLE);

   while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == 0);
   CLK_HSICmd(DISABLE);
   CLK_HSEConfig(CLK_HSE_OFF);

   PWR_UltraLowPowerCmd(ENABLE);

   ADC_Cmd(ADC1, DISABLE);
   ADC_SchmittTriggerConfig(ADC1, ADC_Channel_6,DISABLE);
   
   PWR_PVDCmd(DISABLE);
   PWR_UltraLowPowerCmd(ENABLE);
   
   CLK_PeripheralClockConfig(CLK_Peripheral_LCD, DISABLE);
   CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, DISABLE);
   
   SPI_DeInit(SPI1);
   SPI_Cmd(SPI1, DISABLE);
   CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, DISABLE);
   
   GPIO_Init(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);
   GPIO_Init(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 |GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);
   GPIO_Init(GPIOC, GPIO_Pin_0 | GPIO_Pin_1  |GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);
   GPIO_Init(GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);
   GPIO_Init(GPIOE, GPIO_Pin_All, GPIO_Mode_Out_PP_Low_Slow);
   GPIO_Init(GPIOF, GPIO_Pin_All, GPIO_Mode_Out_PP_Low_Slow);
   
   GPIO_Init(GPIOD,  GPIO_Pin_0, GPIO_Mode_Out_PP_High_Slow);
   
   RTC_ClearITPendingBit(RTC_IT_WUT);
   TIM4_ClearITPendingBit(TIM4_IT_Update);
   EXTI_ClearITPendingBit(EXTI_IT_Pin7);
   halt();
   */
}
void RTC_config(void)
{
  CLK_LSEConfig(CLK_LSE_ON);
  while (CLK_GetFlagStatus(CLK_FLAG_LSERDY) == RESET);
  LSE_StabTime();
  CLK_RTCClockConfig(CLK_RTCCLKSource_LSE, CLK_RTCCLKDiv_1);
  CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);
  Calendar_Init();
}