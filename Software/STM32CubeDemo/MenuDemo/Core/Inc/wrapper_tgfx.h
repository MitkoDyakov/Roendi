#ifndef __WRAPPER_TGFX_H
#define __WRAPPER_TGFX_H

#include "stm32l4xx_hal.h"

void DataReader_WaitForReceiveDone();
void DataReader_ReadData(uint32_t address24, uint8_t* buffer, uint32_t length);
void DataReader_StartDMAReadData(uint32_t address24, uint8_t* buffer, uint32_t length);

#endif
