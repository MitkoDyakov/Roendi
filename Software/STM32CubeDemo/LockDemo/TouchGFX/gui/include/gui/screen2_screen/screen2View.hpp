#ifndef SCREEN2VIEW_HPP
#define SCREEN2VIEW_HPP

#include <gui_generated/screen2_screen/screen2ViewBase.hpp>
#include <gui/screen2_screen/screen2Presenter.hpp>

class screen2View : public screen2ViewBase
{
public:
    screen2View();
    virtual ~screen2View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
protected:
};

#endif // SCREEN2VIEW_HPP
