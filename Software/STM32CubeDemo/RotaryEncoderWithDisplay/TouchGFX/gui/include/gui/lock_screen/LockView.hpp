#ifndef LOCKVIEW_HPP
#define LOCKVIEW_HPP

#include <gui_generated/lock_screen/LockViewBase.hpp>
#include <gui/lock_screen/LockPresenter.hpp>

class LockView : public LockViewBase
{
public:
    LockView();
    virtual ~LockView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // LOCKVIEW_HPP
