/*
 * defs.h
 *
 *  Created on: Aug 12, 2021
 *      Author: Dimitar
 */

#ifndef WAVEPLAYER_H_
#define WAVEPLAYER_H_

typedef enum
{
  LittleEndian,
  BigEndian
}Endianness;

typedef struct
{
  uint32_t  RIFFchunksize;
  uint16_t  FormatTag;
  uint16_t  NumChannels;
  uint32_t  SampleRate;
  uint32_t  ByteRate;
  uint16_t  BlockAlign;
  uint16_t  BitsPerSample;
  uint32_t  DataSize;
}
WAVE_FormatTypeDef;

typedef enum
{
  Valid_WAVE_File = 0,
  Unvalid_RIFF_ID,
  Unvalid_WAVE_Format,
  Unvalid_FormatChunk_ID,
  Unsupporetd_FormatTag,
  Unsupporetd_Number_Of_Channel,
  Unsupporetd_Sample_Rate,
  Unsupporetd_Bits_Per_Sample,
  Unvalid_DataChunk_ID,
  Unsupporetd_ExtraFormatBytes,
  Unvalid_FactChunk_ID
} ErrorCode;

#define  CHUNK_ID                            0x52494646  /* correspond to the letters 'RIFF' */
#define  FILE_FORMAT                         0x57415645  /* correspond to the letters 'WAVE' */
#define  FORMAT_ID                           0x666D7420  /* correspond to the letters 'fmt ' */
#define  DATA_ID                             0x64617461  /* correspond to the letters 'data' */
#define  FACT_ID                             0x66616374  /* correspond to the letters 'fact' */

#define  WAVE_FORMAT_PCM                     0x01
#define  FORMAT_CHNUK_SIZE                   0x10
#define  CHANNEL_MONO                        0x01
#define  CHANNEL_STEREO                      0x02

#define  SAMPLE_RATE_8000                    8000
#define  SAMPLE_RATE_16000                   16000
#define  SAMPLE_RATE_11025                   11025
#define  SAMPLE_RATE_22050                   22050
#define  SAMPLE_RATE_32000                   32000
#define  SAMPLE_RATE_44100                   44100
#define  SAMPLE_RATE_48000                   48000

#define  BITS_PER_SAMPLE_8                   8
#define  BITS_PER_SAMPLE_16                  16

#define  WAVE_DUMMY_BYTE                     0xA5

/* 12-bit Left alignment (dual channels) used in case of stereo and 16-bit data */
#define DAC_DHR12LD_ADDRESS     0x40007424
/* 8-bit right alignment (dual channels) used in case of stereo and 8-bit data */
#define DAC_DHR8RD_ADDRESS      0x40007428
/* 12-bit left alignment (channel1) used in case of mono and 16-bit data */
#define DAC_DHR12L1_ADDRESS     0x4000740C
/* 8-bit right alignment (channel1) used in case of mono and 8-bit data */
#define DAC_DHR8R1_ADDRESS      0x40007410

uint8_t WavePlayerMenu_Start(uint8_t * FileAddressStart, uint8_t * FileAddressEnd, uint32_t fileSize);

#endif /* WAVEPLAYER_H_ */
