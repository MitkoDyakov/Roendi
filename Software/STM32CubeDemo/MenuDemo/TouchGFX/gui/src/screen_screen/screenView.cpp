#include <gui/screen_screen/screenView.hpp>
#include <touchgfx/Color.hpp>
#include <BitmapDatabase.hpp>


extern volatile uint8_t buttonPushed;
uint8_t oldButtonPushed3 = 0;
uint8_t selectedIcon = 0;
uint8_t deSelectedIcon = 0;

uint8_t screenSelected    = 0;
uint8_t oldScreenSelected = 0;

Bitmap a;

screenView::screenView()
{

}

void screenView::setupScreen()
{
    screenViewBase::setupScreen();
}

void screenView::tearDownScreen()
{
    screenViewBase::tearDownScreen();
}

void screenView::handleTickEvent()
{
    if(0 != buttonPushed)
    {
        if(1 == buttonPushed)
        {
            if(0 == selectedIcon)
            {
                selectedIcon = 3;
            }else{

                selectedIcon--;
            }
        }

        if(2 == buttonPushed)
        {
            selectedIcon++;
            if(selectedIcon > 3)
            {
                selectedIcon = 0;
            }
        }

        updateSelectionPointer();
        updateCenterIcon();
    }

    buttonPushed = 0;

    if(screenSelected != oldScreenSelected)
    {
        if(screenSelected == 0)
        {
            cancelText.setVisible(false);
            cancelText.invalidate();
            selectText.setVisible(true);
            selectText.invalidate();
        }else{
            cancelText.setVisible(true);
            cancelText.invalidate();
            selectText.setVisible(false);
            selectText.invalidate();
        }

        selectedIcon = 0;
        oldScreenSelected = screenSelected;
        updateSelectionPointer();
        updateIcons();
    }
}

void screenView::updateIcons()
{
    switch(screenSelected)
    {
        case 0:{
            setScreen0();
            break;
        }
        case 1:{
            setScreen1();
            break;
        }
        case 2:
        {
            setScreen2();
            break;
        }
        case 3:
        {
            setScreen3();
            break;
        }

    }
}

void screenView::updateCenterIcon()
{
    switch(screenSelected)
    {
        case 0:{
            setCenterIconScreen0();
            break;
        }
        case 1:{
            setCenterIconScreen1();
            break;
        }
        case 2:
        {
            setCenterIconScreen2();
            break;
        }
        case 3:
        {
            setCenterIconScreen3();
            break;
        }

    }
}

void screenView::updateSelectionPointer(void)
{
    switch(deSelectedIcon)
    {
        case 0:{
            deSelectTop();
            break;
        }
        case 1:{
            deSelectRight();
            break;
        }
        case 2:{
            deSelectBottom();
            break;
        }
        case 3:{
            deSelectLeft();
            break;
        }
    }

    switch(selectedIcon)
    {
        case 0:{
            selectTop();
            break;
        }
        case 1:{
            selectRight();
            break;
        }
        case 2:{
            selectBottom();
            break;
        }
        case 3:{
            selectLeft();
            break;
        }
    }

    deSelectedIcon = selectedIcon;
}

void screenView::selectTop()
{
    selectorPainter.setColor(touchgfx::Color::getColorFromRGB(46, 46, 46));
    selector.setPainter(selectorPainter);
    selector.invalidate();
}

void screenView::selectLeft()
{
    selector_3Painter.setColor(touchgfx::Color::getColorFromRGB(46, 46, 46));
    selector_3.setPainter(selector_3Painter);
    selector_3.invalidate();
}

void screenView::selectRight()
{
    selector_1Painter.setColor(touchgfx::Color::getColorFromRGB(46, 46, 46));
    selector_1.setPainter(selector_1Painter);
    selector_1.invalidate();
}

void screenView::selectBottom()
{
    selector_2Painter.setColor(touchgfx::Color::getColorFromRGB(46, 46, 46));
    selector_2.setPainter(selector_2Painter);
    selector_2.invalidate();
}

void screenView::deSelectTop()
{
    selectorPainter.setColor(touchgfx::Color::getColorFromRGB(101, 104, 114));
    selector.setPainter(selectorPainter);
    selector.invalidate();
}

void screenView::deSelectLeft()
{
    selector_3Painter.setColor(touchgfx::Color::getColorFromRGB(101, 104, 114));
    selector_3.setPainter(selector_3Painter);
    selector_3.invalidate();
}

void screenView::deSelectRight()
{
    selector_1Painter.setColor(touchgfx::Color::getColorFromRGB(101, 104, 114));
    selector_1.setPainter(selector_1Painter);
    selector_1.invalidate();
}

void screenView::deSelectBottom()
{
    selector_2Painter.setColor(touchgfx::Color::getColorFromRGB(101, 104, 114));
    selector_2.setPainter(selector_2Painter);
    selector_2.invalidate();
}

void screenView::setCenterIconScreen0()
{
    image4.setXY(85, 85);

    switch(selectedIcon)
    {
        case 0:{
            break;
        }
        case 1:{

            break;
        }
        case 2:{

            break;
        }
        case 3:{

            break;
        }
    }

    image4.invalidate();
}

void screenView::setCenterIconScreen1()
{
    image4.setXY(85, 85);

    switch(selectedIcon)
    {
        case 0:{
            image4.setBitmap(touchgfx::Bitmap(a));
            image4.setVisible(true);
            break;
        }
        case 1:{
            image4.setBitmap(touchgfx::Bitmap(BITMAP_PRINTER_GRAY_ID));
            image4.setVisible(true);
            break;
        }
        case 2:{
            image4.setBitmap(touchgfx::Bitmap(BITMAP_TRIANGLE_RULER_GRAY_ID));
            break;
        }
        case 3:{
            image4.setBitmap(touchgfx::Bitmap(BITMAP_SPRAY_PAINT_GRAY_ID));
            image4.setVisible(true);
            break;
        }
    }

    image4.invalidate();
}

