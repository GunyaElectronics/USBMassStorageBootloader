#include "main.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "crc.h"
#include "flash.h"

//---------------- static function declaration -------------------

static void    clockConfig(void);
static void    loadApplication(void);
static uint8_t isSoftReset(void);
static uint8_t isApplicationValid(void);
static void    startUsbMassStorageBootloader(void);

//------------------------ main code -----------------------------

int main(void)
{
    clockConfig();

    if (!isSoftReset() && isApplicationValid()) {
        loadApplication();
    }

    startUsbMassStorageBootloader();

    while (1) {
        // All bootloader code working in interrupt mode
    }
}

//---------------- static function implementation ----------------

static void clockConfig(void)
{
    RCC->CR   |=  RCC_CR_HSEON;
    while ((RCC->CR & RCC_CR_HSERDY)==0) {
    }
    RCC->CR   |=  RCC_CR_CSSON;

    RCC->CFGR &= ~RCC_CFGR_PLLXTPRE;

    FLASH->ACR |=  FLASH_ACR_PRFTBE;
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |=  FLASH_ACR_LATENCY_2;

    RCC->CFGR  |= RCC_CFGR_PLLSRC;
    RCC->CR   &= ~RCC_CR_PLLON;
    RCC->CFGR &= ~RCC_CFGR_PLLMULL;
    RCC->CFGR |=  RCC_CFGR_PLLMULL9;
    RCC->CR   |=  RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0) {
    }

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |=  RCC_CFGR_SW_PLL;
    while ((RCC->CFGR&RCC_CFGR_SWS)!= 0x08) {
    }

    RCC->CFGR &= ~RCC_CFGR_PPRE1;
    RCC->CFGR |=  RCC_CFGR_PPRE1_DIV2;

    RCC->CFGR &= ~RCC_CFGR_ADCPRE;
    RCC->CFGR |=  RCC_CFGR_ADCPRE_DIV6;
}

static uint8_t isSoftReset(void)
{
    return (RCC->CSR & RCC_CSR_SFTRSTF) != 0;
}

static uint8_t isApplicationValid(void)
{
    const uint32_t *pAppCrc = (const uint32_t *)APP_CRC_ADDRESS;

    uint32_t crc = getCrc32(0,(uint8_t *) MAIN_PROGRAM_START_ADDRESS,
                            FLASH_END_ADDRESS - MAIN_PROGRAM_START_ADDRESS - sizeof(*pAppCrc));

    return crc == *pAppCrc;
}

static void startUsbMassStorageBootloader(void)
{
    CLEAR_SOFT_RESET_FLAG();

    MAL_Config();

    RCC->APB1ENR |= RCC_APB1Periph_USB;

    SCB->AIRCR = AIRCR_VECTKEY_MASK | NVIC_PriorityGroup_1;

    NVIC->IP[0x14] = 0x90;
    NVIC->ISER[0x14 >> 0x05] = (uint32_t)0x01 << (0x14 & (uint8_t)0x1F);
    NVIC->IP[0x13] = 0x80;
    NVIC->ISER[0x13 >> 0x05] = (uint32_t)0x01 << (0x13 & (uint8_t)0x1F);

    __enable_irq();

    USB_Init();
}

static void loadApplication(void)
{
#define MSP_SIZE (sizeof(uint32_t))
    typedef void (*pFunction_t)(void);

    static pFunction_t gApplicationFunction = NULL;

    __disable_irq();

    // This block of code sets the vector table location and offset.
    // Here we specify if the vector table is in RAM or FLASH memory. 
    // This parameter can be 0x08000000
    // kJumpAddress is vector Table base offset field. (MAIN_PROGRAM_START_ADDRESS) 
    // This value must be a multiple of 0x100.
    SCB->VTOR = (((uint32_t)0x08000000) | (MAIN_PROGRAM_START_ADDRESS & (uint32_t)0x1FFFFF80));

    const uint32_t kJumpAddress = *(__IO uint32_t*)(MAIN_PROGRAM_START_ADDRESS + MSP_SIZE);

    gApplicationFunction = (pFunction_t)kJumpAddress;

    // Set main stack pointer register
    __set_MSP(*(__IO uint32_t*)MAIN_PROGRAM_START_ADDRESS);

    // Jump to main application
    // NOTE: In Application you nead enable global interrupt
    gApplicationFunction();
#undef MSP_SIZE
}