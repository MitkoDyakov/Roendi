#ifndef AUDIOPRESENTER_HPP
#define AUDIOPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class AudioView;

class AudioPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    AudioPresenter(AudioView& v);

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

    virtual ~AudioPresenter() {};

private:
    AudioPresenter();

    AudioView& view;
};

#endif // AUDIOPRESENTER_HPP
