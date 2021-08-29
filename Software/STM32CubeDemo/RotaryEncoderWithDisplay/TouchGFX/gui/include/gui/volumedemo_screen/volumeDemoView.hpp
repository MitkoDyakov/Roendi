#ifndef VOLUMEDEMOVIEW_HPP
#define VOLUMEDEMOVIEW_HPP

#include <gui_generated/volumedemo_screen/volumeDemoViewBase.hpp>
#include <gui/volumedemo_screen/volumeDemoPresenter.hpp>

class volumeDemoView : public volumeDemoViewBase
{
public:
    volumeDemoView();
    virtual ~volumeDemoView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // VOLUMEDEMOVIEW_HPP
