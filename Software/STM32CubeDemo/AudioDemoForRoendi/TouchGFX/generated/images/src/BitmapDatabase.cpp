// 4.18.0 0x1eb86bbe
// Generated by imageconverter. Please, do not edit!

#include <BitmapDatabase.hpp>
#include <touchgfx/Bitmap.hpp>

extern const unsigned char image_pic3[]; // BITMAP_PIC3_ID = 0, Size: 327x302 pixels

const touchgfx::Bitmap::BitmapData bitmap_database[] = {
    { image_pic3, 0, 327, 302, 0, 0, 327, ((uint8_t)touchgfx::Bitmap::RGB565) >> 3, 302, ((uint8_t)touchgfx::Bitmap::RGB565) & 0x7 }
};

namespace BitmapDatabase
{
const touchgfx::Bitmap::BitmapData* getInstance()
{
    return bitmap_database;
}

uint16_t getInstanceSize()
{
    return (uint16_t)(sizeof(bitmap_database) / sizeof(touchgfx::Bitmap::BitmapData));
}
} // namespace BitmapDatabase
