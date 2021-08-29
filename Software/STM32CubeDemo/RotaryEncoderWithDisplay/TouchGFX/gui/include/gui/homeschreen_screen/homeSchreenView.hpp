#ifndef HOMESCHREENVIEW_HPP
#define HOMESCHREENVIEW_HPP

#include <gui_generated/homeschreen_screen/homeSchreenViewBase.hpp>
#include <gui/homeschreen_screen/homeSchreenPresenter.hpp>

class homeSchreenView : public homeSchreenViewBase
{
public:
    homeSchreenView();
    virtual ~homeSchreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    virtual void selectNone();
    virtual void selectBrightness();
    virtual void selectLock();
    virtual void selectVolumeControl();
    virtual void selectTempControl();
    virtual void selectAudioPlayer();

    virtual void deSelectNone();
    virtual void deSelectBrightness();
    virtual void deSelectLock();
    virtual void deSelectVolumeControl();
    virtual void deSelectTempControl();
    virtual void deSelectAudioPlayer();
protected:
};

#endif // HOMESCHREENVIEW_HPP
