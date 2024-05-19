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

/**
 * @file touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp
 *
 * Declares the touchgfx::CanvasWidgetRenderer class.
 */
#ifndef TOUCHGFX_CANVASWIDGETRENDERER_HPP
#define TOUCHGFX_CANVASWIDGETRENDERER_HPP

#include <touchgfx/canvas_widget_renderer/Cell.hpp>
#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{
/**
 * Class for supporting drawing of figures. This class holds the memory which is used by the
 * underlying algorithms. CanvasWidget will not allocate memory dynamically, but will
 * use memory from the buffer passed to CanvasWidgetRenderer. When using the TouchGFX
 * simulator, it is also possible to get a report on the actual amount of memory used
 * for drawing with CanvasWidgetRenderer to help adjusting the buffer size.
 *
 * @see Widget, setWriteMemoryUsageReport, getWriteMemoryUsageReport
 */
class CanvasWidgetRenderer
{
public:
    /**
     * Setup the buffers used by CanvasWidget.
     *
     * @param [in] buffer     Buffer reserved for CanvasWidget.
     * @param      bufferSize The size of the buffer.
     */
    static void setupBuffer(uint8_t* buffer, unsigned bufferSize);

    /**
     * Removes references to a buffer previously setup using setupBuffer().
     *
     * @see setupBuffer
     */
    static void resetBuffer();

    /**
     * Query if CanvasWidgetRenderer has been initialized with a buffer.
     *
     * @return True if a buffer has been setup.
     */
    static bool hasBuffer();

    /**
     * Gets pointer to memory used for Cell objects in Outline.
     *
     * @return Pointer to memory used internally by Outline.
     */
    static Cell* getOutlineBuffer();

    /**
     * Gets size of memory area used for Cell objects in Outline.
     *
     * @return Size of memory area used internally by Outline.
     */
    static unsigned int getOutlineBufferSize();

#ifdef SIMULATOR
    /**
     * Memory reporting.
     *
     * Memory reporting can be turned on (and off) using this method. CWR will try to work
     * with the given amount of memory passed when calling setupBuffer(). If the outline of
     * the figure is too complex, this will be reported.
     *
     * "CWR requires X bytes" means that X bytes is the highest number of bytes required by
     * CWR so far, but since the size of the invalidated area and the shape of things draw
     * can influence this, this may be reported several times with a higher and higher
     * number. Leave your app running for a long time to find out what the memory
     * requirements are.
     *
     * "CWR requires X bytes (Y bytes missing)" means the same as the report above, but
     * there as was not enough memory to render the entire shape. To get around this, CWR
     * will split the shape into two separate drawings of half size. This means that less
     * memory is required, but drawing will be (somewhat)
     * slower. After you see this message all future draws will be split into smaller chunks,
     * so memory requirements might not get as high. This is followed by:
     *
     * "CWR will split draw into multiple draws due to limited memory." actually just means
     * that CWR will try to work with a smaller amount of memory.
     *
     * In general, if there is enough memory available to run the simulation and never see
     * the message "CWR will split draw ...", this is preferred. The size of the buffer
     * required will be the highest number X reported as "CWR requires X bytes". Good
     * numbers can also be around half of X.
     *
     * @param  writeUsageReport true to write report.
     *
     * @see setupBuffer
     */
    static void setWriteMemoryUsageReport(bool writeUsageReport);

    /**
     * Gets write memory usage report flag.
     *
     * @return true if it CWR writes memory reports, false if not.
     */
    static bool getWriteMemoryUsageReport();

    /// @cond
    /**
     * Called after a shape (Circle, Line, etc.) has been drawn to keep track of the memory
     * requirements of CanvasWidgets.
     *
     * @param  used Number of Cell objects used from the dedicated buffer.
     */
    static void numCellsUsed(unsigned used);

    /**
     * Called after a shape (Circle, Line, etc.) has been drawn to keep track of the memory
     * requirements of CanvasWidgets.
     *
     * @param  missing Number of Cell objects required, but not available.
     */
    static void numCellsMissing(unsigned missing);
    /// @endcond

    /**
     * Calculate how much memory has been required by CanvasWidgets. This can be used to
     * fine tune the size of the buffer passed to CanvasWidgetRenderer upon initialization.
     *
     * @return The number of bytes required.
     */
    static unsigned getUsedBufferSize();

    /**
     * Calculate how much memory was required by CanvasWidgets, but was unavailable. If the
     * value returned is greater than 0 it means the This can be used to fine tune the size
     * of the buffer passed to CanvasWidgetRenderer upon initialization.
     *
     * @return The number of bytes required.
     */
    static unsigned getMissingBufferSize();
#endif

private:
    static Cell* memoryBuffer;
    static unsigned int memoryBufferSize;
#ifdef SIMULATOR
    static unsigned int maxCellsUsed;
    static unsigned int maxCellsMissing;
    static bool writeReport;
#endif
};

} // namespace touchgfx

#endif // TOUCHGFX_CANVASWIDGETRENDERER_HPP
