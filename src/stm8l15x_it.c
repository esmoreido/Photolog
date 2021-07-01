/**
  ******************************************************************************
  * @file    Project/Template/stm8l15x_it.c
  * @author  MCD Tools Team
  * @version V1.2.0
  * @date    09/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "System_functions.h"

/** @addtogroup IT_Functions
  * @{
  */
INTERRUPT_HANDLER(NonHandledInterrupt,0)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);

}
/**
  * @brief TRAP interrupt routine
  * @par Parameters:
  * None
  * @retval 
  * None
*/
INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);

}
/**
  * @brief FLASH Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(FLASH_IRQHandler,1)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);

}
/**
  * @brief DMA1 channel0 and channel1 Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(DMA1_CHANNEL0_1_IRQHandler,2)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
// GPIO_ToggleBits(LED_GREEN_PORT,LED_GREEN_PIN);
  //i_count = 0;
  /*
  ADC_Cmd(ADC1, DISABLE);
  ADC_Cmd(ADC1, ENABLE);
  ADC_ChannelCmd(ADC1, ADC_Channel_15, ENABLE);
  ADC_ChannelCmd(ADC1, ADC_Channel_16, ENABLE);
  ADC_SoftwareStartConv(ADC1);
  DMA_ClearITPendingBit(DMA1_IT_TC0);
*/
}
/**
  * @brief DMA1 channel2 and channel3 Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(DMA1_CHANNEL2_3_IRQHandler,3)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);

}
/**
  * @brief RTC Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
  /*SD card writer counter*/
INTERRUPT_HANDLER(RTC_IRQHandler,4)
{
  static uint8_t minutes_prev = 0xFF;
  static uint8_t h = 0;
  static uint8_t m = 0;
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStr);
 // RTC_GetDate(RTC_Format_BIN, &RTC_DateStr);
  if(minutes_prev != RTC_TimeStr.RTC_Minutes)
  {
    if((RTC_TimeStr.RTC_Hours == next_time.RTC_Hours) &&
       (RTC_TimeStr.RTC_Minutes == next_time.RTC_Minutes))
    {
      if(iters_counter > 0)
      {
        h = set.step / 60;
        m = set.step % 60;
        if((next_time.RTC_Hours + h) > 23) 
        {
          next_time.RTC_Hours = (next_time.RTC_Hours + h) - 24;
        }
        else
        {
          next_time.RTC_Hours = next_time.RTC_Hours + h;
        }
        
        if((next_time.RTC_Minutes + m) > 59)
        {
          next_time.RTC_Hours++;
          next_time.RTC_Minutes = 0;
        }
        else
        {
          next_time.RTC_Minutes = next_time.RTC_Minutes + m;
        }
        
       // RTC_TimeStr.RTC_Hours = next_time.RTC_Hours;
       // RTC_TimeStr.RTC_Minutes = next_time.RTC_Minutes - 1;
        //RTC_SetTime(RTC_Format_BIN, &RTC_TimeStr);
        
     
        if(set.halt)
        {
          go_out_halt();
        }
        status =  WRITE_TIME;
        iters_counter--;
      }
      if(iters_counter == 0)
      {
        next_time = set.start_time;
        iters_counter = set.number_of_iter;

       // RTC_TimeStr.RTC_Hours = 3;
       // RTC_TimeStr.RTC_Minutes = 29;
       // RTC_SetTime(RTC_Format_BIN, &RTC_TimeStr);
        
       // if(RTC_DateStr.RTC_Date == 31)
       // {
       //   RTC_DateStr.RTC_Date = 1;
       // }
       //else
       //{
       //  RTC_DateStr.RTC_Date++;
       //}
        
       // RTC_SetDate(RTC_Format_BIN, &RTC_DateStr);
      }
      status =  WRITE_TIME;
    }
    minutes_prev = RTC_TimeStr.RTC_Minutes;
  }
  
  if(status ==  MAIN_LOOP)
  {
    if(set.halt)
    {
      status =  SLEEP_TIME;
    }
  }
  RTC_ClearITPendingBit(RTC_IT_WUT);
}
/**
  * @brief External IT PORTE/F and PVD Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(EXTIE_F_PVD_IRQHandler,5)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
 
}

/**
  * @brief External IT PORTB Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(EXTIB_IRQHandler,6)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  

}

/**
  * @brief External IT PORTD Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(EXTID_IRQHandler,7)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);

}

/**
  * @brief External IT PIN0 Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */

