/*

  This is demo for interfacing the Dusplay for the rotary encoder

*/
#include <Arduino_GFX_Library.h>
#include <SPI.h>

//backlight pin, this pin can be run with PWM
#define BACKLIGHT PA0

//Dislpay interface pins
#define MOSI2 PB15
#define SCLK2 PB13
#define SS2   PB12
#define DC    PA8
#define RES   PB14

// configure the interface
Arduino_DataBus *bus = new Arduino_SWSPI(DC /* DC */, SS2 /* CS */, SCLK2 /* SCK */, MOSI2 /* MOSI */, -1 /* MISO */);
// set GFX object
Arduino_GFX *gfx = new Arduino_GC9A01(bus, RES, 0 /* rotation */, true /* IPS */);

void setup(void)
{
    // those pins are outputs
    pinMode(DC, OUTPUT); 
    pinMode(RES, OUTPUT); 
    pinMode(BACKLIGHT, OUTPUT); 
    // power on the back light
    digitalWrite(BACKLIGHT, HIGH);

    // the demo is the usual one for GFX
    gfx->begin();
    gfx->fillScreen(BLACK);
    gfx->setCursor(10, 10);
    gfx->setTextColor(RED);
    gfx->println("Hello World!");

    delay(5000); // 5 seconds
}

void loop()
{
    gfx->setCursor(random(gfx->width()), random(gfx->height()));
    gfx->setTextColor(random(0xffff), random(0xffff));
    gfx->setTextSize(random(6) /* x scale */, random(6) /* y scale */, random(2) /* pixel_margin */);
    gfx->println("Hello World!");

    delay(1000); // 1 second
}
