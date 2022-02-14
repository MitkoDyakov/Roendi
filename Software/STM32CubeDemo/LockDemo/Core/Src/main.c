/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "dma.h"
#include "lptim.h"
#include "quadspi.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "app_touchgfx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "GC9A01.h"
#include "math.h"
#include "defs.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define NONE    (0u)
#define LEFT    (1u)
#define RIGHT   (2u)

#define ENCODER_GOING_UP    (0u)
#define ENCODER_GOING_DOWN  (1u)

#define INPUT_TIMEOUT          (1300u)   // around 2 sec
#define DEMO_RUNNING_TIMEOUT   (48000u)  // around 60 sec
#define MAX_DISPLAY_BRIGHTNESS (65520u)
#define MAX_ENCODER_CNT        (65535u)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

volatile int32_t  volume       = 0;
volatile uint8_t  updateFromPC = 0;
volatile uint8_t  movement     = NONE;
volatile uint32_t steps        = 0;

volatile uint8_t buttonPushed  = NONE;
volatile uint8_t userSelection;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void touchgfxSignalVSync(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// global variables for back light operation
volatile uint32_t targetBacklight           = 0;  // 65520 is max brightness
volatile uint8_t  blacklightTransitionReady = 1;  // if this value is not 1 means the back light is in transition from one value to another


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM6)
	{
		touchgfxSignalVSync();
	}

	if (htim->Instance == TIM7)
	{
		if(TIM2->CCR1 > targetBacklight)
		{
			TIM2->CCR1 = TIM2->CCR1 - 70;
		}

		if(TIM2->CCR1 < targetBacklight)
		{
			TIM2->CCR1 = TIM2->CCR1 + 70;
		}

		if(TIM2->CCR1 == targetBacklight)
		{
			blacklightTransitionReady = 1;
		}else{
			blacklightTransitionReady = 0;
		}
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  // this is for navigation
  uint32_t countDif           = 0;
  uint32_t currentEncoderVal  = 0;
  uint32_t prevEncoderVal     = 0;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRC_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_DMA_Init();
  MX_SPI2_Init();
  MX_QUADSPI_Init();
  MX_LPTIM1_Init();
  MX_TouchGFX_Init();
  /* USER CODE BEGIN 2 */

  //for the external flash and memory mapped mode
  CSP_QUADSPI_Init();
  CSP_QSPI_EnableMemoryMappedMode();

  //for the back light PWM and dimming
  TIM2->CCR1 = 0;
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_Base_Start_IT(&htim7);
  TIM2->CCR1 = targetBacklight;

  // init the display driver
  GC9A01_init();

  // this is for the GFX screen refresh
  HAL_TIM_Base_Start_IT(&htim6);

  // for the rotary encoder IP
  HAL_LPTIM_Encoder_Start_IT(&hlptim1, 0xffff);

  //Some sketchy things to make to refresh the screen while the backlight is off
  MX_TouchGFX_Process();

  // turn the backlight on gradually
  targetBacklight = MAX_DISPLAY_BRIGHTNESS;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	 // get the latest value of the encoder
	 currentEncoderVal = HAL_LPTIM_ReadCounter(&hlptim1);

	 // check if the user has turned the encoder
	 if(prevEncoderVal != currentEncoderVal)
	 {
		// get the difference between current counter value and the new counter value
		if(currentEncoderVal > prevEncoderVal)
		{
			countDif = currentEncoderVal - prevEncoderVal;
			movement = LEFT;
		}else{
			countDif = prevEncoderVal - currentEncoderVal;
			movement = RIGHT;
		}

		// here we can see if we have overflow of the counter or not
		if(countDif > (MAX_ENCODER_CNT/2))
		{
			// correct for the over/under flow
			countDif = MAX_ENCODER_CNT - countDif; // just subtract from max value

			if(LEFT == movement)
			{
				movement = RIGHT;
			}else{
				movement = LEFT;
			}
		}

		steps = countDif / 2;

		if(0 != steps)
		{
			prevEncoderVal = currentEncoderVal;
		}else{
			movement = NONE;
		}
	 }else{
		 movement = NONE;
	 }

	 switch(movement)
	 {
		 case LEFT:
		     buttonPushed = LEFT;
			    break;

		 case RIGHT:
		     buttonPushed = RIGHT;
				break;

		 case NONE:
		     //buttonPushed = NONE;
				break;

		 default:
				break;
	 }
    /* USER CODE END WHILE */

  MX_TouchGFX_Process();
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
