#include "GC9A01.h"
#include "main.h"
#include "spi.h"
#include "stm32l4xx_hal.h"

#define ORIENTATION 2   // Set the display orientation 0,1,2,3

// Command codes:
#define COL_ADDR_SET        0x2A
#define ROW_ADDR_SET        0x2B
#define MEM_WR              0x2C
#define COLOR_MODE          0x3A
#define COLOR_MODE__12_BIT  0x03
#define COLOR_MODE__16_BIT  0x05
#define COLOR_MODE__18_BIT  0x06
#define MEM_WR_CONT         0x3C
#define ENTER_SLEEP         0x10

void GC9A01_set_reset(uint8_t val) 
{
      HAL_GPIO_WritePin(SPI2_RS_GPIO_Port, SPI2_RS_Pin, val == 0? GPIO_PIN_RESET:GPIO_PIN_SET);
}

void GC9A01_set_data_command(uint8_t val) 
{
      HAL_GPIO_WritePin(SPI2_CD_GPIO_Port, SPI2_CD_Pin, val == 0? GPIO_PIN_RESET:GPIO_PIN_SET);
}

void GC9A01_set_chip_select(uint8_t val) 
{
      //HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, val == 0? GPIO_PIN_RESET:GPIO_PIN_SET);
}

void GC9A01_spi_tx(uint8_t *data, size_t len) 
{
    HAL_SPI_Transmit(&hspi2, data, len, 10);
}

void GC9A01_delay(uint16_t ms) {
    HAL_Delay(ms);
}

void GC9A01_write_command(uint8_t cmd) {
    GC9A01_set_data_command(0);
    GC9A01_set_chip_select(0);
    GC9A01_spi_tx(&cmd, sizeof(cmd));
    GC9A01_set_chip_select(1);
}

static void GC9A01_write_data(uint8_t *data, size_t len) {
    GC9A01_set_data_command(1);
    GC9A01_set_chip_select(0);
    GC9A01_spi_tx(data, len);
    GC9A01_set_chip_select(1);
}

static inline void GC9A01_write_byte(uint8_t val) {
    GC9A01_write_data(&val, sizeof(val));
}

void GC9A01_sleep(void)
{
	GC9A01_write_command(ENTER_SLEEP);
}

void GC9A01_init(void) {
    
    GC9A01_set_chip_select(1);
    GC9A01_delay(5);
    GC9A01_set_reset(0);
    GC9A01_delay(10);
    GC9A01_set_reset(1);
    GC9A01_delay(120);

    /* Initial Sequence */ 
    
    GC9A01_write_command(0xEF);
    
    GC9A01_write_command(0xEB);
    GC9A01_write_byte(0x14);
    
    GC9A01_write_command(0xFE);
    GC9A01_write_command(0xEF);
    
    GC9A01_write_command(0xEB);
    GC9A01_write_byte(0x14);
    
    GC9A01_write_command(0x84);
    GC9A01_write_byte(0x40);
    
    GC9A01_write_command(0x85);
    GC9A01_write_byte(0xFF);
    
    GC9A01_write_command(0x86);
    GC9A01_write_byte(0xFF);
    
    GC9A01_write_command(0x87);
    GC9A01_write_byte(0xFF);
    
    GC9A01_write_command(0x88);
    GC9A01_write_byte(0x0A);
    
    GC9A01_write_command(0x89);
    GC9A01_write_byte(0x21);
    
    GC9A01_write_command(0x8A);
    GC9A01_write_byte(0x00);
    
    GC9A01_write_command(0x8B);
    GC9A01_write_byte(0x80);
    
    GC9A01_write_command(0x8C);
    GC9A01_write_byte(0x01);
    
    GC9A01_write_command(0x8D);
    GC9A01_write_byte(0x01);
    
    GC9A01_write_command(0x8E);
    GC9A01_write_byte(0xFF);
    
    GC9A01_write_command(0x8F);
    GC9A01_write_byte(0xFF);
    
    
    GC9A01_write_command(0xB6);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x00);
    
    GC9A01_write_command(0x36);
    