void screenView::setCenterIconScreen2()
{
    image4.setXY(85, 85);

    switch(selectedIcon)
    {
        case 0:{
            image4.setBitmap(touchgfx::Bitmap(a));
            image4.setVisible(true);
            break;
        }
        case 1:{
            image4.setBitmap(touchgfx::Bitmap(BITMAP_LOGO_DESIGN_GRAY_ID));
            image4.setVisible(true);
            break;
        }
        case 2:{
            image4.setBitmap(touchgfx::Bitmap(BITMAP_PEN_TOOL_GRAY_ID));
            image4.setVisible(true);
            break;
        }
        case 3:{
            image4.setBitmap(touchgfx::Bitmap(BITMAP_CUBE_GRAY_ID));
            image4.setVisible(true);
            break;
        }
    }

    image4.invalidate();
}

void screenView::setCenterIconScreen3()
{
    image4.setXY(85, 85);

    switch(selectedIcon)
    {
        case 0:{
            image4.setBitmap(touchgfx::Bitmap(a));
            image4.setVisible(true);
            break;
        }
        case 1:{
            image4.setBitmap(touchgfx::Bitmap(BITMAP_EYE_DROPPER_GRAY_ID));
            image4.setVisible(true);
            break;
        }
        case 2:{
            image4.setBitmap(touchgfx::Bitmap(BITMAP_RULER_GRAY_ID));
            image4.setVisible(true);
            break;
        }
        case 3:{
            image4.setBitmap(touchgfx::Bitmap(BITMAP_ERASER_GRAY_ID));
            image4.setVisible(true);
            break;
        }
    }

    image4.invalidate();
}

void screenView::setScreen0()
{
    pirplePainter.setColor(touchgfx::Color::getColorFromRGB(189, 126, 216));
    pirple.setPainter(pirplePainter);
    pirple.invalidate();

    greenPainter.setColor(touchgfx::Color::getColorFromRGB(128, 194, 184));
    green.setPainter(greenPainter);
    green.invalidate();

    redPainter.setColor(touchgfx::Color::getColorFromRGB(237, 113, 97));
    red.setPainter(redPainter);
    red.invalidate();

    image1.setBitmap(touchgfx::Bitmap(BITMAP_ERASER_ID));
    image1.invalidate();

    image2.setBitmap(touchgfx::Bitmap(BITMAP_PRINTER_ID));
    image2.invalidate();

    image3.setBitmap(touchgfx::Bitmap(BITMAP_PEN_TOOL_ID));
    image3.invalidate();

     a = image4.getBitmap();
}

void screenView::setScreen1()
{
    pirplePainter.setColor(touchgfx::Color::getColorFromRGB(128, 194, 184));
    pirple.setPainter(pirplePainter);
    pirple.invalidate();

    greenPainter.setColor(touchgfx::Color::getColorFromRGB(128, 194, 184));
    green.setPainter(greenPainter);
    green.invalidate();

    redPainter.setColor(touchgfx::Color::getColorFromRGB(128, 194, 184));
    red.setPainter(redPainter);
    red.invalidate();

    image1.setBitmap(touchgfx::Bitmap(BITMAP_SPRAY_PAINT_ID));
    image1.invalidate();

    image2.setBitmap(touchgfx::Bitmap(BITMAP_PRINTER_ID));
    image2.invalidate();

    image3.setBitmap(touchgfx::Bitmap(BITMAP_TRIANGLE_RULER_ID));
    image3.invalidate();
}

void screenView::setScreen2()
{//L R B
    pirplePainter.setColor(touchgfx::Color::getColorFromRGB(237, 113, 97));
    pirple.setPainter(pirplePainter);
    pirple.invalidate();

    greenPainter.setColor(touchgfx::Color::getColorFromRGB(237, 113, 97));
    green.setPainter(greenPainter);
    green.invalidate();

    redPainter.setColor(touchgfx::Color::getColorFromRGB(237, 113, 97));
    red.setPainter(redPainter);
    red.invalidate();

    image1.setBitmap(touchgfx::Bitmap(BITMAP_CUBE_ID));
    image1.invalidate();

    image2.setBitmap(touchgfx::Bitmap(BITMAP_LOGO_DESIGN_ID));
    image2.invalidate();

    image3.setBitmap(touchgfx::Bitmap(BITMAP_PEN_TOOL_ID));
    image3.invalidate();
}

void screenView::setScreen3()
{
    pirplePainter.setColor(touchgfx::Color::getColorFromRGB(189, 126, 216));
    pirple.setPainter(pirplePainter);
    pirple.invalidate();

    greenPainter.setColor(touchgfx::Color::getColorFromRGB(189, 126, 216));
    green.setPainter(greenPainter);
    green.invalidate();

    redPainter.setColor(touchgfx::Color::getColorFromRGB(189, 126, 216));
    red.setPainter(redPainter);
    red.invalidate();

    image1.setBitmap(touchgfx::Bitmap(BITMAP_ERASER_ID));
    image1.invalidate();

    image2.setBitmap(touchgfx::Bitmap(BITMAP_EYE_DROPPER_ID));
    image2.invalidate();

    image3.setBitmap(touchgfx::Bitmap(BITMAP_RULER_ID));
    image3.invalidate();
}
