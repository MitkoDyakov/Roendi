/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <new>
#include <gui_generated/common/FrontendApplicationBase.hpp>
#include <gui/common/FrontendHeap.hpp>
#include <touchgfx/transitions/NoTransition.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Texts.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <gui/homeschreen_screen/homeSchreenView.hpp>
#include <gui/homeschreen_screen/homeSchreenPresenter.hpp>
#include <gui/volumedemo_screen/volumeDemoView.hpp>
#include <gui/volumedemo_screen/volumeDemoPresenter.hpp>
#include <gui/brightnessdemo_screen/brightnessDemoView.hpp>
#include <gui/brightnessdemo_screen/brightnessDemoPresenter.hpp>
#include <gui/lock_screen/LockView.hpp>
#include <gui/lock_screen/LockPresenter.hpp>
#include <gui/temp_screen/TempView.hpp>
#include <gui/temp_screen/TempPresenter.hpp>
#include <gui/audio_screen/AudioView.hpp>
#include <gui/audio_screen/AudioPresenter.hpp>

using namespace touchgfx;

FrontendApplicationBase::FrontendApplicationBase(Model& m, FrontendHeap& heap)
    : touchgfx::MVPApplication(),
      transitionCallback(),
      frontendHeap(heap),
      model(m)
{
    touchgfx::HAL::getInstance()->setDisplayOrientation(touchgfx::ORIENTATION_LANDSCAPE);
    touchgfx::Texts::setLanguage(GB);
}

/*
 * Screen Transition Declarations
 */

// homeSchreen

void FrontendApplicationBase::gotohomeSchreenScreenNoTransition()
{
    transitionCallback = touchgfx::Callback<FrontendApplicationBase>(this, &FrontendApplication::gotohomeSchreenScreenNoTransitionImpl);
    pendingScreenTransitionCallback = &transitionCallback;
}

void FrontendApplicationBase::gotohomeSchreenScreenNoTransitionImpl()
{
    touchgfx::makeTransition<homeSchreenView, homeSchreenPresenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}

// volumeDemo

void FrontendApplicationBase::gotovolumeDemoScreenNoTransition()
{
    transitionCallback = touchgfx::Callback<FrontendApplicationBase>(this, &FrontendApplication::gotovolumeDemoScreenNoTransitionImpl);
    pendingScreenTransitionCallback = &transitionCallback;
}

void FrontendApplicationBase::gotovolumeDemoScreenNoTransitionImpl()
{
    touchgfx::makeTransition<volumeDemoView, volumeDemoPresenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}

// brightnessDemo

void FrontendApplicationBase::gotobrightnessDemoScreenNoTransition()
{
    transitionCallback = touchgfx::Callback<FrontendApplicationBase>(this, &FrontendApplication::gotobrightnessDemoScreenNoTransitionImpl);
    pendingScreenTransitionCallback = &transitionCallback;
}

void FrontendApplicationBase::gotobrightnessDemoScreenNoTransitionImpl()
{
    touchgfx::makeTransition<brightnessDemoView, brightnessDemoPresenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}

// Lock

void FrontendApplicationBase::gotoLockScreenNoTransition()
{
    transitionCallback = touchgfx::Callback<FrontendApplicationBase>(this, &FrontendApplication::gotoLockScreenNoTransitionImpl);
    pendingScreenTransitionCallback = &transitionCallback;
}

void FrontendApplicationBase::gotoLockScreenNoTransitionImpl()
{
    touchgfx::makeTransition<LockView, LockPresenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}

// Temp

void FrontendApplicationBase::gotoTempScreenNoTransition()
{
    transitionCallback = touchgfx::Callback<FrontendApplicationBase>(this, &FrontendApplication::gotoTempScreenNoTransitionImpl);
    pendingScreenTransitionCallback = &transitionCallback;
}

void FrontendApplicationBase::gotoTempScreenNoTransitionImpl()
{
    touchgfx::makeTransition<TempView, TempPresenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}

// Audio

void FrontendApplicationBase::gotoAudioScreenNoTransition()
{
    transitionCallback = touchgfx::Callback<FrontendApplicationBase>(this, &FrontendApplication::gotoAudioScreenNoTransitionImpl);
    pendingScreenTransitionCallback = &transitionCallback;
}

void FrontendApplicationBase::gotoAudioScreenNoTransitionImpl()
{
    touchgfx::makeTransition<AudioView, AudioPresenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}
