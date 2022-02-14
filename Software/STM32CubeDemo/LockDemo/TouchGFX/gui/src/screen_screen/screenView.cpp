#include <gui/screen_screen/screenView.hpp>
#include <gui_generated/screen_screen/screenViewBase.hpp>

extern volatile uint8_t buttonPushed;
uint8_t oldButtonPushed1 = 0;

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
    if(buttonPushed != oldButtonPushed1)
    {
        screenViewBase::handleKeyEvent(buttonPushed);
        oldButtonPushed1 = buttonPushed;
        buttonPushed = 0;
    }
}
