
#include "ADC.h"
#include "System_functions.h"
#include "EEPROM.h"
BYTE time_buf[16];
DATETIME dt;
FRESULT rc;                                     			
DWORD fre_sect,fre_clust, tot_sect;
FIL fil;
FATFS fatfs;
FATFS* fs_ptr;
uint8_t err_num = 0;
uint8_t date_prev = 0;
uint8_t ethernal_halt_flag = 0;
UINT bytes_read = 0;
char file_name[12];
char output_char_data[255];
char setings_buf[40];
uint16_t photo_data[32];
char* setings_string[8] = {
  "HALT_MODE",
  "INDICATION_LEVEL",
  "START_TIME",
  "STEP",
  "NUMBER_OF_ITERATIONS",
  "ADC_TRESHOLD_UP",
  "ADC_TRESHOLD_DOWN",
  "ADC_CONVERTIONS"
};
uint16_t setings_uint [9];
char result_string [16];
char* return_string;
POWERSTATE pwr;
void die (FRESULT rc)
{
  while(!rc)
  {
    LED_ON;
    
    delay_ms_sp(500);
    LED_OFF;
    delay_ms_sp(500);
  }
} 
int main (void)
{
  enableInterrupts();
  GPIO_CLK_int(); 
  set.halt                      = HALT_MODE;
  set.indication_level          = INDICATION_LEVEL;
  set.start_time.RTC_Hours      = 7; 
  set.start_time.RTC_Minutes    = 30;
  set.step                      = STEP;
  set.number_of_iter            = NUMBER_OF_ITTER;
  set.adc_tresh_up              = ADC_TRESHOLD_UP;
  set.adc_tresh_down            = ADC_TRESHOLD_DOWN;
  set.adc_conv                  = ADC_CONVERTIONS;

  
  write_flag = 1;
  uint8_t h, m, d, mth, y, size;
  uint8_t set_hours, set_minutes;
  uint8_t i = 0;
  uint8_t j = 0;
  status = WHAIT_SD;
 // minutes_counter = set.sdcard_minutes;
  SPI_int();   
  //ADC_int();
  RTC_config();
  tim4_ini();
  PHOTO_PWR_ON;
  PER_ON;
  //SRCLR_HIGH;
  
  if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) != 0x00) status = WHAIT_SD; else status = SAVE_EEPROM; 
  
  while(1)
  {
    
    switch (status)
    {
    case WHAIT_SD:
      if( set.indication_level > 0)
      {
        LED_ON;
        delay_ms_sp(250);
        LED_OFF;
        delay_ms_sp(250);
      }
      if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) != 0x00) status = WHAIT_SD; else status = READ_SD; 
      //PHOTO_PWR_ON;
      //ReadPhotoLine(photo_data);
      break;
      
    case READ_SD:   
      if( set.indication_level > 0) LED_ON;
      delay_ms_sp(1000);
      disk_initialize(0); 
      rc = f_mount(&fatfs, "",0);
      if(rc){status = INDICATION_ERR; err_num = 1; LED_OFF; break;}
      rc = f_open(&fil, "time.txt", FA_READ);
      if(rc){status = INDICATION_ERR; err_num = 2; LED_OFF; break;}
      rc = f_read(&fil,time_buf,16,&bytes_read);
      if(rc){status = INDICATION_ERR; err_num = 3; LED_OFF; break;}
      rc = f_close(&fil);
      if(rc){status = INDICATION_ERR;  err_num = 4; LED_OFF; break;}
      fs_ptr = &fatfs;
      rc = f_getfree("", &fre_clust, &fs_ptr);
      tot_sect  = ((fatfs.n_fatent - 2) * fatfs.csize) / 2;
      fre_sect = (fre_clust * fatfs.csize) / 2;
      if(fre_sect < 1000){status = INDICATION_ERR;  err_num = 9; LED_OFF; break;}
      
      if( set.indication_level > 0)
      {
        LED_OFF;
        delay_ms_sp(1000);
      }
      status = READ_SETINGS;

      while((!isdigit(time_buf[i])) && (time_buf[i] != '\0')) i++;
      if(time_buf[i] != '\0')
      {
        while((time_buf[i + j] != ' ') && (time_buf[i + j] != '\0')) j++;
        if(time_buf[i] != '\0')
        {
          strncpy(result_string, time_buf + i, j); 
        }
      }
      if(parse_time(result_string, &h, &m) && parse_date(time_buf + i + j, &d, &mth, &y))
      {
        if((h < 24) ) RTC_TimeStr.RTC_Hours = (time_buf[0]- '0')*10 +  (time_buf[1]- '0');
        if((m < 60) )RTC_TimeStr.RTC_Minutes =(time_buf[3]- '0')*10 +  (time_buf[4]- '0');
        if((d <= 31) && (d > 0)) RTC_DateStr.RTC_Date = (time_buf[7]- '0')*10 +  (time_buf[8]- '0');
        if((mth <= 12) && (mth > 0))RTC_DateStr.RTC_Month = (RTC_Month_TypeDef)((time_buf[10]- '0')*10 +  (time_buf[11]- '0'));
        if((y < 99) ) RTC_DateStr.RTC_Year = (time_buf[13]- '0')*10 +  (time_buf[14]- '0');
      }
      else
      {
        status = INDICATION_ERR; err_num = 7; LED_OFF break;
      }
      RTC_SetDate(RTC_Format_BIN, &RTC_DateStr);
      RTC_SetTime(RTC_Format_BIN, &RTC_TimeStr);
      
      //if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) == 0x00) status = WHAIT_SD;
      memset(result_string, 0, sizeof(result_string));
      break;
      
