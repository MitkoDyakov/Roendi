#include <gui/homeschreen_screen/homeSchreenView.hpp>
#include <gui_generated/homeschreen_screen/homeSchreenViewBase.hpp>
#include <touchgfx/Color.hpp>
#include "defs.h"

extern uint8_t userSelection;
extern uint8_t updateScreen;

uint8_t appSelection = 0;

homeSchreenView::homeSchreenView()
{

}

void homeSchreenView::setupScreen()
{
    homeSchreenViewBase::setupScreen();
}

void homeSchreenView::tearDownScreen()
{
    homeSchreenViewBase::tearDownScreen();
}

void homeSchreenView::handleTickEvent()
{
	if(userSelection == appSelection)
	{
		return;
	}

	switch(appSelection)
		{
			case SELECTED_NONE:
			{
				deSelectNone();
				break;
			}
			case SELECTED_BRIGHTNESS:
			{
				deSelectBrightness();
				break;
			}
			case SELECTED_LOCK:
			{
				deSelectLock();
				break;
			}
			case SELECTED_VOLUME:
			{
				deSelectVolumeControl();
				break;
			}
			case SELECTED_TEMP:
			{
				deSelectTempControl();
				break;
			}
			case SELECTED_AUDIO:
			{
				deSelectAudioPlayer();
				break;
			}
			default:break;
		}

	switch(userSelection)
	{
		case SELECTED_NONE:
		{
			selectNone();
			break;
		}
		case SELECTED_BRIGHTNESS:
		{
			selectBrightness();
			break;
		}
		case SELECTED_LOCK:
		{
			selectLock();
			break;
		}
		case SELECTED_VOLUME:
		{
			selectVolumeControl();
			break;
		}
		case SELECTED_TEMP:
		{
			selectTempControl();
			break;
		}
		case SELECTED_AUDIO:
		{
			selectAudioPlayer();
			break;
		}
		default:break;
	}

	appSelection = userSelection;

//	 if(updateScreen)
//	 {
//		 switch(appSelection)
//		 {
//			 case SELECTED_NONE:
//				 application().gotohomeSchreenScreenNoTransition();
//				 break;
//			 case SELECTED_BRIGHTNESS:
//				 application().gotobrightnessDemoScreenNoTransition();
//				 break;
//			 case SELECTED_LOCK:
//				 application().gotoLockScreenNoTransition();
//				 break;
//			 case SELECTED_VOLUME:
//				 application().gotovolumeDemoScreenNoTransition();
//				 break;
//			 case SELECTED_TEMP:
//				 application().gotoTempScreenNoTransition();
//				 break;
//			 case SELECTED_AUDIO:
//				 application().gotoAudioScreenNoTransition();
//				 break;
//			 default:
//				 break;
//		 }
//
//		 updateScreen = false;
//	 }
}


void homeSchreenView::selectNone()
{
	circle1Painter.setColor(touchgfx::Color::getColorFromRGB(161, 83, 0));
	circle1.setPainter(circle1Painter);
	circle1.invalidate();
}


void homeSchreenView::selectBrightness()
{
    circle2Painter.setColor(touchgfx::Color::getColorFromRGB(145, 0, 65));
    circle2.setPainter(circle2Painter);
	circle2.invalidate();
}

void homeSchreenView::selectLock()
{
    circle3Painter.setColor(touchgfx::Color::getColorFromRGB(19, 112, 0));
    circle3.setPainter(circle3Painter);
	circle3.invalidate();
}

void homeSchreenView::selectVolumeControl()
{
    circle4Painter.setColor(touchgfx::Color::getColorFromRGB(0, 11, 161));
    circle4.setPainter(circle4Painter);
	circle4.invalidate();
}

void homeSchreenView::selectTempControl()
{
    circle5Painter.setColor(touchgfx::Color::getColorFromRGB(147, 163, 0));
    circle5.setPainter(circle5Painter);
	circle5.invalidate();
}

void homeSchreenView::selectAudioPlayer()
{
    circle6Painter.setColor(touchgfx::Color::getColorFromRGB(138, 0, 129));
    circle6.setPainter(circle6Painter);
	circle6.invalidate();
}

void homeSchreenView::deSelectNone()
{
	circle1Painter.setColor(touchgfx::Color::getColorFromRGB(255, 153, 43));
	circle1.setPainter(circle1Painter);
	circle1.invalidate();
}


void homeSchreenView::deSelectBrightness()
{
    circle2Painter.setColor(touchgfx::Color::getColorFromRGB(255, 0, 111));
    circle2.setPainter(circle2Painter);
	circle2.invalidate();
}

void homeSchreenView::deSelectLock()
{
    circle3Painter.setColor(touchgfx::Color::getColorFromRGB(30, 181, 0));
    circle3.setPainter(circle3Painter);
	circle3.invalidate();
}

void homeSchreenView::deSelectVolumeControl()
{
    circle4Painter.setColor(touchgfx::Color::getColorFromRGB(0, 12, 235));
    circle4.setPainter(circle4Painter);
	circle4.invalidate();
}

void homeSchreenView::deSelectTempControl()
{
    circle5Painter.setColor(touchgfx::Color::getColorFromRGB(229, 255, 0));
    circle5.setPainter(circle5Painter);
	circle5.invalidate();
}

void homeSchreenView::deSelectAudioPlayer()
{
    circle6Painter.setColor(touchgfx::Color::getColorFromRGB(255, 0, 238));
    circle6.setPainter(circle6Painter);
	circle6.invalidate();
}
