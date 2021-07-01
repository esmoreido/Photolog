#ifndef __EEPROM_H
#define __EEPROM_H
#include "RTC.h"

typedef enum 
{
  EEPROM_OK = 0x00,
  EEPROM_ERR,
  EEPROM_OUTOFMEM,
}EEPROMSTATUS;

EEPROMSTATUS EEPROM_struct_wr(DATETIME dt);
EEPROMSTATUS EEPROM_struct_rd(uint8_t block_adr, DATETIME *ret);
void EEEPROM_adr_init(void);
uint8_t EEPROM_EDC_rd(void);
void EEPROM_EDC_wr(uint8_t num_of_blocks);
void EEEPROM_block_erase(uint16_t start, uint16_t end);
#endif