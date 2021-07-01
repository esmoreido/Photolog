#include "MS5540.h"
#include "System_functions.h"
void resetsensor_soft()
{
  SoftSPI_send(0x15);
  SoftSPI_send(0x55);
  SoftSPI_send(0x40);
}
void MS5540_init(void)
{
  s = MS5540;
  resetsensor_soft();
  SoftSPI_send(0x1D);
  SoftSPI_send(0x50);

  result1 = SoftSPI_recive();
  result1 = result1 << 8; //shift returned byte 
  inbyte1 = SoftSPI_recive(); //send dummy byte to read second byte of word
  result1 = result1 | inbyte1; //combine first and second byte of word
  
 //Calibration word 2
  resetsensor_soft();
  SoftSPI_send(0x1D);
  SoftSPI_send(0x60);

  result2 = SoftSPI_recive();
  result2 = result2 << 8; //shift returned byte 
  inbyte2 = SoftSPI_recive(); //send dummy byte to read second byte of word
  result2 = result2 | inbyte2; //combine first and second byte of word
  
  //Calibration word 3
  resetsensor_soft();
  SoftSPI_send(0x1D);
  SoftSPI_send(0x90);

  result3 = SoftSPI_recive();
  result3 = result3 << 8; //shift returned byte 
  inbyte3 = SoftSPI_recive(); //send dummy byte to read second byte of word
  result3 = result3 | inbyte3; //combine first and second byte of word
  
  //Calibration word 3
  resetsensor_soft();
  SoftSPI_send(0x1D);
  SoftSPI_send(0xA0);

  result4 = SoftSPI_recive();
  result4 = result4 << 8; //shift returned byte 
  inbyte4 = SoftSPI_recive(); //send dummy byte to read second byte of word
  result4 = result4 | inbyte4; //combine first and second byte of word
   
  c1 = (result1 >> 1);
  c2 = ((result3 & 0x3F) << 6) | ((result4 & 0x3F));
  c3 = result4 >> 6;
  c4 = result3 >> 6;
  c5 = ((result1 & 1) << 10) | (result2 >> 6);
  c6 = result2 & 0x003F;
}


int16_t MS5540_temp()
{
  resetsensor_soft();
  SoftSPI_send(0x0F);
  SoftSPI_send(0x20);
  delay_ms_sp(35);

  tempMSB = SoftSPI_recive(); //send dummy byte to read second byte of word
  tempMSB = tempMSB << 8; //combine first and second byte of word
  tempLSB = SoftSPI_recive(); //send dummy byte to read second byte of word
  D2 = tempMSB | tempLSB; //combine first and second byte of word
    
  float UT1 = 8*(float)c5 + 20224; // take this calcs from datasheet
  float dT = D2 - UT1;
  int16_t TEMP = (int16_t)(200 + ((dT * (c6 + 50)/1024)));

  return TEMP;
}

uint32_t MS5540_pres(void)
{
  resetsensor_soft();
  SoftSPI_send(0x0F);
  SoftSPI_send(0x20);
  delay_ms_sp(35);

  tempMSB = SoftSPI_recive(); //send dummy byte to read second byte of word
  tempMSB = tempMSB << 8; //combine first and second byte of word
  tempLSB = SoftSPI_recive(); //send dummy byte to read second byte of word
  D2 = tempMSB | tempLSB; //combine first and second byte of word
    
  float UT1 = 8*(float)c5 + 20224;
  float dT = D2 - UT1;
  
  resetsensor_soft();
  SoftSPI_send(0x0F);
  SoftSPI_send(0x40);
  delay_ms_sp(35);

  presMSB  = SoftSPI_recive(); //send dummy byte to read second byte of word
  presMSB  = presMSB  << 8; //combine first and second byte of word
  presLSB  = SoftSPI_recive(); //send dummy byte to read second byte of word
  D1 = presMSB | presLSB ; //combine first and second byte of word
    
  float OFF = ((float)c2)*4 + ((((float)c4)-512)*dT)/4096; // take this calcs from datasheet
  float SENS = c1 + (((float)c3)*dT)/1024 + 24576;
  float X = (SENS * (D1 - 7168))/16384 - OFF;
  uint32_t P = (uint32_t)((X*10)/32 + 2500);
  
  return P;
}