INTERRUPT_HANDLER(EXTI0_IRQHandler,8)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  //disableInterrupts();
  
  //GPIO_LOW(LED_GREEN_PORT,LED_GREEN_PIN);	
 // enableInterrupts();
  // GPIO_ToggleBits(LED_GREEN_PORT, LED_GREEN_PIN);
//EXTI_ClearITPendingBit(EXTI_IT_Pin0);

}
/**
  * @brief External IT PIN1 Interrupt routine.
  *   On User button pressed:
  *   Check if button presse a long time (4-5 sec.) if yes --> Set Autotest
  *   Else update status_machine to pass to next measuremetn.
  *   Update the LCD bar graph
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(EXTI1_IRQHandler,9)
{

    while (1);
}

/**
  * @brief External IT PIN2 Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */

INTERRUPT_HANDLER(EXTI2_IRQHandler,10)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);

}

/**
  * @brief External IT PIN3 Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(EXTI3_IRQHandler,11)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);

}

/**
  * @brief External IT PIN4 Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(EXTI4_IRQHandler,12)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);

}

/**
  * @brief External IT PIN5 Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(EXTI5_IRQHandler,13)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);
  
}

/**
  * @brief External IT PIN6 Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(EXTI6_IRQHandler,14)
{
 // disableInterrupts();
  
  /* To clear External IT Pin6 */
 // EXTI_ClearITPendingBit(EXTI_IT_Pin6);
  
 // enableInterrupts();

}

/**
  * @brief External IT PIN7 Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(EXTI7_IRQHandler,15)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  
  while (1);

}
/**
  * @brief LCD start of new frame Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(LCD_IRQHandler,16)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);

}
/**
  * @brief CLK switch/CSS/TIM1 break Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(SWITCH_CSS_BREAK_DAC_IRQHandler,17)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);

}

/**
  * @brief ADC1/Comparator Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(ADC1_COMP_IRQHandler,18)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);

}

/**
  * @brief TIM2 Update/Overflow/Trigger/Break Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_IRQHandler,19)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);

}

/**
  * @brief Timer2 Capture/Compare Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(TIM2_CAP_IRQHandler,20)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);

}


/**
  * @brief Timer3 Update/Overflow/Trigger/Break Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(TIM3_UPD_OVF_TRG_BRK_IRQHandler,21)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);

}
/**
  * @brief Timer3 Capture/Compare Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(TIM3_CAP_IRQHandler,22)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);
}
/**
  * @brief TIM1 Update/Overflow/Trigger/Commutation Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_COM_IRQHandler,23)
{
  
}
/**
  * @brief TIM1 Capture/Compare Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(TIM1_CAP_IRQHandler,24)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/


}

/**
  * @brief TIM4 Update/Overflow/Trigger Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
/**/
INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler,25)
{
  static uint8_t counter = 100;
  counter--;
  if(counter < 1) 
  {
    disk_timerproc();
    counter = 100;
  }
  TIM4_ClearITPendingBit(TIM4_IT_Update);
  
}
/**
  * @brief SPI1 Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(SPI1_IRQHandler,26)
{

 
}

/**
  * @brief USART1 TX Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(USART1_TX_IRQHandler,27)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);

}

/**
  * @brief USART1 RX Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(USART1_RX_IRQHandler,28)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);

}

/**
  * @brief I2C1 Interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
INTERRUPT_HANDLER(I2C1_IRQHandler,29)
{
/* In order to detect unexpected events during development,
   it is recommended to set a breakpoint on the following instruction.
*/
  while (1);
}

/**
  * @}
  */
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/

