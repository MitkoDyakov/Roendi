/******************************************************************************
* Copyright (c) 2018(-2024) STMicroelectronics.
* All rights reserved.
*
* This file is part of the TouchGFX 4.23.2 distribution.
*
* This software is licensed under terms that can be found in the LICENSE file in
* the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
*******************************************************************************/

#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/canvas/PainterBWBitmap.hpp>

namespace touchgfx
{
void PainterBWBitmap::setBitmap(const Bitmap& bmp)
{
    AbstractPainterBitmap::setBitmap(bmp);
    assert((bitmap.getId() == BITMAP_INVALID || bitmapFormat == Bitmap::BW || bitmapFormat == Bitmap::BW_RLE) && "PainterBWBitmap only works with BW and BW_RLE bitmaps");
    assert(bitmap.getId() == BITMAP_INVALID || bitmapData);
}

// Found in LCD1bpp
void fillBits(uint8_t* fb, int16_t startX, int16_t startY, int16_t stride, int32_t count, uint8_t color);

void PainterBWBitmap::paint(uint8_t* destination, int16_t offset, int16_t widgetX, int16_t widgetY, int16_t count, uint8_t /*alpha*/) const
{
    if (!adjustBitmapXY(widgetX, widgetY, offset, count))
    {
        return;
    }

    int16_t bitmapAvailable = bitmapRect.width - widgetX;
    if (bitmapFormat == Bitmap::BW_RLE)
    {
        const uint32_t rowSkip = widgetY * bitmapRect.width;
        LCD1bpp::bwRLEdata bw_rle(bitmapData);
        bw_rle.skipNext(widgetX + rowSkip);
        do
        {
            unsigned int length = MIN(bitmapAvailable, count);
            count -= length;
            do
            {
                const uint32_t bw_length = bw_rle.getLength();
                const uint32_t bitsToDraw = MIN(bw_length, length);

                fillBits(destination, offset, 0, 0 /* not used */, bitsToDraw, bw_rle.getColor());
                offset += bitsToDraw;
                length -= bitsToDraw;
                bw_rle.skipNext(bitsToDraw);
            } while (length);
            bw_rle.init(bitmapData);
            bw_rle.skipNext(rowSkip);
            bitmapAvailable = bitmapRect.width;
        } while (count);
    }
    else
    {
        const int32_t rowSkip = widgetY * ((bitmapRect.width + 7) / 8);
        const uint8_t* bitmapPointer = bitmapData + rowSkip;
        do
        {
            const int16_t length = MIN(bitmapAvailable, count);

            const uint8_t* src = bitmapPointer + widgetX / 8;
            uint8_t* RESTRICT dst = destination + (offset / 8);
            uint16_t srcBitX = widgetX % 8;      // & 7
            const uint16_t dstBitX = offset % 8; // & 7

            uint16_t remainingBits = length;

            if (dstBitX > 0)
            {
                // Start by getting (dst-)aligned for faster transfer
                uint16_t neededBits = 8 - dstBitX;
                if (neededBits > remainingBits)
                {
                    neededBits = remainingBits; // Very narrow src inside same word
                }
                const uint16_t bitmapAvailableBits = 8 - srcBitX;
                uint8_t mask = (1u << neededBits) - 1u;
                const uint8_t dstShift = static_cast<uint8_t>(8u - (dstBitX + neededBits));
                mask <<= dstShift;

                uint8_t word = *src;

                if (bitmapAvailableBits > neededBits)
                {
                    word >>= bitmapAvailableBits - neededBits;
                }
                else if (bitmapAvailableBits < neededBits)
                {
                    // Get the last required bits from src[1]
                    word <<= neededBits - bitmapAvailableBits;
                    word |= src[1] >> (8u - (neededBits - bitmapAvailableBits));
                }

                word <<= dstShift;
                *dst = (*dst & ~mask) | (word & mask);

                srcBitX = (srcBitX + neededBits) % 8; // & 7

                if (bitmapAvailableBits <= neededBits)
                {
                    src++;
                }
                dst++;
                remainingBits -= neededBits;
            }

            // dstX is now word aligned (or we have transferred everything of a narrow image and remainingBits==0)
            if (remainingBits >= 8)
            {
                const uint16_t bytesPerLine = remainingBits / 8;
                if (srcBitX == 0)
                {
                    HAL::getInstance()->blockCopy(dst, src, bytesPerLine);
                    src += bytesPerLine;
                    dst += bytesPerLine;
                }
                else
                {
                    const uint16_t _remainingBits = remainingBits;

                    remainingBits = _remainingBits;
                    while (remainingBits >= 8)
                    {
                        uint8_t word = *src++;
                        word <<= srcBitX;
                        word |= (*src) >> (8 - srcBitX);
                        *dst++ = word;
                        remainingBits -= 8;
                    }
                }
                remainingBits %= 8; // &= 7
            }

            // Take the last bits, again we need to mask dst
            if (remainingBits > 0)
            {
                uint8_t word = *src;
                if (srcBitX != 0)
                {
                    word <<= srcBitX;
                    word |= src[1] >> (8u - srcBitX);
                }
                const uint8_t mask = ((1u << remainingBits) - 1u) << (8u - remainingBits);
                *dst = (*dst & ~mask) | (word & mask);
            }

            offset += length;
            count -= length;
            widgetX = 0;
            bitmapAvailable = bitmapRect.width;
        } while (count);
    }
}
} // namespace touchgfx
