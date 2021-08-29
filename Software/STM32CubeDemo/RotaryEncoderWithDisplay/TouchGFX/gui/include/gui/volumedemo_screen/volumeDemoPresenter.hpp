#ifndef VOLUMEDEMOPRESENTER_HPP
#define VOLUMEDEMOPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class volumeDemoView;

class volumeDemoPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    volumeDemoPresenter(volumeDemoView& v);

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

    virtual ~volumeDemoPresenter() {};

private:
    volumeDemoPresenter();

    volumeDemoView& view;
};

#endif // VOLUMEDEMOPRESENTER_HPP
