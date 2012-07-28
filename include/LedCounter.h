#pragma once

#include <vector>
#include "cinder/Color.h"
#include "cinder/Vector.h"
#include "LedDigit.h"

namespace Nibbal
{

class LedCounter
{
public:
	LedCounter();
	LedCounter( unsigned short countDigit, float distanceDigit, float sizeLed, float distanceLed, ci::ColorA colorOn, ci::ColorA colorOff );
	~LedCounter();

	void draw( ci::Vec2f pos );

	float          getWidth();
	float          getHeight();

	void           setActive( bool active );
	bool           getActive();

	void           setNumber( unsigned short number );
	unsigned short getNumber();

private:
	ci::Vec2f     _getPosDigit( int digit, ci::Vec2f pos );
	void          _setNumber();

private:
	bool                       mActive;

	std::vector<LedDigit>      mLedDigits;
	unsigned short             mNumber;

	float                      mDistanceDigit;
};

} // namespace Nibbal
