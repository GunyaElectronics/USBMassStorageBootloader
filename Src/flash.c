#include <stm32f10x.h>
#include <flash.h>

///////////////////////////////////////////////////////
#define FLASH_KEY1               ((uint32_t)0x45670123)
#define FLASH_KEY2               ((uint32_t)0xCDEF89AB)
///////////////////////////////////////////////////////

uint8_t flash_ready(void) {
 return !(FLASH->SR & FLASH_SR_BSY);
}

void flash_erase_page(uint32_t address) {

 flash_unlock();

    FLASH->CR|= FLASH_CR_PER;
    FLASH->AR = address;
    FLASH->CR|= FLASH_CR_STRT;
    while(!flash_ready())
     ;
    FLASH->CR&= ~FLASH_CR_PER;

    flash_lock();
}


void flash_unlock(void) {
   FLASH->KEYR = FLASH_KEY1;
   FLASH->KEYR = FLASH_KEY2;
}

void flash_lock() {
 FLASH->CR |= FLASH_CR_LOCK;
}

void Internal_Flash_Write(unsigned char* data, unsigned int address, unsigned int count) {
	unsigned int i;

	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR |= FLASH_CR_PG;

	for (i = 0; i < count; i += 2) {
		*(volatile unsigned short*)(address + i) = (((unsigned short)data[i + 1]) << 8) + data[i];
		while (!(FLASH->SR & FLASH_SR_EOP));
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR &= ~(FLASH_CR_PG);
}