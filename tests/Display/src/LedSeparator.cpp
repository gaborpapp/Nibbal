#include "cinder/gl/gl.h"
#include "LedSeparator.h"

using namespace ci;

namespace DisplayTest
{

/*
off  :
 .   .  
 .   .  
 -   @
 .   .  
 -   @
 .   .  
 .   .  
*/
LedSeparator::LedSeparator()
: LedBase()
{
}

LedSeparator::LedSeparator( float sizeLed, float distanceLed, ColorA colorOn, ColorA colorOff )
: LedBase( sizeLed, distanceLed, colorOn, colorOff )
{
}

LedSeparator::~LedSeparator()
{
}

void LedSeparator::draw( Vec2f pos )
{
	for( int i = 0; i < LED_SEPARATOR_NUMBER; i++ )
	{
		Vec2f  posLed   = _getPosLed( i, pos );
		ColorA colorLed = _getColorLed( i );

		gl::color( colorLed );
		gl::drawSolidCircle( posLed, mSizeLed / 2 );
	}
}

float LedSeparator::getWidth()
{
	return 1 * mSizeLed;
}

float LedSeparator::getHeight()
{
	return 7 * mSizeLed + 6 * mDistanceLed;
}

Vec2f LedSeparator::_getPosLed( int led, Vec2f pos )
{
	float distance = mSizeLed + mDistanceLed;
	Vec2f posLed   = pos;

	switch( led )
	{
	case  0:
		posLed.x += 0 * distance;
		posLed.y += 2 * distance;
		break;
	case  1:
		posLed.x += 0 * distance;
		posLed.y += 4 * distance;
		break;
	}

	posLed += mSizeLed / 2;

	return posLed;
}

ColorA LedSeparator::_getColorLed( int led )
{
	if( mActive )
		return mColorOn;

	return mColorOff;
}

} // namespace DisplayTest