#if ORIENTATION == 0
    GC9A01_write_byte(0x18);
#elif ORIENTATION == 1
    GC9A01_write_byte(0x28);
#elif ORIENTATION == 2
    GC9A01_write_byte(0x48);
#else
    GC9A01_write_byte(0x88);
#endif
    
    GC9A01_write_command(COLOR_MODE);
    GC9A01_write_byte(COLOR_MODE__16_BIT);
    
    GC9A01_write_command(0x90);
    GC9A01_write_byte(0x08);
    GC9A01_write_byte(0x08);
    GC9A01_write_byte(0x08);
    GC9A01_write_byte(0x08);
    
    GC9A01_write_command(0xBD);
    GC9A01_write_byte(0x06);
    
    GC9A01_write_command(0xBC);
    GC9A01_write_byte(0x00);
    
    GC9A01_write_command(0xFF);
    GC9A01_write_byte(0x60);
    GC9A01_write_byte(0x01);
    GC9A01_write_byte(0x04);
    
    GC9A01_write_command(0xC3);
    GC9A01_write_byte(0x13);
    GC9A01_write_command(0xC4);
    GC9A01_write_byte(0x13);
    
    GC9A01_write_command(0xC9);
    GC9A01_write_byte(0x22);
    
    GC9A01_write_command(0xBE);
    GC9A01_write_byte(0x11);
    
    GC9A01_write_command(0xE1);
    GC9A01_write_byte(0x10);
    GC9A01_write_byte(0x0E);
    
    GC9A01_write_command(0xDF);
    GC9A01_write_byte(0x21);
    GC9A01_write_byte(0x0c);
    GC9A01_write_byte(0x02);
    
    GC9A01_write_command(0xF0);
    GC9A01_write_byte(0x45);
    GC9A01_write_byte(0x09);
    GC9A01_write_byte(0x08);
    GC9A01_write_byte(0x08);
    GC9A01_write_byte(0x26);
    GC9A01_write_byte(0x2A);
    
    GC9A01_write_command(0xF1);
    GC9A01_write_byte(0x43);
    GC9A01_write_byte(0x70);
    GC9A01_write_byte(0x72);
    GC9A01_write_byte(0x36);
    GC9A01_write_byte(0x37);
    GC9A01_write_byte(0x6F);
    
    GC9A01_write_command(0xF2);
    GC9A01_write_byte(0x45);
    GC9A01_write_byte(0x09);
    GC9A01_write_byte(0x08);
    GC9A01_write_byte(0x08);
    GC9A01_write_byte(0x26);
    GC9A01_write_byte(0x2A);
    
    GC9A01_write_command(0xF3);
    GC9A01_write_byte(0x43);
    GC9A01_write_byte(0x70);
    GC9A01_write_byte(0x72);
    GC9A01_write_byte(0x36);
    GC9A01_write_byte(0x37);
    GC9A01_write_byte(0x6F);
    
    GC9A01_write_command(0xED);
    GC9A01_write_byte(0x1B);
    GC9A01_write_byte(0x0B);
    
    GC9A01_write_command(0xAE);
    GC9A01_write_byte(0x77);
    
    GC9A01_write_command(0xCD);
    GC9A01_write_byte(0x63);
    
    GC9A01_write_command(0x70);
    GC9A01_write_byte(0x07);
    GC9A01_write_byte(0x07);
    GC9A01_write_byte(0x04);
    GC9A01_write_byte(0x0E);
    GC9A01_write_byte(0x0F);
    GC9A01_write_byte(0x09);
    GC9A01_write_byte(0x07);
    GC9A01_write_byte(0x08);
    GC9A01_write_byte(0x03);
    
    GC9A01_write_command(0xE8);
    GC9A01_write_byte(0x34);
    
    GC9A01_write_command(0x62);
    GC9A01_write_byte(0x18);
    GC9A01_write_byte(0x0D);
    GC9A01_write_byte(0x71);
    GC9A01_write_byte(0xED);
    GC9A01_write_byte(0x70);
    GC9A01_write_byte(0x70);
    GC9A01_write_byte(0x18);
    GC9A01_write_byte(0x0F);
    GC9A01_write_byte(0x71);
    GC9A01_write_byte(0xEF);
    GC9A01_write_byte(0x70);
    GC9A01_write_byte(0x70);
    
    GC9A01_write_command(0x63);
    GC9A01_write_byte(0x18);
    GC9A01_write_byte(0x11);
    GC9A01_write_byte(0x71);
    GC9A01_write_byte(0xF1);
    GC9A01_write_byte(0x70);
    GC9A01_write_byte(0x70);
    GC9A01_write_byte(0x18);
    GC9A01_write_byte(0x13);
    GC9A01_write_byte(0x71);
    GC9A01_write_byte(0xF3);
    GC9A01_write_byte(0x70);
    GC9A01_write_byte(0x70);
    
    GC9A01_write_command(0x64);
    GC9A01_write_byte(0x28);
    GC9A01_write_byte(0x29);
    GC9A01_write_byte(0xF1);
    GC9A01_write_byte(0x01);
    GC9A01_write_byte(0xF1);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x07);
    
    GC9A01_write_command(0x66);
    GC9A01_write_byte(0x3C);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0xCD);
    GC9A01_write_byte(0x67);
    GC9A01_write_byte(0x45);
    GC9A01_write_byte(0x45);
    GC9A01_write_byte(0x10);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x00);
    
    GC9A01_write_command(0x67);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x3C);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x01);
    GC9A01_write_byte(0x54);
    GC9A01_write_byte(0x10);
    GC9A01_write_byte(0x32);
    GC9A01_write_byte(0x98);
    
    GC9A01_write_command(0x74);
    GC9A01_write_byte(0x10);
    GC9A01_write_byte(0x85);
    GC9A01_write_byte(0x80);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x4E);
    GC9A01_write_byte(0x00);
    
    GC9A01_write_command(0x98);
    GC9A01_write_byte(0x3e);
    GC9A01_write_byte(0x07);
    
    GC9A01_write_command(0x35);
    GC9A01_write_command(0x21);
    
    GC9A01_write_command(0x11);
    GC9A01_delay(120);
    GC9A01_write_command(0x29);
    GC9A01_delay(20);
    
}

