#include "cinder/gl/gl.h"
#include "LedDigit.h"

using namespace ci;

namespace DisplayTest
{

/*
 off   0    1    2    3    4    5    6    7    8    9 
---- @@@@ ---@ @@@@ @@@@ @--@ @@@@ @@@@ @@@@ @@@@ @@@@
-  - @  @ -  @ -  @ -  @ @  @ @  - @  - -  @ @  @ @  @
-  - @  @ -  @ -  @ -  @ @  @ @  - @  - -  @ @  @ @  @
---- @--@ ---@ @@@@ @@@@ @@@@ @@@@ @@@@ ---@ @@@@ @@@@
-  - @  @ -  @ @  - -  @ -  @ -  @ @  @ -  @ @  @ -  @
-  - @  @ -  @ @  - -  @ -  @ -  @ @  @ -  @ @  @ -  @
---- @@@@ ---@ @@@@ @@@@ ---@ @@@@ @@@@ ---@ @@@@ @@@@
*/
const int LedDigit::mNumberMask[10][LED_DIGIT_NUMBER] = {
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 },  // 0
	{ 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1 },  // 1
	{ 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1 },  // 2
	{ 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1 },  // 3
	{ 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1 },  // 4
	{ 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1 },  // 5
	{ 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },  // 6
	{ 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1 },  // 7
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },  // 8
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1 }}; // 9

LedDigit::LedDigit()
: LedBase()
, mNumber( 0 )
, mShowNull( true )
{
}

LedDigit::LedDigit( float sizeLed, float distanceLed, ColorA colorOn, ColorA colorOff )
: LedBase( sizeLed, distanceLed, colorOn, colorOff )
, mNumber( 0 )
, mShowNull( true )
{
}

LedDigit::~LedDigit()
{
}

void LedDigit::draw( Vec2f pos )
{
	for( int i = 0; i < LED_DIGIT_NUMBER; i++ )
	{
		Vec2f  posLed   = _getPosLed( i, pos );
		ColorA colorLed = _getColorLed( i );

		gl::color( colorLed );
		gl::drawSolidCircle( posLed, mSizeLed / 2 );
	}
}

float LedDigit::getWidth()
{
	return 4 * mSizeLed + 3 * mDistanceLed;
}

float LedDigit::getHeight()
{
	return 7 * mSizeLed + 6 * mDistanceLed;
}

void LedDigit::setNumber( unsigned short number )
{
	if( number <= 9 )
		mNumber = number;
	else
		mNumber = 0;
}

unsigned short LedDigit::getNumber()
{
	return mNumber;
}

void LedDigit::setShowNull( bool showNull )
{
	mShowNull = showNull;
}

bool LedDigit::getShowNull()
{
	return mShowNull;
}

Vec2f LedDigit::_getPosLed( int led, Vec2f pos )
{
	float distance = mSizeLed + mDistanceLed;
	Vec2f posLed   = pos;

	switch( led )
	{
	case  0:
	case  1:
	case  2:
	case  3:
		posLed.x += led * distance;
		posLed.y += 0   * distance;
		break;
	case  4:
		posLed.x += 0 * distance;
		posLed.y += 1 * distance;
		break;
	case  5:
		posLed.x += 3 * distance;
		posLed.y += 1 * distance;
		break;
	case  6:
		posLed.x += 0 * distance;
		posLed.y += 2 * distance;
		break;
	case  7:
		posLed.x += 3 * distance;
		posLed.y += 2 * distance;
		break;
	case  8:
	case  9:
	case 10:
	case 11:
		posLed.x += ( led - 8 ) * distance;
		posLed.y +=         3   * distance;
		break;
	case 12:
		posLed.x += 0 * distance;
		posLed.y += 4 * distance;
		break;
	case 13:
		posLed.x += 3 * distance;
		posLed.y += 4 * distance;
		break;
	case 14:
		posLed.x += 0 * distance;
		posLed.y += 5 * distance;
		break;
	case 15:
		posLed.x += 3 * distance;
		posLed.y += 5 * distance;
		break;
	case 16:
	case 17:
	case 18:
	case 19:
		posLed.x += ( led - 16 ) * distance;
		posLed.y +=          6   * distance;
		break;
	}

	posLed += mSizeLed / 2;

	return posLed;
}

ColorA LedDigit::_getColorLed( int led )
{
	if( ! mShowNull && mNumber == 0 )
		return mColorOff;

	if( mActive && mNumberMask[mNumber][led] )
		return mColorOn;

	return mColorOff;
}

} // namespace DisplayTest
