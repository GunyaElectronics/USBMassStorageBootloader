#ifndef _CRC_H
#define _CRC_H

//==================[inclusions]==============================================

#include <stdint.h>
 
//==================[public functions declaration]============================

uint32_t getCrc32(uint32_t crc, uint8_t * pData, uint32_t dataLength);

//==================[end of file]=============================================

#endif // _CRC_H