void GC9A01_set_frame(struct GC9A01_frame frame) 
{
    uint8_t data[4];
    
    GC9A01_write_command(COL_ADDR_SET);
    data[0] = (frame.start.X >> 8) & 0xFF;
    data[1] = frame.start.X & 0xFF;
    data[2] = (frame.end.X >> 8) & 0xFF;
    data[3] = frame.end.X & 0xFF;
    GC9A01_write_data(data, sizeof(data));

    GC9A01_write_command(ROW_ADDR_SET);
    data[0] = (frame.start.Y >> 8) & 0xFF;
    data[1] = frame.start.Y & 0xFF;
    data[2] = (frame.end.Y >> 8) & 0xFF;
    data[3] = frame.end.Y & 0xFF;
    GC9A01_write_data(data, sizeof(data));   
}

void GC9A01_write(uint8_t *data, size_t len) 
{
    GC9A01_write_command(MEM_WR);
    GC9A01_write_data(data, len);
}

void GC9A01_write_DMA(uint8_t *data, uint16_t len) 
{
  GC9A01_write_command(MEM_WR);
  GC9A01_set_data_command(1);
  HAL_SPI_Transmit_DMA(&hspi2, data, len);
}

void GC9A01_write_continue(uint8_t *data, size_t len) {
    GC9A01_write_command(MEM_WR_CONT);
    GC9A01_write_data(data, len);
}
