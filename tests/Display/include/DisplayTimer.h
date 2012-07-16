#pragma once

#include "cinder/Color.h"
#include "cinder/Vector.h"
#include "LedTimer.h"

namespace DisplayTest
{

class DisplayTimer : public LedTimer
{
public:
	DisplayTimer();
	DisplayTimer( float sizeBorder, ci::ColorA colorBack, float distanceDigit, float distanceSeparator, float sizeLed, float distanceLed, ci::ColorA colorOn, ci::ColorA colorOff );

	void  draw( ci::Vec2f pos );

	float getWidth();
	float getHeight();

private:
	float          mSizeBorder;
	ci::ColorA     mColorBack;
};

} // namespace DisplayTest