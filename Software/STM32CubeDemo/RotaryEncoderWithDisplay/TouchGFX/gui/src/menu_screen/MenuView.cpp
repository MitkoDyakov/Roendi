#include <gui/menu_screen/MenuView.hpp>
#include <gui_generated/menu_screen/MenuViewBase.hpp>
#include <touchgfx/Color.hpp>

#include "defs.h"

extern uint8_t userSelection;

uint8_t appSelection = 0;


MenuView::MenuView()
{

}

void MenuView::setupScreen()
{
    MenuViewBase::setupScreen();
}

void MenuView::tearDownScreen()
{
    MenuViewBase::tearDownScreen();
}

void MenuView::handleTickEvent()
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

//   if(updateScreen)
//   {
//       switch(appSelection)
//       {
//           case SELECTED_NONE:
//               application().gotohomeSchreenScreenNoTransition();
//               break;
//           case SELECTED_BRIGHTNESS:
//               application().gotobrightnessDemoScreenNoTransition();
//               break;
//           case SELECTED_LOCK:
//               application().gotoLockScreenNoTransition();
//               break;
//           case SELECTED_VOLUME:
//               application().gotovolumeDemoScreenNoTransition();
//               break;
//           case SELECTED_TEMP:
//               application().gotoTempScreenNoTransition();
//               break;
//           case SELECTED_AUDIO:
//               application().gotoAudioScreenNoTransition();
//               break;
//           default:
//               break;
//       }
//
//       updateScreen = false;
//   }
}


void MenuView::selectNone()
{
    selectPainter.setColor(touchgfx::Color::getColorFromRGB(161, 83, 0));
    select.setPainter(selectPainter);
    select.invalidate();
}


void MenuView::selectBrightness()
{
    backlightPainter.setColor(touchgfx::Color::getColorFromRGB(145, 0, 65));
    backlight.setPainter(backlightPainter);
    backlight.invalidate();
}

void MenuView::selectLock()
{
    lockPainter.setColor(touchgfx::Color::getColorFromRGB(19, 112, 0));
    lock.setPainter(lockPainter);
    lock.invalidate();
}

void MenuView::selectVolumeControl()
{
    volumePainter.setColor(touchgfx::Color::getColorFromRGB(0, 11, 161));
    volume.setPainter(volumePainter);
    volume.invalidate();
}

void MenuView::selectTempControl()
{
    tempPainter.setColor(touchgfx::Color::getColorFromRGB(147, 163, 0));
    temp.setPainter(tempPainter);
    temp.invalidate();
}

void MenuView::selectAudioPlayer()
{
    musicPainter.setColor(touchgfx::Color::getColorFromRGB(138, 0, 129));
    music.setPainter(musicPainter);
    music.invalidate();
}

void MenuView::deSelectNone()
{
    selectPainter.setColor(touchgfx::Color::getColorFromRGB(255, 153, 43));
    select.setPainter(selectPainter);
    select.invalidate();
}


void MenuView::deSelectBrightness()
{
    backlightPainter.setColor(touchgfx::Color::getColorFromRGB(255, 0, 111));
    backlight.setPainter(backlightPainter);
    backlight.invalidate();
}

void MenuView::deSelectLock()
{
    lockPainter.setColor(touchgfx::Color::getColorFromRGB(30, 181, 0));
    lock.setPainter(lockPainter);
    lock.invalidate();
}

void MenuView::deSelectVolumeControl()
{
    volumePainter.setColor(touchgfx::Color::getColorFromRGB(0, 12, 235));
    volume.setPainter(volumePainter);
    volume.invalidate();
}

void MenuView::deSelectTempControl()
{
    tempPainter.setColor(touchgfx::Color::getColorFromRGB(229, 255, 0));
    temp.setPainter(tempPainter);
    temp.invalidate();
}

void MenuView::deSelectAudioPlayer()
{
    musicPainter.setColor(touchgfx::Color::getColorFromRGB(255, 0, 238));
    music.setPainter(musicPainter);
    music.invalidate();
}
