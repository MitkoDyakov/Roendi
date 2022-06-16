/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    quadspi.c
  * @brief   This file provides code for the configuration
  *          of the QUADSPI instances.
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
/* Includes ------------------------------------------------------------------*/
#include "quadspi.h"

/* USER CODE BEGIN 0 */
uint8_t QSPI_FlashReset(void);
uint8_t QSPI_WriteEnable(void);
uint8_t QSPI_EnableQuadMode(void);
uint8_t QSPI_ManufactureID(uint8_t *ID);
uint8_t QSPI_AutoPollingMemReady(uint32_t timeout);
uint8_t QSPI_AutoPollingWriteEnable(void);
uint8_t QSPI_ReadConfigReg(uint8_t *reg);
uint8_t QSPI_ReadStatusReg1(uint8_t *reg);
uint8_t QSPI_ReadStatusReg2(uint8_t *reg);

uint8_t CSP_QSPI_EnableMemoryMappedMode(void)
{
    QSPI_CommandTypeDef      sCommand;
    QSPI_MemoryMappedTypeDef sMemMappedCfg;

    /* Enable Memory-Mapped mode-------------------------------------------------- */

    sCommand.Instruction            = 0x6C; //0x03
    sCommand.InstructionMode        = QSPI_INSTRUCTION_1_LINE;

    sCommand.AddressSize            = QSPI_ADDRESS_32_BITS;
    sCommand.AddressMode            = QSPI_ADDRESS_1_LINE;
    sCommand.Address                = 0;
    sCommand.AlternateByteMode      = QSPI_ALTERNATE_BYTES_NONE;

    sCommand.DdrMode                = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle       = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode               = QSPI_SIOO_INST_EVERY_CMD;

    sCommand.DataMode               = QSPI_DATA_4_LINES;
    sCommand.NbData                 = 0;

    sCommand.DummyCycles            = 8u;

    sMemMappedCfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;

    //HAL_QSPI_Abort(&hqspi);

    if (HAL_QSPI_MemoryMapped(&hqspi, &sCommand, &sMemMappedCfg) != HAL_OK) {
            return HAL_ERROR;
    }

    return HAL_OK;
}

