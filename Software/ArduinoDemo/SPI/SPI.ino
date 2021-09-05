/*

  This is demo for interfacing with SPI for the rotary encoder

*/

#include <SPI.h>

#define MOSI2 PB15
#define MISO2 PB14
#define SCLK2 PB13
#define SS2   PB12

//SPIClass(uint8_t mosi, uint8_t miso, uint8_t sclk, uint8_t ssel)
SPIClass SPI_2 = SPIClass(MOSI2, MISO2, SCLK2, SS2);

void setup() 
{
  // put your setup code here, to run once:  
  SPI_2.begin();
}

void loop() 
{
  // put your main code here, to run repeatedly:
  SPI_2.transfer(0xb4); //Transfers data to the second device.
  delay(1000);
}
