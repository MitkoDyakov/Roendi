//#include "stm32l4xx_hal.h"
//#include "stm32l4xx_hal_dma.h"
//#include "WavePlayer.h"
//#include "string.h"
//#include "main.h"
//#include "tim.h"
//#include "dac.h"
//
//#define BUFFER_SIZE_BYTE     512
//#define BUFFER_SIZE_WORD     BUFFER_SIZE_BYTE / 4
//
//uint8_t Buffer1[BUFFER_SIZE_BYTE] = {0};
//uint8_t Buffer2[BUFFER_SIZE_BYTE] = {0};
//
//WAVE_FormatTypeDef WAVE_Format;
//static __IO ErrorCode WaveFileStatus  = Unvalid_RIFF_ID;
//static __IO uint32_t SpeechDataOffset = 0x00;
//uint16_t TIM6ARRValue                 = 6000;
//__IO uint32_t WaveDataLength          = 0;
//__IO uint32_t BufferSelection         = 1;
//__IO uint32_t BufferRead              = 0;
//__IO uint32_t WaveCounter;
//
//static ErrorCode WavePlayer_WaveParsing(const char * WavAddress, uint32_t FileLen);
//static void ToggleBufferSign(uint32_t* pBuffer, uint32_t BufferSize);
//static void Audio_MAL_Play(uint32_t Addr, uint32_t Size);
//uint32_t ReadUnit(uint8_t *buffer, uint8_t idx, uint8_t NbrOfBytes, Endianness BytesFormat);
//void WavePlayer_Init(void);
//
///**
//  * @brief  Starts playing wave.
//  * @param  FileName: pointer to the wave file name to be read
//  * @param  FileLen: pointer to the wave file length to be read
//  * @retval None
//  */
//
//#define BUFFERSIZE 2048
//#define MIN(a,b) (((a)<(b))? (a):(b))
//extern uint8_t *__USER_DATA;
//
//
//uint8_t WavePlayerMenu_Start(uint8_t * FileAddressStart, uint8_t * FileAddressEnd, uint32_t fileSize)
//{
//
//    HAL_TIM_Base_Start(&htim6);
//
//    uint8_t *p = (uint8_t *)FILE_STATRT_ADDRESS;
//     p += 4; // "RIFF"
//     p += 4; // samples
//     p += 4; // "WAVE"
//     p += 4; // "fmt "
//     p += 4; // Number of bytes in fmt chunk
//     p += 2; // Audio format
//     p += 2; // Number of channels (assuming 1)
//     p += 4; // Sampling frequency (Hz)
//     p += 4; // Average number of bytes per second (sampling frequency * block size)
//     p += 2; // Block size
//     p += 2; // Bit / sample
//     p += 4; // "data"
//
//    // Sub-chunk size "Number of bytes" of waveform data
//    uint32_t samples = *p + (*(p+1) * 256)  + (*(p+2) * 256 * 256)  + (*(p+3) * 256 * 256 * 256); // this is like shift
//    samples /= 2;  // only one channel?
//    p += 4;
//
//    // beep
//    int16_t *pp = (int16_t *)p;
//    uint16_t dmaBuf[2][BUFFERSIZE];
//    int bank = 0;
//
//  //  flg_dma_done = 1;
//    int count = samples;
//
//    while (0 < count)
//    {
//        const int blksize = MIN(samples, BUFFERSIZE);
//
//        // Match waveform to DAC output (amplitude and offset)
//        for(int i=0; i< blksize; i++) {
//            int16_t v0 = *pp;
//            pp++;
//            int32_t v = v0;
//            v >>= 4;
//            v += 2047;    // half of the max 12bit value. Zero level is in the middle it has + and - values
//
//            dmaBuf[bank][i] = v & 0x0fff;
//        }
//
//        // wait for DMA complete
//        while(!flg_dma_done) {
//            __NOP();
//        }
//
//        flg_dma_done = 0;
//        HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t*)dmaBuf[bank], blksize, DAC_ALIGN_12B_R);
//
//        bank = !bank;
//        count -= blksize;
//    }
//
//    HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
//
//return 0;
//
//  WaveFileStatus = Unvalid_RIFF_ID;
//  uint32_t musicPointer = 0;
//  uint32_t tmp;
//
//  /* Read the Speech wave file status */
//  WaveFileStatus = WavePlayer_WaveParsing((const char * )FileAddressStart, fileSize);
//
//  if(WaveFileStatus == Valid_WAVE_File)  /* the .WAV file is valid */
//  {
//        /* Set WaveDataLenght to the Speech wave length */
//        WaveDataLength = WAVE_Format.DataSize;
//  }
//  else
//  {
//        return 0;
//  }
//
//  WavePlayer_Init();
//
//  /* Jump to wave data */
//  musicPointer = SpeechDataOffset;
//
//  memcpy(Buffer1, FileAddressStart + musicPointer, BUFFER_SIZE_BYTE);
//  musicPointer = musicPointer + BUFFER_SIZE_BYTE;
//
//  /* Convert signed to unsigned format */
//  ToggleBufferSign(Buffer1, BUFFER_SIZE_BYTE);
//
//  /* Play buffer 1 */
//  Audio_MAL_Play((uint32_t)Buffer1, BUFFER_SIZE_BYTE);
//
//  /* Store data in buffer 2 */
//  memcpy(Buffer2, FileAddressStart + musicPointer, BUFFER_SIZE_BYTE);
//  musicPointer = musicPointer + BUFFER_SIZE_BYTE;
//  /* Invert sign bit: PCM format is 16-bit signed and DAC is 12-bit unsgined */
//  ToggleBufferSign(Buffer2, BUFFER_SIZE_BYTE);
//
//  BufferSelection = 2;
//  WaveDataLength = WAVE_Format.DataSize;
//  /* while not end of file */
//
//  uint32_t j = 0;
//
//  while ((uint32_t)(FileAddressStart + musicPointer) < FILE_END_ADDRESS)
//  {
//    if(BufferRead != 0)
//    {
//      j++;
//      if(BufferSelection == 2)
//      {
//        /* Play buffer 2 */
//        Audio_MAL_Play((uint32_t)Buffer2,  BUFFER_SIZE_BYTE);
//        /* Store data in buffer 1 */
//        memcpy(Buffer1, FileAddressStart + musicPointer, BUFFER_SIZE_BYTE);
//        musicPointer = musicPointer + BUFFER_SIZE_BYTE;
//
//        /* Convert signed to unsigned format */
//        ToggleBufferSign(Buffer1, BUFFER_SIZE_BYTE);
//
//        BufferRead = 0;
//      }
//      else if (BufferSelection == 1)
//      {
//        /* Play buffer 1 */
//        Audio_MAL_Play((uint32_t)Buffer1, BUFFER_SIZE_BYTE);
//        /* Store data in buffer 2 */
//        memcpy(Buffer2, FileAddressStart + musicPointer, BUFFER_SIZE_BYTE);
//        musicPointer = musicPointer + BUFFER_SIZE_BYTE;
//
//        /* Convert signed to unsigned format */
//        ToggleBufferSign(Buffer2, BUFFER_SIZE_BYTE);
//
//        BufferRead = 0;
//      }
//      WaveDataLength -= BUFFER_SIZE_BYTE;
//      tmp = (uint8_t) ((uint32_t)((WAVE_Format.DataSize - WaveDataLength) * 100) / WAVE_Format.DataSize);
//    }
//  }
////
////  DMA1_Channel3->CCR = 0x0;
////
////  /* Disable TIM6 */
////  TIM_Cmd(TIM6, DISABLE);
////
////  WaveDataLength = 0;
//
//  return 0;
//}
//
//
///**
//  * @brief  Checks the format of the .WAV file and gets information about the audio
//  *         format. This is done by reading the value of a number of parameters
//  *         stored in the file header and comparing these to the values expected
//  *         authenticates the format of a standard .WAV  file (44 bytes will be read).
//  *         If  it is a valid .WAV file format, it continues reading the header
//  *         to determine the  audio format such as the sample rate and the sampled
//  *         data size. If the audio format is supported by this application, it
//  *         retrieves the audio format in WAVE_Format structure and returns a zero
//  *         value. Otherwise the function fails and the return value is nonzero.
//  *         In this case, the return value specifies the cause of  the function
//  *         fails. The error codes that can be returned by this function are declared
//  *         in the header file.
//  * @param  WavName: wav file name
//  * @param  FileLen: wav file length
//  * @retval Zero value if the function succeed, otherwise it returns a nonzero
//  *         value which specifies the error code.
//  */
//static ErrorCode WavePlayer_WaveParsing(const char * WavAddress, uint32_t FileLen)
//{
//    uint32_t temp = 0x00;
//    uint32_t extraformatbytes = 0;
//    uint16_t TempBuffer[BUFFER_SIZE_BYTE];
//
//    //void * memcpy ( void * destination, const void * source, size_t num );
//    memcpy(TempBuffer, WavAddress, BUFFER_SIZE_BYTE);
//
//    /* Read chunkID, must be 'RIFF'  -------------------------------------------*/
//    temp = ReadUnit((uint8_t*)TempBuffer, 0, 4, BigEndian);
//
//    if(temp != CHUNK_ID)
//    {
//        return(Unvalid_RIFF_ID);
//    }
//
//    /* Read the file length ----------------------------------------------------*/
//    WAVE_Format.RIFFchunksize = ReadUnit((uint8_t*)TempBuffer, 4, 4, LittleEndian);
//
//    /* Read the file format, must be 'WAVE' ------------------------------------*/
//    temp = ReadUnit((uint8_t*)TempBuffer, 8, 4, BigEndian);
//
//    if(temp != FILE_FORMAT)
//    {
//        return(Unvalid_WAVE_Format);
//    }
//
//    /* Read the format chunk, must be'fmt ' ------------------------------------*/
//    temp = ReadUnit((uint8_t*)TempBuffer, 12, 4, BigEndian);
//    if(temp != FORMAT_ID)
//    {
//        return(Unvalid_FormatChunk_ID);
//    }
//    /* Read the length of the 'fmt' data, must be 0x10 -------------------------*/
//    temp = ReadUnit((uint8_t*)TempBuffer, 16, 4, LittleEndian);
//    if(temp != 0x10)
//    {
//        extraformatbytes = 1;
//    }
//    /* Read the audio format, must be 0x01 (PCM) -------------------------------*/
//    WAVE_Format.FormatTag = ReadUnit((uint8_t*)TempBuffer, 20, 2, LittleEndian);
//    if(WAVE_Format.FormatTag != WAVE_FORMAT_PCM)
//    {
//        return(Unsupporetd_FormatTag);
//    }
//
//    /* Read the number of channels, must be 0x02 (Stereo) ----------------------*/
//    WAVE_Format.NumChannels = ReadUnit((uint8_t*)TempBuffer, 22, 2, LittleEndian);
//
//    if(WAVE_Format.NumChannels != CHANNEL_MONO)
//    {
//        return(Unsupporetd_Number_Of_Channel);
//    }
//
//    /* Read the Sample Rate ----------------------------------------------------*/
//    WAVE_Format.SampleRate = ReadUnit((uint8_t*)TempBuffer, 24, 4, LittleEndian);
//    /* Update the OCA value according to the .WAV file Sample Rate */
//    switch (WAVE_Format.SampleRate)
//    {
//        case SAMPLE_RATE_8000 :
//            TIM6ARRValue = 6000;
//            break; /* 8KHz = 48MHz / 6000 */
//        case SAMPLE_RATE_16000 :
//            TIM6ARRValue = 3000;
//            break; /* 8KHz = 48MHz / 6000 */
//        case SAMPLE_RATE_11025:
//            TIM6ARRValue = 4353;
//            break; /* 11.025KHz = 48MHz / 4353 */
//        case SAMPLE_RATE_22050:
//            TIM6ARRValue = 3628;
//            break; /* 22.05KHz = 48MHz / 2176 */
//        case SAMPLE_RATE_32000:
//            TIM6ARRValue = 1500;
//            break; /* 32KHz = 48MHz / 1500 */
//        case SAMPLE_RATE_44100:
//            TIM6ARRValue = 1088;
//            break; /* 44.1KHz = 48MHz / 1088 */
//
//        default:
//        return(Unsupporetd_Sample_Rate);
//    }
//
//    /* Read the Byte Rate ------------------------------------------------------*/
//    WAVE_Format.ByteRate = ReadUnit((uint8_t*)TempBuffer, 28, 4, LittleEndian);
//
//    /* Read the block alignment ------------------------------------------------*/
//    WAVE_Format.BlockAlign = ReadUnit((uint8_t*)TempBuffer, 32, 2, LittleEndian);
//
//    /* Read the number of bits per sample --------------------------------------*/
//    WAVE_Format.BitsPerSample = ReadUnit((uint8_t*)TempBuffer, 34, 2, LittleEndian);
//
//    if(WAVE_Format.BitsPerSample != BITS_PER_SAMPLE_8)
//    {
//        return(Unsupporetd_Bits_Per_Sample);
//    }
//
//    SpeechDataOffset = 36;
//    /* If there is Extra format bytes, these bytes will be defined in "Fact Chunk" */
//    if(extraformatbytes == 1)
//    {
//        /* Read th Extra format bytes, must be 0x00 ------------------------------*/
//        temp = ReadUnit((uint8_t*)TempBuffer, 36, 2, LittleEndian);
//        if(temp != 0x00)
//        {
//            return(Unsupporetd_ExtraFormatBytes);
//        }
//        /* Read the Fact chunk, must be 'fact' -----------------------------------*/
//        temp = ReadUnit((uint8_t*)TempBuffer, 38, 4, BigEndian);
//        if(temp != FACT_ID)
//        {
//            return(Unvalid_FactChunk_ID);
//        }
//        /* Read Fact chunk data Size ---------------------------------------------*/
//        temp = ReadUnit((uint8_t*)TempBuffer, 42, 4, LittleEndian);
//
//        SpeechDataOffset += 10 + temp;
//    }
//    /* Read the Data chunk, must be 'data' -------------------------------------*/
//    temp = ReadUnit((uint8_t*)TempBuffer, SpeechDataOffset, 4, BigEndian);
//    SpeechDataOffset += 4;
//    if(temp != DATA_ID)
//    {
//        return(Unvalid_DataChunk_ID);
//    }
//
//    /* Read the number of sample data ------------------------------------------*/
//    WAVE_Format.DataSize = ReadUnit((uint8_t*)TempBuffer, SpeechDataOffset, 4, LittleEndian);
//    SpeechDataOffset += 4;
//    WaveCounter =  SpeechDataOffset;
//    return(Valid_WAVE_File);
//}
//
///**
//  * @brief  Reads a number of bytes from the SPI Flash and reorder them in Big
//  *         or little endian.
//  * @param  NbrOfBytes: number of bytes to read.
//  *         This parameter must be a number between 1 and 4.
//  * @param  ReadAddr: external memory address to read from.
//  * @param  Endians: specifies the bytes endianness.
//  *         This parameter can be one of the following values:
//  *             - LittleEndian
//  *             - BigEndian
//  * @retval Bytes read from the SPI Flash.
//  */
//uint32_t ReadUnit(uint8_t *buffer, uint8_t idx, uint8_t NbrOfBytes, Endianness BytesFormat)
//{
//  uint32_t index = 0;
//  uint32_t temp = 0;
//
//  for (index = 0; index < NbrOfBytes; index++)
//  {
//    temp |= buffer[idx + index] << (index * 8);
//  }
//
//  if(BytesFormat == BigEndian)
//  {
//    temp = __REV(temp);
//  }
//
//  return temp;
//}
//
///**
//  * @brief  Starts playing audio stream from the audio Media.
//  * @param  Add: Buffer address
//  * @param  Size: Buffer size
//  * @retval None
//  */
//static void Audio_MAL_Play(uint32_t Addr, uint32_t Size) // Words
//{
////    DMA_HandleTypeDef DMA_InitStructure;
//
////  /* Enable DMA1 Channel3 */
////  DMA_Cmd(DMA1_Channel3, DISABLE);
//    HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
//
////  /* Disable TIM6 counter */
////    TIM_Cmd(TIM6, DISABLE);
//    HAL_TIM_Base_Stop(&htim6);
//
////  /* Configure the buffer address and size */
////  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Addr;
////  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12LD_ADDRESS;
////  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
////  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
////  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
////  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
////  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
////  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
////  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
////  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
////  DMA_InitStructure.DMA_BufferSize = (uint32_t)Size;
////
////  hdma_dac_ch1.Instance = DMA1_Channel3;
////  hdma_dac_ch1.Init.Request = DMA_REQUEST_6;
////  hdma_dac_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
////  hdma_dac_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
////  hdma_dac_ch1.Init.MemInc = DMA_MINC_ENABLE;
////  hdma_dac_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
////  hdma_dac_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
////  hdma_dac_ch1.Init.Mode = DMA_NORMAL;
////  hdma_dac_ch1.Init.Priority = DMA_PRIORITY_HIGH;
//
////  /* Configure the DMA Channel with the new parameters */
////  DMA_Init(DMA1_Channel3, &DMA_InitStructure);
////  /* Enable the DMA Channel */
////  DMA_Cmd(DMA1_Channel3, ENABLE);
////
////  /* Enable DMA for DAC Channel 1 */
////  DAC_DMACmd(DAC_Channel_1, ENABLE);
//    HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *)Addr, Size, DAC_ALIGN_8B_R);
//
////  /* Enable TIM6 counter */
////  TIM_Cmd(TIM6, ENABLE);
//    HAL_TIM_Base_Start(&htim6);
//}
//
///**
//  * @brief  Wave player Initialization
//  * @param  None
//  * @retval None
//  */
//void WavePlayer_Init(void)
//{
////  DAC_InitTypeDef  DAC_InitStructure;
////  GPIO_InitTypeDef GPIO_InitStructure;
////  NVIC_InitTypeDef NVIC_InitStructure;
////
////  /* TIM6 clock enable */
////  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
////
////  /******************************* TIM6 Configuration *************************/
////  TIM_DeInit(TIM6);
////  /* Set the timer period */
////  TIM_SetAutoreload(TIM6, TIM6ARRValue);
////
////  /* TIM6 TRGO update selection */
////  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
////
////  /* DAC Periph clock enable */
////  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
////
////  DAC_DeInit();
////
////  /* GPIOA clock enable */
////  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
////
////  /******************************* Init DAC channel 1 (PA4) *******************/
////  /* Configure PA.04 (DAC_OUT1) in analog mode */
////  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 ;
////  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
////  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
////  GPIO_Init(GPIOA, &GPIO_InitStructure);
////
////  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
////  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
////  DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
////  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
////  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
////
////  /* Enable DAC Channel 1 */
////  DAC_Cmd(DAC_Channel_1, ENABLE);
////
////  /************************** Init DAC channel 2 (PA5) ************************/
////  /* Configure PA.05 (DAC_OUT2) in analog mode */
////  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
////  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
////  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
////  GPIO_Init(GPIOA, &GPIO_InitStructure);
////
////  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
////  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
////  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
////  DAC_Init(DAC_Channel_2, &DAC_InitStructure);
////
////  /* Enable DAC Channel 2 */
////  DAC_Cmd(DAC_Channel_2, ENABLE);
////
////  /********************************* DMA Config *******************************/
////  /* DMA1 clock enable (to be used with DAC) */
////  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
////  /* DMA channel3 Configuration is managed by Audio_MAL_Play() function */
////
////  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_3_IRQn;
////  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
////  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
////  NVIC_Init(&NVIC_InitStructure);
////
////  /* Enable Transfer Complete on channel 3 */
////  DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
//}
//
///**
//  * @brief  Toggles sign bit of input buffer.
//  * @param  pBuffer: pointer to the input buffer
//  * @param  BufferSize: the size of the buffer in words
//  * @retval None
//  */
//static void ToggleBufferSign(uint32_t* pBuffer, uint32_t BufferSize)
//{
//  return;
//
//  uint32_t readdata = (uint32_t)pBuffer;
//  uint32_t loopcounter = 0;
//
//  /* Invert sign bit: PCM format is 16-bit signed and DAC is 12-bit unsigned */
//  for(loopcounter = 0; loopcounter < BufferSize; loopcounter++)
//  {
//    *(uint32_t*)readdata ^= 0x80008000;
//    readdata+=4;
//  }
//}
//
//
//
//
