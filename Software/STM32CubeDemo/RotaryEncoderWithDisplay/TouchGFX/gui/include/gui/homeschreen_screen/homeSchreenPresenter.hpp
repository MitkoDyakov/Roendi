#ifndef HOMESCHREENPRESENTER_HPP
#define HOMESCHREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class homeSchreenView;

class homeSchreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    homeSchreenPresenter(homeSchreenView& v);

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

    virtual ~homeSchreenPresenter() {};

private:
    homeSchreenPresenter();

    homeSchreenView& view;
};

#endif // HOMESCHREENPRESENTER_HPP
