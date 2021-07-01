#include "EEPROM.h"
#pragma location=0x1000 
__no_init uint8_t num_of_blocks_0;
#pragma location=0x1001
__no_init uint8_t num_of_blocks_1;
#pragma location=0x1002 
__no_init uint8_t num_of_blocks_2;

void EEEPROM_block_erase(uint16_t start, uint16_t end)
{
 FLASH_Unlock(FLASH_MemType_Data);
 for(uint16_t i = start; i <= end; i++) FLASH_ProgramByte(i, 0x00);
 FLASH_Lock(FLASH_MemType_Data);
}

//Error detection and correction simple algoritm realisation
uint8_t EEPROM_EDC_rd(void)
{
  if(num_of_blocks_0 == num_of_blocks_1) return  num_of_blocks_0;
  if(num_of_blocks_0 == num_of_blocks_2) return num_of_blocks_2;
  if(num_of_blocks_1 == num_of_blocks_2) return num_of_blocks_1;
  return 0xFF;
}

void EEPROM_EDC_wr(uint8_t num_of_blocks)
{
  disableInterrupts();
  FLASH_Unlock(FLASH_MemType_Data);
  num_of_blocks_0 = num_of_blocks;
  num_of_blocks_1 = num_of_blocks;
  num_of_blocks_2 = num_of_blocks;
  FLASH_Lock(FLASH_MemType_Data);
  enableInterrupts();
}

EEPROMSTATUS EEPROM_struct_wr(DATETIME dt)
{
  uint8_t  num = EEPROM_EDC_rd();
  if(num == 0xFF)
  {
    EEPROM_EDC_wr(0x00); // reset blocks_counter if mem was dameged
    num = 0x00;
  }
  uint16_t adr = (num*10) + 0x1018;
  
  if(num < 100)
  {
    disableInterrupts();
    FLASH_Unlock(FLASH_MemType_Data);

      FLASH_ProgramByte(adr, (uint8_t)(dt.timestamp>>24));
      adr+= 1;
      FLASH_ProgramByte(adr, (uint8_t)(dt.timestamp>>16));
      adr+= 1;
      FLASH_ProgramByte(adr, (uint8_t)(dt.timestamp>>8));
      adr+= 1;
      FLASH_ProgramByte(adr, (uint8_t)(dt.timestamp));
      adr+= 1;
  
      FLASH_ProgramByte(adr, (uint8_t)(dt.pres>>24));
      adr+= 1;
      FLASH_ProgramByte(adr, (uint8_t)(dt.pres>>16));
       adr+= 1;
      FLASH_ProgramByte(adr, (uint8_t)(dt.pres>>8));
      adr+= 1;
      FLASH_ProgramByte(adr, (uint8_t)(dt.pres));
      adr+= 1;
  
      FLASH_ProgramByte(adr, (uint8_t)(dt.temp>>8));
      adr+= 1;
      FLASH_ProgramByte(adr, (uint8_t)dt.temp);
      num += 1;
      EEPROM_EDC_wr(num);
      
    FLASH_Lock(FLASH_MemType_Data);
    enableInterrupts();
    return EEPROM_OK;
  }
  else return EEPROM_OUTOFMEM;
}

EEPROMSTATUS EEPROM_struct_rd(uint8_t block_adr, DATETIME *ret)
{
  uint8_t  num = EEPROM_EDC_rd();
  if(num == 0xFF)
  {
    EEPROM_EDC_wr(0x00); // reset blocks_counter if mem was dameged
    num = 0x00;
  }
  if((num > 0)&&(block_adr < num))
  {
      ret->timestamp = 0; ret->pres = 0; ret->temp = 0;
      ret->timestamp = (uint32_t)FLASH_ReadByte(((block_adr*10) + 0x1018)) << 24;         
      ret->timestamp |= (uint32_t)FLASH_ReadByte( ((block_adr*10) + 0x1018)+1) << 16;                
      ret->timestamp |= (uint32_t)FLASH_ReadByte( ((block_adr*10) + 0x1018)+2) << 8;
      ret->timestamp |= (uint32_t)FLASH_ReadByte( ((block_adr*10) + 0x1018)+3);
      
      ret->pres = (uint32_t)FLASH_ReadByte( ((block_adr*10) + 0x1018)+4) << 24;         
      ret->pres |= (uint32_t)FLASH_ReadByte( ((block_adr*10) + 0x1018)+5) << 16;                
      ret->pres |= (uint32_t)FLASH_ReadByte( ((block_adr*10) + 0x1018)+6) << 8;
      ret->pres |= (uint32_t)FLASH_ReadByte( ((block_adr*10) + 0x1018)+7);
      
      ret->temp =  ((uint16_t)FLASH_ReadByte( ((block_adr*10) + 0x1018)+8)<<8) | (uint16_t)FLASH_ReadByte( ((block_adr*10) + 0x1018)+9);
      return EEPROM_OK;
  }   
  else return EEPROM_ERR;
        
}
