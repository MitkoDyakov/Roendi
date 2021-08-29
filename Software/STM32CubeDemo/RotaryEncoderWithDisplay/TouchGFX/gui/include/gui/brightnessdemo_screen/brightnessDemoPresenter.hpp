#ifndef BRIGHTNESSDEMOPRESENTER_HPP
#define BRIGHTNESSDEMOPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class brightnessDemoView;

class brightnessDemoPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    brightnessDemoPresenter(brightnessDemoView& v);

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

    virtual ~brightnessDemoPresenter() {};

private:
    brightnessDemoPresenter();

    brightnessDemoView& view;
};

#endif // BRIGHTNESSDEMOPRESENTER_HPP
