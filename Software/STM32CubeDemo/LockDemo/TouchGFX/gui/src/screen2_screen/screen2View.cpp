#include <gui/screen2_screen/screen2View.hpp>
#include <gui_generated/screen2_screen/screen2ViewBase.hpp>
#include <touchgfx/Color.hpp>

extern volatile uint8_t buttonPushed;
uint8_t oldButtonPushed3 = 0;

uint8_t check = 0;
uint8_t unclock = 0;

uint8_t digitSel = 3;

uint8_t digit1 = 0;
uint8_t digit2 = 0;
uint8_t digit3 = 0;
uint8_t digit4 = 0;

screen2View::screen2View()
{

}

void screen2View::setupScreen()
{
    screen2ViewBase::setupScreen();
}

void screen2View::tearDownScreen()
{
    screen2ViewBase::tearDownScreen();
}

void screen2View::handleTickEvent()
{
    if(0 != buttonPushed && 0 == unclock)
    {
        if(buttonPushed != oldButtonPushed3)
        {
            digitSel++;

            if(digitSel > 3)
            {
                check = 1;
                digitSel = 0;
            }

            oldButtonPushed3 = buttonPushed;
        }

        switch(digitSel)
        {
            case 0:
                if(check)
                {
                    if(digit1 == 9 && digit2 == 2 && digit3 == 9 && digit4 == 2)
                    {
                        boxWithBorder1.setColor(touchgfx::Color::getColorFromRGB(124, 255, 92));
                        boxWithBorder1_1.setColor(touchgfx::Color::getColorFromRGB(124, 255, 92));
                        boxWithBorder1_2.setColor(touchgfx::Color::getColorFromRGB(124, 255, 92));
                        boxWithBorder1_3.setColor(touchgfx::Color::getColorFromRGB(124, 255, 92));
                        boxWithBorder1.invalidate();
                        boxWithBorder1_1.invalidate();
                        boxWithBorder1_2.invalidate();
                        boxWithBorder1_3.invalidate();
                        unclock = 1;
                    }else{
                        Unicode::snprintf(textArea1Buffer, TEXTAREA1_SIZE, "%u", 0);
                        //Text.setWildcard(TextBuffer);
                        textArea1.invalidate();
                        Unicode::snprintf(textArea1_1Buffer, TEXTAREA1_SIZE, "%u", 0);
                        //Text.setWildcard(TextBuffer);
                        textArea1_1.invalidate();
                        Unicode::snprintf(textArea1_2Buffer, TEXTAREA1_SIZE, "%u", 0);
                        //Text.setWildcard(TextBuffer);
                        textArea1_2.invalidate();
                        Unicode::snprintf(textArea1_3Buffer, TEXTAREA1_SIZE, "%u", 0);
                        //Text.setWildcard(TextBuffer);
                        textArea1_3.invalidate();

                        digit1 = 0;
                        digit2 = 0;
                        digit3 = 0;
                        digit4 = 0;
                    }
                    check = 0;
                }else{
                    digit1++;
                    if(digit1 > 9)
                    {
                        digit1 = 0;
                    }
                    Unicode::snprintf(textArea1Buffer, TEXTAREA1_SIZE, "%u", digit1);
                    //Text.setWildcard(TextBuffer);
                    textArea1.invalidate();
                }


                break;

            case 1:
                digit2++;
                if(digit2 > 9)
                {
                    digit2 = 0;
                }
                Unicode::snprintf(textArea1_1Buffer, TEXTAREA1_SIZE, "%u", digit2);
                //Text.setWildcard(TextBuffer);
                textArea1_1.invalidate();
                break;

            case 2:
                digit3++;
                if(digit3 > 9)
                {
                    digit3 = 0;
                }
                Unicode::snprintf(textArea1_2Buffer, TEXTAREA1_SIZE, "%u", digit3);
                //Text.setWildcard(TextBuffer);
                textArea1_2.invalidate();
                break;

            case 3:
                digit4++;
                if(digit4 > 9)
                {
                    digit4 = 0;
                }
                Unicode::snprintf(textArea1_3Buffer, TEXTAREA1_SIZE, "%u", digit4);
                //Text.setWildcard(TextBuffer);
                textArea1_3.invalidate();
                break;
        }
    }

    buttonPushed = 0;
}
