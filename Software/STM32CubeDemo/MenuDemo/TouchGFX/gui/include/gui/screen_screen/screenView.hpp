#ifndef SCREENVIEW_HPP
#define SCREENVIEW_HPP

#include <gui_generated/screen_screen/screenViewBase.hpp>
#include <gui/screen_screen/screenPresenter.hpp>

class screenView : public screenViewBase
{
public:
    screenView();
    virtual ~screenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
    virtual void deSelectLeft();
    virtual void deSelectTop();
    virtual void deSelectBottom();
    virtual void deSelectRight();
    virtual void selectLeft();
    virtual void selectTop();
    virtual void selectBottom();
    virtual void selectRight();
    virtual void setCenterIconScreen0();
    virtual void setCenterIconScreen1();
    virtual void setCenterIconScreen2();
    virtual void setCenterIconScreen3();
    virtual void updateSelectionPointer();
    virtual void setScreen0();
    virtual void setScreen1();
    virtual void setScreen2();
    virtual void setScreen3();
    virtual void updateIcons();
    virtual void updateCenterIcon();
protected:
};

#endif // SCREENVIEW_HPP
