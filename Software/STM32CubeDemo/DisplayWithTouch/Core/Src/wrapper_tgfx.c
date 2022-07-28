#include "wrapper_tgfx.h"
#include "GC9A01.h"

volatile uint8_t isTransmittingData = 0;

void DisplayDriver_TransferCompleteCallback();

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hspi);
  isTransmittingData = 0;
  DisplayDriver_TransferCompleteCallback();
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_TxCpltCallback should be implemented in the user file
   */
}

int touchgfxDisplayDriverTransmitActive()
{
    return isTransmittingData;
}

static void ConvHL(uint8_t *s, int32_t l)
{
  uint8_t v;

  while (l > 0) 
  {
    v = *(s+1);
    *(s+1) = *s;
    *s = v;
    s += 2;
    l -= 2;
  }
}

void touchgfxDisplayDriverTransmitBlock(const uint8_t* pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  isTransmittingData = 1;

  uint8_t startX = 0;  
  uint8_t startY = 0;
  uint8_t endX   = 0;    
  uint8_t endY   = 0;  
  
  startX = x;
  startY = y;
  endX   = (x + w) - 1;
  endY   = (y + h) - 1;
    
  struct GC9A01_frame frame = {{startX, startY}, {endX, endY}};

  GC9A01_set_frame(frame);
  
  GC9A01_write_command(MEM_WR);
  
  ConvHL((uint8_t *)pixels, (int32_t)w*h*2);
  GC9A01_write_DMA((uint8_t *)pixels, h*w*2);
}
