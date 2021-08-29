#ifndef LOCKPRESENTER_HPP
#define LOCKPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class LockView;

class LockPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    LockPresenter(LockView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~LockPresenter() {};

private:
    LockPresenter();

    LockView& view;
};

#endif // LOCKPRESENTER_HPP