uint8_t CSP_QSPI_WriteMemory(uint8_t* buffer, uint32_t address, uint32_t buffer_size)
{
    QSPI_CommandTypeDef sCommand;

    uint32_t end_addr = 0;
    uint32_t current_size = 0;
    uint32_t current_addr = 0;

    /* Calculation of the size between the write address and the end of the page */
    while (current_addr <= address)
    {
          current_addr += MEMORY_PAGE_SIZE;
    }

    current_size = current_addr - address;

    /* Check if the size of the data is less than the remaining place in the page */
    if (current_size > buffer_size)
    {
          current_size = buffer_size;
    }

    /* Initialize the adress variables */
    current_addr = address;
    end_addr     = address + buffer_size;

    sCommand.Instruction       = PAGE_PROGRAM_CMD;
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;

    sCommand.AddressMode       = QSPI_ADDRESS_1_LINE;
    sCommand.AddressSize       = QSPI_ADDRESS_32_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;

    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    sCommand.DataMode          = QSPI_DATA_1_LINE;
    sCommand.NbData            = buffer_size;
    sCommand.Address           = address;
    sCommand.DummyCycles       = 0;

    /* Perform the write page by page */
    do {
            sCommand.Address = current_addr;
            sCommand.NbData = current_size;

            if (current_size == 0)
            {
                    return HAL_OK;
            }

            /* Enable write operations */
            if (QSPI_WriteEnable() != HAL_OK)
            {
                    return HAL_ERROR;
            }

            /* Configure the command */
            if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
            {
                    return HAL_ERROR;
            }

            /* Transmission of the data */
            if (HAL_QSPI_Transmit(&hqspi, buffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {

                    return HAL_ERROR;
            }

            /* Configure automatic polling mode to wait for end of program */
            if (QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
            {
                    return HAL_ERROR;
            }

            /* Update the address and size variables for next page programming */
            current_addr += current_size;
            buffer += current_size;
            current_size =((current_addr + MEMORY_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : MEMORY_PAGE_SIZE;

    } while (current_addr <= end_addr);

    return HAL_OK;
}

uint8_t CSP_QSPI_EraseSector(uint32_t EraseStartAddress, uint32_t EraseEndAddress)
{
    QSPI_CommandTypeDef sCommand;

    EraseStartAddress = EraseStartAddress - EraseStartAddress % MEMORY_SECTOR_SIZE;

    /* Initialize the read command */
    sCommand.Instruction       = SECTOR_ERASE_CMD;
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressMode       = QSPI_ADDRESS_1_LINE;
    sCommand.AddressSize       = QSPI_ADDRESS_32_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DummyCycles       = 0u;
    sCommand.DataMode          = QSPI_DATA_NONE;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;


    uint8_t stReg2 = 0;
    QSPI_ReadStatusReg2(&stReg2);

    while (EraseEndAddress >= EraseStartAddress)
    {
        sCommand.Address = (EraseStartAddress & 0x0FFFFFFF);

        if (QSPI_WriteEnable() != HAL_OK) {
                return HAL_ERROR;
        }

        if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
                return HAL_ERROR;
        }

        EraseStartAddress += MEMORY_SECTOR_SIZE;

        if (QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
                return HAL_ERROR;
        }
    }

     QSPI_ReadStatusReg2(&stReg2);
    return HAL_OK;
}

uint8_t CSP_QSPI_Erase_Chip(void)
{
    QSPI_WriteEnable();

    uint8_t reg12 = 0;
    QSPI_ReadStatusReg1(&reg12);

    QSPI_CommandTypeDef sCommand;
    //uint8_t reg = 0;
    /* Initialize the read command */
    sCommand.Instruction       = BULK_ERASE_CMD;
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;

    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.DummyCycles       = 0u;
    sCommand.DataMode          = QSPI_DATA_NONE;
    sCommand.NbData            = 0;

    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }

    QSPI_ReadStatusReg1(&reg12);

    if (QSPI_AutoPollingMemReady(100000) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

uint8_t CSP_QUADSPI_Init(void)
{
   // uint8_t flashID[3] = {0,0,0};

    MX_QUADSPI_Init();

//    if(QSPI_FlashReset() != HAL_OK)
//    {
//        return HAL_ERROR;
//    }
//
//    if(QSPI_EnableQuadMode() != HAL_OK)
//    {
//        return HAL_ERROR;
//    }

//    if(QSPI_ManufactureID(flashID) != HAL_OK)
//    {
//        return HAL_ERROR;
//    }
//
//    if(flashID[0] != 0x01)
//    {
//        return HAL_ERROR;
//    }

    return HAL_OK;
}

uint8_t QSPI_FlashReset(void)
{
    QSPI_CommandTypeDef sCommand;

    /* Initialize the read command */
    sCommand.Instruction       = FLASH_RESET_EN_CMD;
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;

    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;

    sCommand.DummyCycles       = 0u;

    sCommand.DataMode          = QSPI_DATA_NONE;

    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return 1;
    }

    sCommand.Instruction       = FLASH_RESET_CMD;
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;

    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;

    sCommand.DummyCycles       = 0u;

    sCommand.DataMode          = QSPI_DATA_NONE;

    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return 1;
    }

    if (QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
            return HAL_ERROR;
    }

    return 0;
}

uint8_t QSPI_WriteEnableNV(void)
{
    QSPI_CommandTypeDef sCommand;

    /* Initialize the read command */
    sCommand.Instruction       = WRITE_ENABLENV_CMD;
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;

    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AddressSize       = QSPI_ADDRESS_32_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;

    sCommand.DummyCycles       = 0u;

    sCommand.DataMode          = QSPI_DATA_NONE;
    sCommand.NbData            = 0;

    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (HAL_QSPI_Command(&hqspi, &sCommand, 5 * HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }

    if (QSPI_AutoPollingWriteEnable() != HAL_OK) {
		return HAL_ERROR;
    }

    return HAL_OK;
}

uint8_t QSPI_WriteEnable(void)
{
    QSPI_CommandTypeDef sCommand;

    /* Initialize the read command */
    sCommand.Instruction       = WRITE_ENABLE_CMD;
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;

    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AddressSize       = QSPI_ADDRESS_32_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;

    sCommand.DummyCycles       = 0u;

    sCommand.DataMode          = QSPI_DATA_NONE;
    sCommand.NbData            = 0;

    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (HAL_QSPI_Command(&hqspi, &sCommand, 5 * HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }

    if (QSPI_AutoPollingWriteEnable() != HAL_OK) {
		return HAL_ERROR;
    }

    return HAL_OK;
}

uint8_t QSPI_EnableQuadMode(void)
{
    uint8_t configReg       = 0;
    uint8_t statusRegister1 = 0;
    uint8_t data[2]         = {0,0};

    QSPI_CommandTypeDef sCommand;

    QSPI_WriteEnable();
    QSPI_ReadConfigReg(&configReg);
    QSPI_ReadStatusReg1(&statusRegister1);

    if(0 == (configReg & QUAD_MODE_ENABLE_BIT))
    {
        configReg = configReg | QUAD_MODE_ENABLE_BIT;
        data[0]   = statusRegister1;
        data[1]   = configReg;

        sCommand.Instruction       = REG_PROGRAM_CMD;
        sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;

        sCommand.AddressMode       = QSPI_ADDRESS_1_LINE;
        sCommand.AddressSize       = QSPI_ADDRESS_NONE;
        sCommand.Address           = 0;
        sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;

        sCommand.DataMode          = QSPI_DATA_1_LINE;
        sCommand.NbData            = 2;

        sCommand.DummyCycles       = 0;

        sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
        sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
        sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

        /* Configure the command */
        if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
                return HAL_ERROR;
        }

        /* Transmission of the data */
        if (HAL_QSPI_Transmit(&hqspi, data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {

                return HAL_ERROR;
        }

        /* Configure automatic polling mode to wait for end of program */
        if (QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
                return HAL_ERROR;
        }
    }

    //QSPI_ReadConfigReg(&configReg); //for dbg

    return HAL_OK;
}

uint8_t QSPI_ReadStatusReg1(uint8_t *reg)
{
    QSPI_CommandTypeDef sCommand;

    /* Initialize the read command */
    sCommand.Instruction       = READ_STATUS_REG1;
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;

    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AddressSize       = QSPI_ADDRESS_32_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;

    sCommand.DummyCycles       = 0u;

    sCommand.DataMode          = QSPI_DATA_1_LINE;
    sCommand.NbData            = 1;

    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return 1;
    }

    /* Reception of the data */
    if (HAL_QSPI_Receive(&hqspi, reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return 1;
    }

    return 0;
}

uint8_t QSPI_ReadStatusReg2(uint8_t *reg)
{
    QSPI_CommandTypeDef sCommand;

    /* Initialize the read command */
    sCommand.Instruction       = READ_STATUS_REG2;
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;

    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AddressSize       = QSPI_ADDRESS_32_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;

    sCommand.DummyCycles       = 0u;

    sCommand.DataMode          = QSPI_DATA_1_LINE;
    sCommand.NbData            = 1;

    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return 1;
    }

    /* Reception of the data */
    if (HAL_QSPI_Receive(&hqspi, reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return 1;
    }

    return 0;
}

uint8_t QSPI_ReadConfigReg(uint8_t *reg)
{
    QSPI_CommandTypeDef sCommand;

    /* Initialize the read command */
    sCommand.Instruction       = READ_CONFING_REG;
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;

    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AddressSize       = QSPI_ADDRESS_32_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;

    sCommand.DummyCycles       = 0u;

    sCommand.DataMode          = QSPI_DATA_1_LINE;
    sCommand.NbData            = 1;

    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return 1;
    }

    /* Reception of the data */
    if (HAL_QSPI_Receive(&hqspi, reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return 1;
    }

    return 0;
}

uint8_t QSPI_ReadConfigReg2(uint8_t *reg)
{
    QSPI_CommandTypeDef sCommand;

    /* Initialize the read command */
    sCommand.Instruction       = READ_CONFING_REG2;
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;

    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AddressSize       = QSPI_ADDRESS_32_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;

    sCommand.DummyCycles       = 0u;

    sCommand.DataMode          = QSPI_DATA_1_LINE;
    sCommand.NbData            = 1;

    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return 1;
    }

    /* Reception of the data */
    if (HAL_QSPI_Receive(&hqspi, reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return 1;
    }

    return 0;
}

uint8_t QSPI_ManufactureID(uint8_t *ID)
{
    QSPI_CommandTypeDef sCommand;

    /* Initialize the read command */
    sCommand.Instruction       = READ_MANUCATURE_ID;
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;

    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AddressSize       = QSPI_ADDRESS_32_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;

    sCommand.DummyCycles       = 0u;

    sCommand.DataMode          = QSPI_DATA_1_LINE;
    sCommand.NbData            = 3;

    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return 1;
    }

    /* Reception of the data */
    if (HAL_QSPI_Receive(&hqspi, ID, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return 1;
    }

    return 0;
}

uint8_t QSPI_AutoPollingMemReady(uint32_t timeout)
{
    QSPI_CommandTypeDef sCommand;
    QSPI_AutoPollingTypeDef sConfig;

    sCommand.Instruction       = READ_STATUS_REG1;
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DummyCycles       = 0u;
    sCommand.DataMode          = QSPI_DATA_1_LINE;
    sCommand.NbData            = 1;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    sConfig.Match              = 0x00;
    sConfig.Mask               = 0x01;
    sConfig.MatchMode          = QSPI_MATCH_MODE_AND;
    sConfig.StatusBytesSize    = 1;
    sConfig.Interval           = 0x10;
    sConfig.AutomaticStop      = QSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_QSPI_AutoPolling(&hqspi, &sCommand, &sConfig, timeout) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

uint8_t QSPI_AutoPollingWriteEnable(void)
{
    QSPI_CommandTypeDef sCommand;
    QSPI_AutoPollingTypeDef sConfig;

    /* Configure automatic polling mode to wait for memory ready ------ */
    sCommand.Instruction       = READ_STATUS_REG1;
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;

    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AddressSize       = QSPI_ADDRESS_32_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;

    sCommand.DummyCycles       = 0u;

    sCommand.DataMode          = QSPI_DATA_1_LINE;
    sCommand.NbData            = 1;

    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    sConfig.Match              = 0x02;
    sConfig.Mask               = 0x02;
    sConfig.MatchMode          = QSPI_MATCH_MODE_AND;
    sConfig.StatusBytesSize    = 1;
    sConfig.Interval           = 0x10;
    sConfig.AutomaticStop      = QSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_QSPI_AutoPolling(&hqspi, &sCommand, &sConfig, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

uint8_t QSPI_PowerDown(void)
{
    QSPI_CommandTypeDef sCommand;

    /* Initialize the deep power down command */
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction       = DEEP_POWER_DOWN_CMD;
    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode          = QSPI_DATA_NONE;
    sCommand.DummyCycles       = 0;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return 1;
    }

    return HAL_OK;
}
/* USER CODE END 0 */

QSPI_HandleTypeDef hqspi;

/* QUADSPI init function */
void MX_QUADSPI_Init(void)
{

  /* USER CODE BEGIN QUADSPI_Init 0 */

  /* USER CODE END QUADSPI_Init 0 */

  /* USER CODE BEGIN QUADSPI_Init 1 */

  /* USER CODE END QUADSPI_Init 1 */
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 1;
  hqspi.Init.FifoThreshold = 1;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
  hqspi.Init.FlashSize = 24;
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  hqspi.Init.FlashID = QSPI_FLASH_ID_1;
  hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN QUADSPI_Init 2 */

  /* USER CODE END QUADSPI_Init 2 */

}

void HAL_QSPI_MspInit(QSPI_HandleTypeDef* qspiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(qspiHandle->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspInit 0 */

  /* USER CODE END QUADSPI_MspInit 0 */
    /* QUADSPI clock enable */
    __HAL_RCC_QSPI_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**QUADSPI GPIO Configuration
    PA3     ------> QUADSPI_CLK
    PA6     ------> QUADSPI_BK1_IO3
    PA7     ------> QUADSPI_BK1_IO2
    PB0     ------> QUADSPI_BK1_IO1
    PB1     ------> QUADSPI_BK1_IO0
    PB11     ------> QUADSPI_BK1_NCS
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN QUADSPI_MspInit 1 */

  /* USER CODE END QUADSPI_MspInit 1 */
  }
}

void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef* qspiHandle)
{

  if(qspiHandle->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspDeInit 0 */

  /* USER CODE END QUADSPI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_QSPI_CLK_DISABLE();

    /**QUADSPI GPIO Configuration
    PA3     ------> QUADSPI_CLK
    PA6     ------> QUADSPI_BK1_IO3
    PA7     ------> QUADSPI_BK1_IO2
    PB0     ------> QUADSPI_BK1_IO1
    PB1     ------> QUADSPI_BK1_IO0
    PB11     ------> QUADSPI_BK1_NCS
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_7);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_11);

  /* USER CODE BEGIN QUADSPI_MspDeInit 1 */

  /* USER CODE END QUADSPI_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
