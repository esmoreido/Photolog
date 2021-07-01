#include "System_functions.h"
#include <string.h>
void tim4_ini()
{
  /*Specific timer for FATFS (Need to wake up SD every 10ms)*/
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
  TIM4_TimeBaseInit(TIM4_Prescaler_128, 131);
  TIM4_ITConfig(TIM4_IT_Update, ENABLE);
  TIM4_Cmd(ENABLE);
}
void SPI_int(void)
{
  /*SPI 8 Mhz, MSB, Master, Mode 0, without hardware CS controller*/
  CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE);
  SPI_DeInit(SPI1);
        SPI_Init
      (
        SPI1,
        SPI_FirstBit_MSB, SPI_BaudRatePrescaler_2, SPI_Mode_Master,
        SPI_CPOL_Low, SPI_CPHA_1Edge,
        SPI_Direction_2Lines_FullDuplex, SPI_NSS_Soft, 0x00
      );

  SPI_Cmd(SPI1, ENABLE);
}
/*Pwm depricated for MS5083*/
void PWM_int(void)
{
  // PWM frq - 32.7 kHz
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE); // Enable tim4
  TIM2_TimeBaseInit(TIM2_Prescaler_1, TIM2_CounterMode_Up, 488);
  TIM2_OC1Init(TIM2_OCMode_PWM2, TIM2_OutputState_Enable,244, TIM2_OCPolarity_Low,TIM2_OCIdleState_Set);
  TIM2_OC1PreloadConfig(ENABLE);
  TIM2_ARRPreloadConfig(ENABLE);
  TIM2_CtrlPWMOutputs(ENABLE);
  TIM2_Cmd(ENABLE);
}

void delay_10us_sp(uint32_t ms) //very precision delay for us
{
  while(ms != 0) {
    uint8_t count = 3;
    while(count != 0) {count--;} // 1 nop - about 3us
    ms--;
  }
}
void delay_ms_sp(uint32_t ms)//very precision delay for ms
{
  delay_10us_sp(ms*100);
}

/*typical software spi design*/
void SoftSPI_send(uint8_t data)
{
  uint8_t counter = 0;
  if(!s)
  {
      while(counter < 8)
      {
          SCK_RESET;
          delay_10us_sp(1); 
          if( ((data << counter)&0x80) != 0x00 ) {MOSI_SET;} else {MOSI_RESET;}    //MSB first, if 1 pull up data pin if 0 pull down
          counter++;
          SCK_SET;
          delay_10us_sp(1); 
      }
    MOSI_RESET;
  }
  else
  {
       while(counter < 8)
      {
        if( ((data << counter)&0x80) != 0x00 ) {MOSI_SET;} else {MOSI_RESET;}   //MSB first, if 1 pull up data pin if 0 pull down
          counter++;
          SCK_SET;
          delay_10us_sp(1);    
          SCK_RESET;
          delay_10us_sp(1);
      }
    MOSI_RESET;
  }
}


uint8_t SoftSPI_recive(void)
{
  
  uint8_t counter = 0;
  uint8_t data = 0x00;
  
 if(!s)
 {
      while(counter < 8)
      {  
          SCK_RESET;  
          SCK_SET;
          if(MISO_READ != 0x00){data = data << 1; data |= 0x01;}
          else data = data << 1;
          counter++;
      }
 }
 else
 {
     while(counter < 8)
    {
          SCK_SET;
          delay_10us_sp(1);    
          SCK_RESET;
          if(MISO_READ != 0x00){data = data << 1; data |= 0x01; }
          else data = data << 1;
          counter++;
     }
 }
  return data;
}

/*typical software spi design*/
void SoftSPI_send32(uint32_t data)
{
  uint32_t counter = 0;
  CS_LOW;
  if(!s)
  {
      while(counter < 32)
      {
          SCK_RESET;
          delay_10us_sp(1); 
          if( ((data << counter)&0x80000000) != 0x00 ) {MOSI_SET;} else {MOSI_RESET;}    //MSB first, if 1 pull up data pin if 0 pull down
          counter++;
          SCK_SET;
          delay_10us_sp(1); 
      }
    MOSI_RESET;
  }
  else
  {
       while(counter < 32)
      {
        if( ((data << counter)&0x80) != 0x00 ) {MOSI_SET;} else {MOSI_RESET;}   //MSB first, if 1 pull up data pin if 0 pull down
          counter++;
          SCK_SET;
          delay_10us_sp(1);    
          SCK_RESET;
          delay_10us_sp(1);
      }
    MOSI_RESET;
  }
  CS_HIGH;
}

