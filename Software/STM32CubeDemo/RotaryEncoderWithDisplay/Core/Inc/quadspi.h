/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    quadspi.h
  * @brief   This file contains all the function prototypes for
  *          the quadspi.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __QUADSPI_H__
#define __QUADSPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
//Device description
#define MEMORY_FLASH_SIZE        (0x1000000u)  /* 128 MBits => 16MBytes */
#define MEMORY_SECTOR_SIZE       (0x1000u)     /* 4096 sectors of 4kBytes */
#define MEMORY_PAGE_SIZE         (0x100u)      /* 256 pages of 256 bytes */
#define MEMORY_BLOCK_SIZE        (0x10000u)    /* 256 blocks of 64KBytes */

#define QUAD_MODE_ENABLE_BIT     (0x02)

#define READ_MANUCATURE_ID       (0x9Fu)
#define READ_STATUS_REG1         (0x05u)
#define READ_STATUS_REG2         (0x07u)
#define READ_CONFING_REG         (0x35u)
#define READ_CONFING_REG2        (0x15u)
#define DEEP_POWER_DOWN_CMD      (0xB9u)

#define REG_PROGRAM_CMD          (0x01u)
#define PAGE_PROGRAM_CMD         (0x12u)
#define READ_CMD                 (0x03u)
#define SECTOR_ERASE_CMD         (0x21u)
#define BULK_ERASE_CMD           (0x60u)
#define FLASH_RESET_EN_CMD       (0x66u)
#define FLASH_RESET_CMD          (0x99u)
#define WRITE_ENABLE_CMD         (0x06u)
#define WRITE_ENABLENV_CMD       (0x50u)
/* USER CODE END Includes */

extern QSPI_HandleTypeDef hqspi;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_QUADSPI_Init(void);

/* USER CODE BEGIN Prototypes */
//loader
uint8_t CSP_QSPI_EnableMemoryMappedMode(void);
uint8_t CSP_QSPI_WriteMemory(uint8_t* buffer, uint32_t address, uint32_t buffer_size);
uint8_t CSP_QSPI_EraseSector(uint32_t EraseStartAddress, uint32_t EraseEndAddress);
uint8_t CSP_QSPI_Erase_Chip(void);
uint8_t CSP_QUADSPI_Init(void);
uint8_t QSPI_PowerDown(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __QUADSPI_H__ */

