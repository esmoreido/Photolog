
#include "inc/System_functions.h"
void resetsensor()
{
  CS_LOW;
  SoftSPI_send(SENSOR_CMD_RESET);
  CS_HIGH;
}

uint16_t coefficient_read(uint8_t index)
{
  uint16_t result = 0x00;
  CS_LOW;
  SoftSPI_send(0xA0 + (index * 2));
  result = SoftSPI_recive();
  result = result << 8;
  result |= SoftSPI_recive(); 
  SoftSPI_recive(); 
  CS_HIGH;
  return result;
}

uint32_t d1_read(void)
{
  
  uint8_t returnedData = 0x00;
  uint32_t result = 0x00;
  CS_LOW;
  SoftSPI_send(0x48);
  CS_HIGH;
  delay_ms_sp(100);
  CS_LOW;
  
  SoftSPI_send(SENSOR_CMD_ADC_READ);
  
  returnedData = SoftSPI_recive();
  result = 65536 * returnedData;
  returnedData = SoftSPI_recive();
  result = result + 256 * returnedData;
  returnedData = SoftSPI_recive();
  result = result + returnedData;
  CS_HIGH;
  return result;
}
uint32_t d2_read(void)
{
  CS_LOW;
  uint8_t returnedData = 0x00;
  uint32_t result = 0x00;
  SoftSPI_send(0x58);
  CS_HIGH;
  delay_ms_sp(100);
  CS_LOW;
  SoftSPI_send(SENSOR_CMD_ADC_READ);
  
  returnedData = SoftSPI_recive();
  result = 65536 * returnedData;
  returnedData = SoftSPI_recive();
  result = result + 256 * returnedData;
  returnedData = SoftSPI_recive();
  result = result + returnedData;
  CS_HIGH;
  return result;
}

void MS5803_init(void)
{
  s = MS5803;
  resetsensor();
  
      for (uint8_t i = 0; i < 8; i++ ){
        sensorCoefficients[ i ] = coefficient_read( i );  // read coefficients
        delay_ms_sp(100);
    }
}

void MS5803_read(void)
{
 uint32_t D1 = d1_read();
 uint32_t D2 = d2_read();
  
  deltaTemp = D2 - ((float)sensorCoefficients[5]) * 256;
  sensorOffset = ((float)sensorCoefficients[2]) * 65536 + ( deltaTemp * ((float)sensorCoefficients[4]) ) / 128;
  sensitivity = ((float)sensorCoefficients[1]) * 32768 + ( deltaTemp * (float)sensorCoefficients[3] ) / 256;
    // calculate 2nd order pressure and temperature (MS5803 2st order algorithm)
  temp = ( 2000 + (deltaTemp * (float)sensorCoefficients[6] ) / 8388608 ) / 100; 
  press = ( ( ( ( (float)D1 * (float)sensitivity ) / 2097152 - sensorOffset) / 32768 ) / 10 );
}