void GPIO_CLK_int(void)
{
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI); // 8 Mhz
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
    CLK_HSICmd(ENABLE);
    
    GPIO_Init( GPIOE, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Fast); // LED PIN
    GPIO_Init( GPIOB, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Fast); // PERIHIAL ENABLE PIN
    GPIO_Init( GPIOB, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Fast); // BUTTON
    GPIO_Init( GPIOD, GPIO_Pin_0, GPIO_Mode_Out_PP_High_Fast); // PHOTO PWR EN
    
    GPIO_Init( GPIOC, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast); // LATCH
    GPIO_Init( GPIOC, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Fast); // DATA
    GPIO_Init( GPIOC, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast); // CLK
    GPIO_Init( GPIOC, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Fast); // SRCLR
    
    GPIO_Init( GPIOB, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast);  // CS HARD
    GPIO_Init( GPIOB, GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Fast);  // SCK HARD
    GPIO_Init( GPIOB, GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Fast);  // MOSI HARD
    GPIO_Init( GPIOB, GPIO_Pin_7, GPIO_Mode_In_FL_No_IT);      // MISO HARD
    
    GPIO_Init( GPIOD, GPIO_Pin_1, GPIO_Mode_In_PU_No_IT);      // PHOTO ADC 

   
}

uint8_t parse_time(char *str, uint8_t *hours, uint8_t *minutes)
{
  uint8_t i = 0;
  uint8_t hr = 0;
  uint8_t mt = 0;
  while((!isdigit(str[i])) && (str[i] != '\0')) i++;
  
  if(str[i] != '\n')
  {
    if(isdigit(str[i + 1]))
    {
      hr = (uint8_t)((str[i] - '0')*10 +  (str[i + 1] - '0'));
      
      if(isdigit(str[i + 3]) && isdigit(str[i + 4]))
      {
        mt = (str[i + 3] - '0')*10 +  (str[i + 4] - '0');
      }
      else
      {
        return 0;
      }
    }
    else if (str[i + 1] == ':')
    {
      hr = (uint8_t)(str[i] - '0');
      
      if(isdigit(str[i + 2]) && isdigit(str[i + 3]))
      {
        mt = (str[i + 2] - '0')*10 +  (str[i + 3] - '0');
      }
      else
      {
        return 0;
      }
    }
    else
    {
      return 0;
    }  
  }
  else
  {
    return 0;
  }
  *hours = hr;
  *minutes = mt;
  return 1;
}

uint8_t parse_date(char *str, uint8_t *day, uint8_t *mounth, uint8_t *year)
{
  uint8_t i = 0;
  uint8_t date[3];
  uint8_t counter = 0;
  while((!isdigit(str[i])) && (str[i] != '\0')) i++;
  if(str[i] != '\n')
  {
    while((str[i] != '\0') && (counter < 3))
    {
      if(isdigit(str[i + 1]) && ((str[i + 2] == '.') || (str[i + 2] == ' ') || (str[i + 2] == '\0')))
      {
        date[counter] = (uint8_t)((str[i] - '0')*10 +  (str[i + 1] - '0'));
        i += 3;
        counter++;
      }
      else if(str[i + 1] == '.')
      {
        date[counter] = (uint8_t)(str[i] - '0');
        i += 2;
        counter++;
      }
      else
      {
        return 0;
      }
    }
  }
  else
  {
    return 0;
  }
  
  *day = date[0];
  *mounth = date[1];
  *year = date[2];
  return 1;
}
/*convert date, time and sensor data to string*/
void data_to_string(char *buffer, RTC_TimeTypeDef time_d, RTC_DateTypeDef data_d, uint16_t *vt_data)
{
  uint8_t start_ptr  = sprintf (buffer, "%d:%d:%d;", time_d.RTC_Hours, time_d.RTC_Minutes, time_d.RTC_Seconds);
  for(uint8_t i = 0; i < 31; i++)
  {
    uint8_t end_ptr = sprintf (&buffer[start_ptr], "%d;", vt_data[i]);
    start_ptr += end_ptr;
  }
  sprintf (&buffer[start_ptr], "%d\n", vt_data[31]);
}
// convert RTC_DateTypeDef to file name (8.3 standart FAT format)
void get_name(char *file_name, RTC_DateTypeDef data_d)
{
  char date[3], month[3], year[3];
  if((data_d.RTC_Date / 10) == 0)sprintf (date, "0%d",data_d.RTC_Date);
  else sprintf (date, "%d",data_d.RTC_Date);
  
  if((data_d.RTC_Month / 10) == 0)sprintf (month, "0%d",data_d.RTC_Month);
  else sprintf (month, "%d",data_d.RTC_Month);
  
  if((data_d.RTC_Year / 10) == 0)sprintf (year, "0%d",data_d.RTC_Year);
  else sprintf (year, "%d",data_d.RTC_Year);
  sprintf (file_name, "%s%s%s.csv",date,month,year);
}
//convert temperature, presure and date to eeprom format
void eeprom_to_string(char *buffer, DATETIME dt)
{
  RTC_TimeTypeDef time_d;
  RTC_DateTypeDef data_d;
  convertUnixTimeToDate(dt.timestamp, &data_d, &time_d); // make unix epoch timestamp to write on eeprom
  sprintf (buffer, "%d:%d:%d %d.%d.%d;%d,%d;%lu,%lu\n",
           time_d.RTC_Hours, time_d.RTC_Minutes, time_d.RTC_Seconds, data_d.RTC_Date, data_d.RTC_Month, data_d.RTC_Year, dt.temp/10, dt.temp%10,dt.pres/10, dt.pres%10);
}