case READ_SETINGS:
      memset(setings_uint, 0xffff, 9);
      if( set.indication_level > 0) LED_ON;
      delay_ms_sp(1000);
      disk_initialize(0); 
      rc = f_mount(&fatfs, "",0);
      if(rc){status = INDICATION_ERR; err_num = 1; LED_OFF; break;}
      rc = f_open(&fil, "setings.txt", FA_READ);
      if(rc){status = INDICATION_ERR; err_num = 2; LED_OFF; break;}
      do 
      {
        f_gets(setings_buf, 40, &fil);
        for(uint8_t i = 0; i < 8; i++)
        {
          return_string = strstr(setings_buf,setings_string[i]);
          if(return_string != 0)
          {
            uint8_t j = 0;
            while((!isdigit(return_string[j])) && (return_string[j] != '\0'))j++;
            if(return_string[j] != '\0')
            {
              strcpy(result_string, return_string + j); 
              if(i == 2)                                                        // If it is a time_set topic 
              {
                if(parse_time(return_string, &set_hours, &set_minutes))
                {
                  setings_uint[2] = 0x0000;
                }
                else
                {
                  status = INDICATION_ERR; err_num = 7; LED_OFF break;
                }

              }
              else
              {
                setings_uint[i] = atoi(result_string);
              }
              
            }
          }
        }
        if(status == INDICATION_ERR) break;
        memset(setings_buf, 0, 40);
      } while (!f_eof(&fil));

      if(setings_uint[0] != 0xFFFF) 
      {
        if(setings_uint[0] < 2) 
        {
          set.halt = setings_uint[0];
        }
        else {status = INDICATION_ERR; err_num = 7; LED_OFF; break;}
      }
      if(setings_uint[1] != 0xFFFF) 
      {
         if(setings_uint[1] < 3) 
         {
           set.indication_level = setings_uint[1];
         }
         else {status = INDICATION_ERR; err_num = 7; LED_OFF; break;}
      }
      if(setings_uint[2] != 0xFFFF) 
      {
        if((set_hours < 24) && (set_minutes < 60))
        {
          set.start_time.RTC_Hours = set_hours; 
          set.start_time.RTC_Minutes = set_minutes; 
          next_time = set.start_time;
        }
        else {status = INDICATION_ERR; err_num = 7; LED_OFF; break;}
      }
      if(setings_uint[3] != 0xFFFF) 
      {
        if(setings_uint[3] < 1440)
        {
          set.step = setings_uint[3];
        }
        else {status = INDICATION_ERR; err_num = 7; LED_OFF; break;}
      }
      if(setings_uint[4] != 0xFFFF) {
        if(setings_uint[4] <= 10)
        {
          set.number_of_iter = setings_uint[4]; 
          iters_counter = set.number_of_iter;
        }
        else {status = INDICATION_ERR; err_num = 7; LED_OFF; break;}
      }
      if(setings_uint[5] != 0xFFFF) 
      {
        if(setings_uint[5] < 3000)
        {
          set.adc_tresh_up = setings_uint[5];
        }
        else {status = INDICATION_ERR; err_num = 7; LED_OFF; break;}
      }
      if(setings_uint[6] != 0xFFFF) 
      {
        if(setings_uint[6] > 100)
        {
          set.adc_tresh_down = setings_uint[6];
        }
        else {status = INDICATION_ERR; err_num = 7; LED_OFF; break;}
      }
      if(setings_uint[7] != 0xFFFF) 
      {
        if(setings_uint[7] <= 10)
        {
          set.adc_conv = setings_uint[7];
        }
        else {status = INDICATION_ERR; err_num = 7; LED_OFF; break;}
      }
      rc = f_close(&fil);
      if(rc){status = INDICATION_ERR;  err_num = 4; LED_OFF; break;}
      if( set.indication_level > 0)
      {
        LED_OFF;
        delay_ms_sp(1000);
      }
      status = MAIN_LOOP; 
      RTC_WakeUpCmd(ENABLE);
      //RTC_WakeUpCmd(DISABLE);
      //RTC_WakeUpCmd(ENABLE);
      get_name(file_name,RTC_DateStr);
      date_prev =  RTC_DateStr.RTC_Date;
      break;
      
    case SAVE_EEPROM:
      size = EEPROM_EDC_rd();
      if((size > 0x00) && (size != 0xFF))
      {
        DATETIME rd;
        disk_initialize(0); 
        rc = f_mount(&fatfs, "",0); 
        if(rc){status = INDICATION_ERR; err_num = 1; LED_OFF; break;}
        rc = f_open(&fil, "backup.csv", FA_OPEN_ALWAYS | FA_WRITE);
        if(rc){status = INDICATION_ERR; err_num = 2; LED_OFF; break;}
        rc = f_lseek (&fil, f_size(&fil));
        if(rc){status = INDICATION_ERR; err_num = 5; LED_OFF; break;}
        
        for(uint8_t i = 0; i < size; i++)
        {
          EEPROM_struct_rd(i,&rd);
          eeprom_to_string(output_char_data, rd);
          f_printf(&fil, output_char_data);
        }
        
        rc =  f_close(&fil);
        EEPROM_EDC_wr(0x00);
        if(rc){status = INDICATION_ERR; err_num = 4; LED_OFF; break;}
      }
      
      get_name(file_name,RTC_DateStr);
      date_prev =  RTC_DateStr.RTC_Date;
      status = MAIN_LOOP;
      //if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) == 0x00) status = WHAIT_SD;
      break;
      
    case MAIN_LOOP: 
        PHOTO_PWR_ON;
        PER_ON;
        SRCLR_HIGH;
        ReadPhotoLine(photo_data);
       // RTC_GetTime(RTC_Format_BIN, &RTC_TimeStr);
       // RTC_GetDate(RTC_Format_BIN, &RTC_DateStr);
        dt.timestamp =  convertDateToUnixTime(RTC_DateStr,RTC_TimeStr);
        //if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) == 0x00) status = WHAIT_SD;    
        delay_ms_sp(1000);
      break;
      
    case WRITE_TIME:
      
      PHOTO_PWR_ON;
      PER_ON;
      SRCLR_HIGH;
      RTC_GetTime(RTC_Format_BIN, &RTC_TimeStr);
      RTC_GetDate(RTC_Format_BIN, &RTC_DateStr);
      ReadPhotoLine(photo_data);
      dt.timestamp =  convertDateToUnixTime(RTC_DateStr,RTC_TimeStr);
      
      if(date_prev != RTC_DateStr.RTC_Date)
      {
        get_name(file_name,RTC_DateStr);
        date_prev = RTC_DateStr.RTC_Date;
      }
      
      disk_initialize(0); 
      rc = f_mount(&fatfs, "",0); 
      rc = f_open(&fil, file_name, FA_OPEN_ALWAYS | FA_WRITE);
      f_lseek (&fil, f_size(&fil));
      data_to_string(output_char_data, RTC_TimeStr, RTC_DateStr,  photo_data);
      f_printf(&fil, output_char_data);
      rc =  f_close(&fil);
  //    minutes_counter = set.sdcard_minutes;
      if(rc)
      {
        if( set.indication_level > 1)
        {
          LED_ON;
          delay_ms_sp(1000);
          LED_OFF;  
        }
        status = INDICATION_ERR;
      }
      else
      {
        if( set.indication_level > 1)
        {   
          LED_ON;
          delay_ms_sp(5000);
          LED_OFF;
        }
        //RTC_WakeUpCmd(DISABLE);
        //RTC_WakeUpCmd(ENABLE);
        status = MAIN_LOOP;
        if(set.halt)
        {
          go_to_halt(0);
        }
      }
      break;
    
    case SLEEP_TIME:
      go_to_halt(0);
      break;
      
    case INDICATION_ERR:
      while(err_num)
      {
        if(set.indication_level > 0)
        {
          LED_ON;
          delay_ms_sp(300);
          LED_OFF;
          delay_ms_sp(300);
          err_num--;
        }
      }
      status = WHAIT_SD;
      break;
    }
  }
  
}




