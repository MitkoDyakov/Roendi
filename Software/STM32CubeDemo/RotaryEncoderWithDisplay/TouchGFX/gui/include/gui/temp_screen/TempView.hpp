#ifndef TEMPVIEW_HPP
#define TEMPVIEW_HPP

#include <gui_generated/temp_screen/TempViewBase.hpp>
#include <gui/temp_screen/TempPresenter.hpp>

class TempView : public TempViewBase
{
public:
    TempView();
    virtual ~TempView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // TEMPVIEW_HPP
