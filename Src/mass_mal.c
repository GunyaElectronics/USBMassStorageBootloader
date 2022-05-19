#include "platform_config.h"
#include "mass_mal.h"
#include "emfat.h"
#include "flash.h"

//------------------------------- Private variables ---------------------------

uint32_t Mass_Memory_Size[2];
uint32_t Mass_Block_Size[2];
uint32_t Mass_Block_Count[2];

static __IO uint32_t Status = 0;
static emfat_t emfat;

//------------------------------- User code ----------------------------------

void inputReadProcess(uint8_t *dest, int size, uint32_t offset, size_t userdata)
{
    const uint32_t address = USER_FLASH_START_ADDRESS + offset;
	memcpy(dest, (uint8_t *)address, size);
}

void inputWriteProcess(const uint8_t *dest, int size, uint32_t offset, size_t userdata)
{ 
    static uint32_t address = USER_FLASH_START_ADDRESS;
    static int not_erased = 1;
    unsigned char* data = (unsigned char*)dest;

    if (not_erased) {
        uint32_t page_count = (FLASH_END_ADDRESS - USER_FLASH_START_ADDRESS) / PAGE_SIZE;
        uint32_t addr = USER_FLASH_START_ADDRESS;

        __disable_irq();

        for (uint32_t i = 0; i < page_count; i++) {
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
#define FW_SIZE             (FLASH_END_ADDRESS - USER_FLASH_START_ADDRESS)
#define BLOCK_SIZE           512

emfat_entry_t entries[] =
{
	// name     dir    lvl offset size max_size user  time  read write
	{ "",       true,  0,  0,  0,      0,       0,    CMA,  NULL, NULL },
	{ "fw.bin", false, 1,  0,  FW_SIZE,FW_SIZE, 0,    CMA,  inputReadProcess, inputWriteProcess },   
	{ NULL }
};

void emfat_main_init()
{
    emfat_init(&emfat, "boot", entries);
}

//------------------------------ MAL functions ----------------------------

uint16_t MAL_Init(uint8_t lun)
{
    emfat_main_init();
    return MAL_OK;
}

uint16_t MAL_Write(uint8_t lun, uint32_t Memory_Offset, uint32_t
                   *Writebuff, uint16_t Transfer_Length)
{
    if (lun == 0) {     
        emfat_write(&emfat,(uint8_t *) Writebuff,
                    Memory_Offset/BLOCK_SIZE, Transfer_Length/BLOCK_SIZE);
        return MAL_OK;
    }
      
    return MAL_FAIL;
}

uint16_t MAL_Read(uint8_t lun, uint32_t Memory_Offset, uint32_t *Readbuff, uint16_t Transfer_Length)
{
    if (lun == 0) {
        emfat_read(&emfat,(uint8_t *)Readbuff,
                   Memory_Offset/BLOCK_SIZE, Transfer_Length/BLOCK_SIZE);
        return MAL_OK;
    }

    return MAL_FAIL;
}

uint16_t MAL_GetStatus (uint8_t lun)
{
    if (lun == 0) {
        Mass_Block_Count[0] = emfat.disk_sectors;
        Mass_Block_Size[0]  = BLOCK_SIZE;
        Mass_Memory_Size[0] = (Mass_Block_Count[0] * Mass_Block_Size[0]);
    } else {
        Mass_Block_Count[1] = 0;
        Mass_Block_Size[1]  = 0;
        Mass_Memory_Size[1] = 0;
    }
      
    return MAL_OK;
}
