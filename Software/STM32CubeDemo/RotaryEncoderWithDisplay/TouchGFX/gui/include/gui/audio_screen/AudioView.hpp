#ifndef AUDIOVIEW_HPP
#define AUDIOVIEW_HPP

#include <gui_generated/audio_screen/AudioViewBase.hpp>
#include <gui/audio_screen/AudioPresenter.hpp>

class AudioView : public AudioViewBase
{
public:
    AudioView();
    virtual ~AudioView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // AUDIOVIEW_HPP
