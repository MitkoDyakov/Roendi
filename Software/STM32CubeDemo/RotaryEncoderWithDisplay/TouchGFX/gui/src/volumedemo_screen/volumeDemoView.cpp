#include <gui/volumedemo_screen/volumeDemoView.hpp>


// volumeMask
// volumeValueText
// background

#define VOL_MAX (-240)
#define VOL_MIN (-480)
#define STEP    (10)
#define NONE    (0u)
#define LEFT    (1u)
#define RIGHT   (2u)

int32_t tmpVol = 0;
extern volatile int32_t volume;
extern volatile uint8_t  updateFromPC;
extern volatile uint8_t  movement;
extern volatile uint32_t steps;

volumeDemoView::volumeDemoView()
{

}

void volumeDemoView::setupScreen()
{
    volumeDemoViewBase::setupScreen();
}

void volumeDemoView::tearDownScreen()
{
    volumeDemoViewBase::tearDownScreen();
}

void volumeDemoView::handleTickEvent()
{
	int16_t  arcEnd   = 0;
	int16_t  arcStart = 0;
	uint16_t arcDiff  = 0;

	if(1 == updateFromPC)
	{
		if(tmpVol != volume)
		{
			volumeMask.setArc(-120, -480 + (volume * 5));
			volumeMask.invalidate();
			background.invalidate();

			Unicode::snprintf(volumeValueTextBuffer, VOLUMEVALUETEXT_SIZE, "%u", volume);
			//Text.setWildcard(TextBuffer);
			volumeValueText.invalidate();
			tmpVol = volume;
		}
	}else{
		if(tmpVol != volume)
		{
			arcEnd   = volumeMask.getArcEnd();
			arcStart = volumeMask.getArcStart();
			arcDiff = (steps * 10);

			if(tmpVol > volume)
			{
				volumeMask.setArc(arcStart - arcDiff, arcEnd - arcDiff);
				Unicode::snprintf(volumeValueTextBuffer, VOLUMEVALUETEXT_SIZE, "-");
			}else{
				volumeMask.setArc(arcStart + arcDiff, arcEnd + arcDiff);
				Unicode::snprintf(volumeValueTextBuffer, VOLUMEVALUETEXT_SIZE, "+");
			}

			volumeMask.invalidate();
			background.invalidate();
			volumeValueText.invalidate();
			tmpVol = volume;
		}
	}
}
