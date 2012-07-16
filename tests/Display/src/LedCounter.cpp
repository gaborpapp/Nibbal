#include "LedCounter.h"

using namespace ci;
using namespace std;

namespace DisplayTest
{

LedCounter::LedCounter()
: mActive( false )
, mLedDigits()
, mNumber( 0 )
, mDistanceDigit( 0 )
{
}

LedCounter::LedCounter( unsigned short countDigit, float distanceDigit, float sizeLed, float distanceLed, ci::ColorA colorOn, ci::ColorA colorOff )
: mActive( false )
, mLedDigits()
, mNumber( 0 )
, mDistanceDigit( distanceDigit )
{
	for( int i = 0; i < countDigit; i++ )
	{
		mLedDigits.push_back( LedDigit( sizeLed, distanceLed, colorOn, colorOff ));

		if( i < countDigit - 1 )
			mLedDigits[i].setShowNull( false );
	}
}

LedCounter::~LedCounter()
{
}

void LedCounter::draw( ci::Vec2f pos )
{
	int i = 0;
	for( vector<LedDigit>::iterator p = mLedDigits.begin(); p != mLedDigits.end(); ++p )
	{
		Vec2f posDigit = _getPosDigit( i++, pos );
		p->draw( posDigit );
	}
}

float LedCounter::getWidth()
{
	float widthDigit = mLedDigits[0].getWidth();
	int   size       = mLedDigits.size();

	return size * widthDigit + ( size - 1 ) * mDistanceDigit;
}

float LedCounter::getHeight()
{
	float heightDigit = mLedDigits[0].getHeight();

	return heightDigit;
}

void LedCounter::setActive( bool active )
{
	mActive = active;

	for( vector<LedDigit>::iterator p = mLedDigits.begin(); p != mLedDigits.end(); ++p )
	{
		p->setActive( active );
	}
}

bool LedCounter::getActive()
{
	return mActive;
}

void LedCounter::setNumber( unsigned short number )
{
	int size = mLedDigits.size();
	int maxNumber = 1;

	while( size-- )
	{
		maxNumber *= 10;
	}

	if( number < maxNumber)
		mNumber = number;
	else
		mNumber = 0;

	_setNumber();
}

unsigned short LedCounter::getNumber()
{
	return mNumber;
}

ci::Vec2f LedCounter::_getPosDigit( int digit, ci::Vec2f pos )
{
	float distance = mLedDigits[0].getWidth() + mDistanceDigit;
	Vec2f posDigit = pos;

	posDigit.x += digit * distance;

	return posDigit;
}

void LedCounter::_setNumber()
{
	unsigned short number = mNumber;

	for( vector<LedDigit>::reverse_iterator p = mLedDigits.rbegin(); p != mLedDigits.rend(); ++p )
	{
		p->setNumber( number % 10 );
		number /= 10;

		if( p != mLedDigits.rbegin())
		{
			if( number )
				p->setShowNull( true );
			else
				p->setShowNull( false );
		}
	}
}

} // namespace DisplayTest
