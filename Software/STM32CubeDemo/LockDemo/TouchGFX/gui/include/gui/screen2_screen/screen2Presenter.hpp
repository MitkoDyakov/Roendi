#ifndef SCREEN2PRESENTER_HPP
#define SCREEN2PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class screen2View;

class screen2Presenter : public touchgfx::Presenter, public ModelListener
{
public:
    screen2Presenter(screen2View& v);

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

    virtual ~screen2Presenter() {};

private:
    screen2Presenter();

    screen2View& view;
};

#endif // SCREEN2PRESENTER_HPP
