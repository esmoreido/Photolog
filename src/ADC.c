#include "ADC.h"
#include "System_functions.h"
uint32_t vt_collection[6] = {0x20000000, 0x4000000, 0x40000000, 0x8000000, 0x80000000, 0x10000000};
void ADC_int(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE); 
  GPIO_Init(GPIOC, GPIO_Pin_2, GPIO_Mode_In_FL_No_IT); // ADC input pin
  ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_12Bit, ADC_Prescaler_1); // single mode, 12bit(0-4095), 8MHz
  ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels, ADC_SamplingTime_384Cycles); // 384 cycles for more accuracy
  ADC_VrefintCmd (DISABLE); // without Vref
  ADC_Cmd(ADC1, ENABLE);
  ADC_ChannelCmd(ADC1, ADC_Channel_6, ENABLE); // pinup on C2 pin
}
POWERSTATE ADC_read_pw(uint16_t up, uint16_t down, uint8_t conv)
{
  uint32_t adc_buf = 0;
  POWERSTATE pw = POWER_OK;
  DEVIDER_ON; // turn on adc devider
  delay_ms_sp(100); // whait mosfet gate charge
  for(uint8_t i = 0; i < conv; i++)
  {
    ADC_SoftwareStartConv(ADC1); // make some adc magic
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);
    adc_buf += ADC_GetConversionValue(ADC1);
    delay_ms_sp(100); 
  }
  
   DEVIDER_OFF; // turn off adc devider
   adc_buf = adc_buf/(uint32_t)conv;

      if(adc_buf > up) pw = POWER_OVER;
      if((adc_buf < up) && (adc_buf > down) ) pw = POWER_OK;
      if(adc_buf < down) pw = POWER_LOW;
      
  return pw;
}
uint16_t ADC_read_raw(void) // raw adc for debug
{
  DEVIDER_ON;             // turn on adc devider
  delay_ms_sp(100);       // whait mosfet gate charge
  ADC_SoftwareStartConv(ADC1); // make some adc magic
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);
  DEVIDER_OFF;            // turn off adc devider
  
  return ADC_GetConversionValue(ADC1);;
  
}

uint16_t read_adc(ADC_Channel_TypeDef channel)
{
  /* Enable ADC1 clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
  /* Initialize and configure ADC1 */
  ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_10Bit, ADC_Prescaler_1);
  /* ADC channel used for IDD measurement */
  ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels, ADC_SamplingTime_384Cycles);
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  ADC_VrefintCmd (DISABLE);
  /* Enable ADC1 Channel used for IDD measurement */
  ADC_ChannelCmd(ADC1, channel, ENABLE);
  /* Start ADC1 Conversion using Software trigger*/
  ADC_SoftwareStartConv(ADC1);
  /* Wait until End-Of-Convertion */
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0);
  /* Get conversion value */
  uint16_t res = ADC_GetConversionValue(ADC1);
  ADC_ChannelCmd(ADC1, channel, DISABLE);
  ADC_Cmd(ADC1, DISABLE);
  ADC_DeInit(ADC1);
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, DISABLE);
  return res;
}
 
uint8_t ReadPhotoLine(uint16_t *data)
{

  uint32_t ptr = 1;
  uint8_t i = 0;
  SRCLR_LOW;
  delay_10us_sp(10);
  SRCLR_HIGH;
  while(!(ptr & 0x4000000))
  {
    SoftSPI_send32(~ptr);
    delay_10us_sp(120);
    data[i] = read_adc(ADC_CHANNEL);
    ptr = ptr << 1;
    i++;
  }

  for(uint8_t j = 0; j < 6; j++)
  {
    SoftSPI_send32(~vt_collection[j]);
    delay_10us_sp(120);
    data[i] = read_adc(ADC_CHANNEL);
    i++;
  }

  return 0;  
}
