#ifndef _FLASH_
#define _FLASH_

#define USER_FLASH_START_ADDRESS    0x08002C00
#define FLASH_END_ADDRESS           0x08010000
#define PAGE_SIZE                   1024

uint8_t flash_ready(void);
void flash_erase_all_pages(void);
void flash_erase_page(uint32_t address);
void flash_unlock(void);
void flash_lock();
void flash_write(uint32_t address,uint32_t data);
uint32_t flash_read(uint32_t address);
int  writeSector(uint32_t Address,void * values, uint16_t size);
void Internal_Flash_Write(unsigned char* data, unsigned int address,
                          unsigned int count);

#endif
