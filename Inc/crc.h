/***************************************************************************//**
 * @file       crc.h
 * @brief      Provides crc function prototypes and defines
 * @version    1
 * @author     Maxim Hunko
 * @date       27-JAN-2017
 * @copyright  See the copyright file (TBD)
 *
 * Description:
 * -------------------------------------------------------------------
 *
 * History
 * -------------------------------------------------------------------
 * Version --- Date --- Name
 *
 * 1 --- 27-JAN-2017 - Maxim Hunko
 * 
 *     Initial version
 *
 ******************************************************************************/

#ifndef _CRC_H
#define _CRC_H

/*==================[inclusions]==============================================*/

#include <stdint.h>

/**
 * @brief Provides crc function prototypes and defines
 * @addtogroup CRC CRC_32
 */
 
/*==================[public functions declaration]============================*/

uint32_t get_crc32(uint32_t crc, uint8_t * p_data, uint32_t data_len);

/** @} Doxygen end group definition */

/*==================[end of file]=============================================*/

#endif /* _CRC_H */
