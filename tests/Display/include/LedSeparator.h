#pragma once

#include "LedBase.h"

namespace DisplayTest
{
#define LED_SEPARATOR_NUMBER 2

class LedSeparator : public LedBase
{
public:
	LedSeparator();
	LedSeparator( float sizeLed, float distanceLed, ci::ColorA colorOn, ci::ColorA colorOff );
	~LedSeparator();

	void draw( ci::Vec2f pos );

	float          getWidth();
	float          getHeight();

private:
	ci::Vec2f     _getPosLed  ( int led, ci::Vec2f pos );
	ci::ColorA    _getColorLed( int led );
};

} // namespace DisplayTest