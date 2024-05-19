#ifndef MENUVIEW_HPP
#define MENUVIEW_HPP

#include <gui_generated/menu_screen/MenuViewBase.hpp>
#include <gui/menu_screen/MenuPresenter.hpp>

class MenuView : public MenuViewBase
{
public:
    MenuView();
    virtual ~MenuView() {}
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

#endif // MENUVIEW_HPP
