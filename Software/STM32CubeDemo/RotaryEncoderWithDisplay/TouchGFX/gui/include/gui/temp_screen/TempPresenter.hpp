#ifndef TEMPPRESENTER_HPP
#define TEMPPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class TempView;

class TempPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    TempPresenter(TempView& v);

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

    virtual ~TempPresenter() {};

private:
    TempPresenter();

    TempView& view;
};

#endif // TEMPPRESENTER_HPP
