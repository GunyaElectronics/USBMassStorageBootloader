#include <stm32f10x.h>
#include <flash.h>

#define FLASH_UNLOCK_KEY1               ((uint32_t)0x45670123)
#define FLASH_UNLOCK_KEY2               ((uint32_t)0xCDEF89AB)

uint8_t isFlashReady(void) {
    return !(FLASH->SR & FLASH_SR_BSY);
}

void flashErasePage(uint32_t address)
{
    flashUnlock();

    FLASH->CR|= FLASH_CR_PER;
    FLASH->AR = address;
    FLASH->CR|= FLASH_CR_STRT;

    while(!isFlashReady()) {
    }

    FLASH->CR&= ~FLASH_CR_PER;

    flashLock();
}

void flashUnlock(void) {
    FLASH->KEYR = FLASH_UNLOCK_KEY1;
    FLASH->KEYR = FLASH_UNLOCK_KEY2;
}

void flashLock() {
    FLASH->CR |= FLASH_CR_LOCK;
}

void flashWrite(uint8_t *pData, uint32_t address, uint32_t count) {
    uint32_t i;

    while (FLASH->SR & FLASH_SR_BSY) {
    }

    if (FLASH->SR & FLASH_SR_EOP) {
        FLASH->SR = FLASH_SR_EOP;
    }

    FLASH->CR |= FLASH_CR_PG;

    for (i = 0; i < count; i += 2) {
        *(volatile unsigned short*)(address + i) = (((unsigned short)pData[i + 1]) << 8) + pData[i];
        while (!(FLASH->SR & FLASH_SR_EOP)) {
        }
        FLASH->SR = FLASH_SR_EOP;
    }

    FLASH->CR &= ~(FLASH_CR_PG);
}