#ifndef _FLASH_
#define _FLASH_

#define USER_FLASH_START_ADDRESS    0x08002C00

#if   defined(STM32F103x8)
#   define FLASH_END_ADDRESS        0x08010000
#elif defined(STM32F103xB)
#   define FLASH_END_ADDRESS        0x08020000
#else
#   error "Set MCU part number"
#endif

#define PAGE_SIZE                   1024

uint8_t isFlashReady(void);
void flashErasePage(uint32_t address);
void flashUnlock(void);
void flashLock();
void flashWrite(unsigned char* data, unsigned int address,
                          unsigned int count);

#endif //_FLASH_
