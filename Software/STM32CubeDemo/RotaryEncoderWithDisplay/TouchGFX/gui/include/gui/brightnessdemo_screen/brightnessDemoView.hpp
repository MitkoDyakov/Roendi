#ifndef BRIGHTNESSDEMOVIEW_HPP
#define BRIGHTNESSDEMOVIEW_HPP

#include <gui_generated/brightnessdemo_screen/brightnessDemoViewBase.hpp>
#include <gui/brightnessdemo_screen/brightnessDemoPresenter.hpp>

class brightnessDemoView : public brightnessDemoViewBase
{
public:
    brightnessDemoView();
    virtual ~brightnessDemoView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // BRIGHTNESSDEMOVIEW_HPP
