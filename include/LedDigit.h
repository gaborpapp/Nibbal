#pragma once

#include "LedBase.h"

namespace Nibbal
{
#define LED_DIGIT_NUMBER 20

class LedDigit : public LedBase
{
private:
	static const int mNumberMask[10][LED_DIGIT_NUMBER];

public:
	LedDigit();
	LedDigit( float sizeLed, float distanceLed, ci::ColorA colorOn, ci::ColorA colorOff );
	~LedDigit();

	void draw( ci::Vec2f pos );

	float          getWidth();
	float          getHeight();

	void           setNumber( unsigned short number );
	unsigned short getNumber();

	void           setShowNull( bool showNull );
	bool           getShowNull();

private:
	ci::Vec2f     _getPosLed  ( int led, ci::Vec2f pos );
	ci::ColorA    _getColorLed( int led );

private:
	unsigned short mNumber;
	bool           mShowNull;
};

} // namespace Nibbal
