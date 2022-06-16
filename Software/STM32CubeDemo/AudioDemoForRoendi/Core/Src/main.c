/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "crc.h"
#include "dac.h"
#include "dma.h"
#include "lptim.h"
#include "quadspi.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "app_touchgfx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "WavePlayer.h"
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
#define BUFFERSIZE             2048
#define MIN(a,b)               (((a)<(b))? (a):(b))
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

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
volatile uint8_t  movement                  = NONE;
volatile uint8_t  buttonPushed              = NONE;

volatile uint8_t  progress                  = 0;

volatile uint8_t  userInput                 = 0;
volatile uint32_t userInputDelay            = 0;
volatile int16_t *pp;
volatile uint8_t *p = (uint8_t *)FILE_STATRT_ADDRESS;

#define USERTIMEOIT                         350



volatile int count;
uint32_t samples;

uint8_t flg_dma_done;

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
    // flag updated every time the DAC is ready
    flg_dma_done = 1;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM16)
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

        if(userInput == 1)
        {
            userInputDelay++;
            // we do this timeout so when we start rewinding the player is not immediately effected.
            if(userInputDelay > USERTIMEOIT)
            {
                userInputDelay = 0;
                userInput = 0;

                count = samples - (float)samples * ((float)progress / 100.0f);
                pp = (volatile int16_t *)p + (uint32_t)((float)samples * ((float)progress / 100.0f));
            }
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
    uint8_t  clickInput         = 0;
    uint8_t  bufferSelect       = 0;
    uint8_t  bufferSwitch       = 2;
    uint32_t countDif           = 0;
    uint32_t currentEncoderVal  = 0;
    uint32_t prevEncoderVal     = 0;
    uint32_t steps              = 0;
    uint16_t dmaBuf[2][BUFFERSIZE];
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
  MX_DAC1_Init();
  MX_TIM16_Init();
  MX_TouchGFX_Init();
  /* USER CODE BEGIN 2 */

    // init the QSPI and put it into memory mapped mode
    CSP_QUADSPI_Init();
    CSP_QSPI_EnableMemoryMappedMode();

    // for the backlight
    TIM2->CCR1 = 0;
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_Base_Start_IT(&htim7);
    TIM2->CCR1 = 0;  // the display is off, later CCR1 reg is updated via timer interrupt to accomplish smooth dimming

    // init the display driver
    GC9A01_init();

    // this is for the GFX screen refresh
    HAL_TIM_Base_Start_IT(&htim16);

    // for the rotary encoder IP
    HAL_LPTIM_Encoder_Start_IT(&hlptim1, 0xffff);

    // This is not the correct place for this function to be called but
    // some sketchy things to make to refresh the screen while the backlight is off
    MX_TouchGFX_Process();

    // wait for the user to press the encoder
    clickInput = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);
    while(clickInput == 0)
    {
        clickInput = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);
    }

    // turn the backlight on gradually
    targetBacklight = MAX_DISPLAY_BRIGHTNESS;

    // this timer is used for trigger event for the DAC
    HAL_TIM_Base_Start(&htim6);

    // set the pointer at the start of the file in the QSPI flash
    p = (uint8_t *)FILE_STATRT_ADDRESS;

    // By adding numbers to the pointer we are in effect moving the pointer along the file contents
    // in the header you can find useful information, if plan to make universal WAV player

    p += 4; //  "WAVE"     - File Type Header. For our purposes, it always equals “WAVE”
    p += 4; //  4-bytes    - Size of the overall file in bytes
    p += 4; //  "RIFF"     - Marks the file as a riff file. Characters are each 1 byte long.
    p += 4; //  "fmt "     - Format chunk marker. Includes trailing null
    p += 4; //  4-bytes    - Number of bytes in fmt chunk
    p += 2; //  2-bytes    - Type of format (1 is PCM)
    p += 2; //  2-bytes    - Number of channels (assuming 1)
    p += 4; //  4-bytes    - Sample Rate, common values are 44100 (CD), 48000 (DAT). Sample Rate = Number of Samples per second, or Hertz.
    p += 4; //  4-bytes    - Average number of bytes per second (Sample Rate * BitsPerSample * Channels) / 8
    p += 2; //  2-bytes    - Block size
    p += 2; //  2-bytes    - Bits per sample
    p += 4; //  "data"     - “data” chunk header. Marks the beginning of the data section.

    // We are pointing at the next 4 bytes that contain the size of the data section
    samples  = *p + (*(p+1)<<8) + (*(p+2)<<16) + (*(p+3)<<24);
    samples /= 2;  // only one channel

    p += 4; // we are pointing at the sound data
    pp = (int16_t *)p;

    flg_dma_done = 1;
    count = samples;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  // blksize is the size of the data that we read/play at any one time
  const int blkSize = MIN(samples, BUFFERSIZE);

  while (1)
  {
    // count is keeping the count of samples left to be played
    if(0 < count)
    {
        // Double buffering is utilized.
        // One buffer is used for playing sounds and the other is used to read data into.
        // When we are done playing the first buffer we switch them around to have 'fresh' data that we can play,
        // while the old data is being updated

        if(bufferSwitch > 0)
        {
            // Match waveform to DAC output (amplitude and offset) - this is very interesting.
            // The DAC output can go from 0V to Vmax. This means we cannot have negative voltage.
            // But we have negative values in the samples. Take this sine wave as an example.

            // Vmax
            //
            //
            //
            //
            //
            //              ^~~^:                  ^~~^.
            //             7~. .!~               .7^  .7^
            //            7^     !!             .?.     7^
            //           !~       7^            ?:       ?:
            //          :?        .J           ~!        :?
            //          ?:         ~!          J.         7^         .:
            // 0v   ~~~7           J.~~~~~~~~~!!          .?~~~~~~~~~?^

            // To mitigate this clipping, we need to move the hole sine wave up. In effect adding 2047 to the each sample and moving the "zero level" in the middle of the range

            // Vmax         :~~~:                  ^~~^:
            //             !!. .~!                7~. .!~
            //            !~     ^7              7^     !!
            //           ~7       !!            !~       7^
            //          .J         ?:          :?        .J
            //          ^^         ^?          ?:         ~!
            //                      ?:        ^7           J.
            //                      .?        ?.           ^7
            //                       ^7      7^             !!
            //                        ~!.  .7^               !!.
            // 0v                      :~~~~.                 ^!^

            for(int i=0; i< blkSize; i++)
            {
                int16_t v0 = *pp;
                pp++;
                int32_t v = v0;
                v >>= 4;
                v += 2047;                              // half of the max 12bit value. Zero level is in the middle i.e. Vmax / 2.
                dmaBuf[bufferSelect][i] = v & 0x0fff;   // mask only the 12bits we can play.
            }
            bufferSwitch--;
        }

        // we switch the buffer when we are done playing
        if(1 == flg_dma_done)
        {
            flg_dma_done = 0;
            HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t*)dmaBuf[bufferSelect], blkSize, DAC_ALIGN_12B_R);

            bufferSelect = !bufferSelect;
            count -= blkSize;
            bufferSwitch++;
        }

        // this is used for rewinding
        if(0 == userInput)
        {
            progress = 100 - (100.0 * (float)count / (float)samples);
        }
    }

    // The code that follows is for the encoder and display

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
            userInput = 1;
            userInputDelay = 0;
            buttonPushed = LEFT;
            if(progress > 5)
            {
                progress-=5;
            }
            break;

        case RIGHT:
            userInput = 1;
            userInputDelay = 0;
            buttonPushed = RIGHT;
            if(progress < 100)
            {
                progress+=5;

                if(progress > 100)
                {
                    progress = 100;
                }
            }
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
