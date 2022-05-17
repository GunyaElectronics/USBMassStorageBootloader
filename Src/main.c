#include "main.h"
#include <crc.h>
#include "flash.h"

#define MAIN_PROGRAM_START_ADDRESS ((uint32_t)USER_FLASH_START_ADDRESS)
#define APP_CRC_ADDRESS (FLASH_END_ADDRESS - 4)

typedef void (*p_function_t)(void);
void USB_Init(void);
void MAL_Config(void);

uint32_t g_jump_address;                /**< jump address application*/
p_function_t g_app_func;                /**< application function*/

void load_app()
{       
    __disable_irq();

    /*
    * This block of code sets the vector table location and offset.
    * Here we specify if the vector table is in RAM or FLASH memory. 
    * This parameter can be 0x08000000
    * app_image_addr is vector Table base offset field. (MAIN_PROGRAM_START_ADDRESS) 
    * This value must be a multiple of 0x100.
    */
    SCB->VTOR = (((uint32_t)0x08000000) | (MAIN_PROGRAM_START_ADDRESS & (uint32_t)0x1FFFFF80));
    
    g_jump_address = *(__IO uint32_t*)(MAIN_PROGRAM_START_ADDRESS + 4);
    
    g_app_func = (p_function_t)g_jump_address;
    
    __set_MSP(*(__IO uint32_t*)MAIN_PROGRAM_START_ADDRESS);
    
    /* In Application you nead enable global interrupt */
    g_app_func();
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  RCC->CR   |=  RCC_CR_HSEON;
  while((RCC->CR & RCC_CR_HSERDY)==0) {}
  RCC->CR   |=  RCC_CR_CSSON;

  RCC->CFGR &= ~RCC_CFGR_PLLXTPRE;

  FLASH->ACR|=  FLASH_ACR_PRFTBE;
  FLASH->ACR&= ~FLASH_ACR_LATENCY;
  FLASH->ACR |= FLASH_ACR_LATENCY_2;

  RCC->CFGR  |= RCC_CFGR_PLLSRC;
  RCC->CR   &= ~RCC_CR_PLLON;
  RCC->CFGR &= ~RCC_CFGR_PLLMULL;
  RCC->CFGR |=  RCC_CFGR_PLLMULL9;
  RCC->CR   |=  RCC_CR_PLLON;
  while((RCC->CR & RCC_CR_PLLRDY) == 0) {}

  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |=  RCC_CFGR_SW_PLL;
  while((RCC->CFGR&RCC_CFGR_SWS)!= 0x08) {}

  RCC->CFGR &= ~RCC_CFGR_PPRE1;          
  RCC->CFGR |=  RCC_CFGR_PPRE1_DIV2;     


  RCC->CFGR &= ~RCC_CFGR_ADCPRE;             
  RCC->CFGR |=  RCC_CFGR_ADCPRE_DIV6; 
  
  __disable_irq();

  if ((RCC->CSR & RCC_CSR_SFTRSTF) == 0) {
      uint32_t crc = getCrc32(0,(uint8_t*) MAIN_PROGRAM_START_ADDRESS,
                             FLASH_END_ADDRESS - MAIN_PROGRAM_START_ADDRESS -4);
      
      uint32_t *app_crc = (uint32_t *)APP_CRC_ADDRESS;
      if(crc == *app_crc)
          load_app();
  }

  //clear flag        
  RCC->CSR |= RCC_CSR_RMVF;

  MAL_Config();
  
  RCC->APB1ENR |= RCC_APB1Periph_USB;
  
  SCB->AIRCR = AIRCR_VECTKEY_MASK | NVIC_PriorityGroup_1;

  NVIC->IP[0x14] = 0x90;
  NVIC->ISER[0x14 >> 0x05] = (uint32_t)0x01 << (0x14 & (uint8_t)0x1F);
  NVIC->IP[0x13] = 0x80;
  NVIC->ISER[0x13 >> 0x05] = (uint32_t)0x01 << (0x13 & (uint8_t)0x1F);

  __enable_irq();

  USB_Init();
  while(1);
}