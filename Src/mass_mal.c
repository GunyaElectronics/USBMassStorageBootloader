/**
  ******************************************************************************
  * @file    mass_mal.c
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    29-June-2012
  * @brief   Medium Access Layer interface
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "mass_mal.h"
#include "emfat.h"
#include "flash.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t Mass_Memory_Size[2];
uint32_t Mass_Block_Size[2];
uint32_t Mass_Block_Count[2];
__IO uint32_t Status = 0;

emfat_t emfat;

uint8_t inputfile[128];

void input_read_proc(uint8_t *dest, int size, uint32_t offset, size_t userdata)
{
    uint32_t address = USER_FLASH_START_ADDRESS + offset;
	memcpy(dest, (uint8_t*)address, size);
}

void input_write_proc(const uint8_t *dest, int size, uint32_t offset, size_t userdata)
{ 
    static uint32_t address = USER_FLASH_START_ADDRESS;
    static int not_erased = 1;
    unsigned char* data = (unsigned char*)dest;
    
    if(not_erased)
    {
        uint32_t page_count = (FLASH_END_ADDRESS - USER_FLASH_START_ADDRESS) / PAGE_SIZE;
        uint32_t addr = USER_FLASH_START_ADDRESS;
        
        __disable_irq();
        
        for(int i = 0; i < page_count; i++)
        {
            flashErasePage(addr);
            addr += PAGE_SIZE;
        }
        
        not_erased = 0;
        
        __enable_irq();
    }
	
    __disable_irq();
    
    flashUnlock();
    
    flashWrite(data, address, size);
    
    address += size;
        
    flashLock();
    
    __enable_irq();
}

#define CMA_TIME EMFAT_ENCODE_CMA_TIME(5,6,2019, 0,0,0)
#define CMA { CMA_TIME, CMA_TIME, CMA_TIME }
#define FW_SIZE (FLASH_END_ADDRESS - USER_FLASH_START_ADDRESS)

emfat_entry_t entries[] =
{
	// name     dir    lvl offset size max_size user  time  read write
	{ "",       true,  0,  0,  0,      0,       0,    CMA,  NULL, NULL },
	{ "fw.bin", false, 1,  0,  FW_SIZE,FW_SIZE, 0,    CMA,  input_read_proc, input_write_proc },   
	{ NULL }
};

void emfat_main_init()
{
    memset(inputfile, '.', sizeof(inputfile));
    emfat_init(&emfat, "boot", entries);
}

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : MAL_Init
* Description    : Initializes the Media on the STM32
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t MAL_Init(uint8_t lun)
{
  uint16_t status = MAL_OK;
  emfat_main_init();
  return status;
}
/*******************************************************************************
* Function Name  : MAL_Write
* Description    : Write sectors
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t MAL_Write(uint8_t lun, uint32_t Memory_Offset, uint32_t *Writebuff, uint16_t Transfer_Length)
{
  if(lun == 0)
  {     
    emfat_write(&emfat,(uint8_t*) Writebuff, Memory_Offset/512, Transfer_Length/512);
    
    return MAL_OK;
  }
      
  return MAL_FAIL;
}

/*******************************************************************************
* Function Name  : MAL_Read
* Description    : Read sectors
* Input          : None
* Output         : None
* Return         : Buffer pointer
*******************************************************************************/
uint16_t MAL_Read(uint8_t lun, uint32_t Memory_Offset, uint32_t *Readbuff, uint16_t Transfer_Length)
{
  if(lun == 0)
  {
    emfat_read(&emfat,(uint8_t*)  Readbuff, Memory_Offset/512, Transfer_Length/512);
    
    return MAL_OK;
  }
      
  return MAL_FAIL;
}

/*******************************************************************************
* Function Name  : MAL_GetStatus
* Description    : Get status
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t MAL_GetStatus (uint8_t lun)
{
    if(lun == 0)
    {
        Mass_Block_Count[0] = emfat.disk_sectors;
        Mass_Block_Size[0] = 512;
        Mass_Memory_Size[0] = (Mass_Block_Count[0] * Mass_Block_Size[0]);
    }
    else
    {
        Mass_Block_Count[1] = 0;
        Mass_Block_Size[1] = 0;
        Mass_Memory_Size[1] = 0;
    }
      
    return MAL_OK;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

