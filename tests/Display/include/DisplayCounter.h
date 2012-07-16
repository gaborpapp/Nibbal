#pragma once

#include "cinder/Color.h"
#include "cinder/Vector.h"
#include "LedCounter.h"

namespace DisplayTest
{

class DisplayCounter : public LedCounter
{
public:
	DisplayCounter();
	DisplayCounter( float sizeBorder, ci::ColorA colorBack, unsigned short countDigit, float distanceDigit, float sizeLed, float distanceLed, ci::ColorA colorOn, ci::ColorA colorOff );

	void  draw( ci::Vec2f pos );

	float getWidth();
	float getHeight();

private:
	float          mSizeBorder;
	ci::ColorA     mColorBack;
};

} // namespace DisplayTest