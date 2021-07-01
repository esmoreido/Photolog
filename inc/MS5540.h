#ifndef __MS5540_H
#define __MS5540_H
#include "stm8l15x.h"
#include "stm8l15x_conf.h"

  /*MS5540 data parser varibles*/
  long c1,c2,c3,c4,c5,c6;
  unsigned int result1 ;
  unsigned int result2 ;
  unsigned int result3 ;
  unsigned int result4 ;
  float D1, D2;
  uint8_t inbyte1 ;
  uint8_t inbyte2 ; 
  uint8_t inbyte3 ; 
  uint8_t inbyte4 ; 
  unsigned int tempMSB ; //first byte of value
  unsigned int tempLSB ; //last byte of value
  unsigned int presMSB ; //first byte of value
  unsigned int presLSB ; //last byte of value
  
  void resetsensor_soft();
  void MS5540_init(void);               // MS5540 initialization
  int16_t MS5540_temp(void);           // MS5540 read temperature      
  uint32_t MS5540_pres(void);          // MS5540 read preasure    

#endif