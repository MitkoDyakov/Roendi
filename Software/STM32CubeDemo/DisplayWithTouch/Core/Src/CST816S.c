/*
 * CST816S.c
 *
 *  Created on: Jun 21, 2022
 *      Author: Dimitar
 */
#include "stm32l4xx_hal.h"
#include "gpio.h"
#include "i2c.h"

#include "CST816S.h"

data_struct data;
volatile uint8_t event_available;


// EXTI Line9 External Interrupt ISR Handler CallBackFun
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_1) // If The INT Source Is EXTI Line9 (A9 Pin)
    {
          event_available = 1;
          read_touch();
    }
}

uint8_t touchPanelRead(uint8_t devAddress, uint8_t regAddress, uint8_t *data, uint32_t len)
{
    uint8_t res = 0;
    uint8_t transmitData[32] = {0};
    transmitData[0] = regAddress;

    res = HAL_I2C_Master_Transmit(&hi2c1, devAddress, transmitData, 1, 5000);
    res = HAL_I2C_Master_Receive(&hi2c1, devAddress, data, len, 5000);

    return res;
}

uint8_t touchPanelWrite(uint8_t devAddress, uint8_t regAddress  , uint8_t *data, uint32_t len)
{
    uint8_t res = 0;
    uint8_t transmitData[32] = {0};
    uint16_t i = 0;

    transmitData[0] = regAddress;

    for (i=0; i<len; i++)
    {
        transmitData[i + 1] = *(data + i);
    }

    len = len + 1;

    res = HAL_I2C_Master_Transmit(&hi2c1, devAddress, transmitData, len, 1000);

    return res;
}

void touchPanelWork(void)
{
    if(1 == event_available)
    {
        read_touch();
        event_available = 0;
    }
}

void touchPanelInit(void)
{
    // reset
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_Delay(50);

    uint8_t hwInfo[3] = {0};
    touchPanelRead(CST816S_ADDRESS, CST816S_CHIP_ID, hwInfo, 3);
}

void read_touch(void)
{
    uint8_t data_raw[6] = {0};

    touchPanelRead(CST816S_ADDRESS, 0x01, data_raw, 6);

    data.gestureID = data_raw[0];
    data.points = data_raw[1];
    data.x = (data_raw[2] & 0x0F) << 8 | data_raw[3];
    data.y = (data_raw[4] & 0x0F) << 8 | data_raw[5];
}

void touchPanelWakeup(void)
{
    uint8_t dummy = 0;
    touchPanelRead(CST816S_ADDRESS, 0x15, &dummy, 1);
    HAL_Delay(50);
    touchPanelRead(CST816S_ADDRESS, 0xA7, &dummy, 1);
    HAL_Delay(50);
}

void touchPanelSleep(void)
{
    uint8_t standby_value = 0x03;

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_Delay(50);

    touchPanelWrite(CST816S_ADDRESS, 0xA5, &standby_value, 1);
}

