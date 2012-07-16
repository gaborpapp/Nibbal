#include "cinder/app/App.h"
#include "cinder/CinderMath.h"
#include "LedTimer.h"

using namespace ci;

namespace DisplayTest
{

LedTimer::LedTimer()
: mActive( false )
, mRun( false )
, mDistanceDigit( 0 )
, mDistanceSeparator( 0 )
, mLedSeparator()
, mHundredths( 0 )
, mTimer( 0 )
{
}

LedTimer::LedTimer( float distanceDigit, float distanceSeparator, float sizeLed, float distanceLed, ci::ColorA colorOn, ci::ColorA colorOff )
: mActive( false )
, mRun( false )
, mDistanceDigit( distanceDigit )
, mDistanceSeparator( distanceSeparator )
, mLedSeparator( sizeLed, distanceLed, colorOn, colorOff )
, mHundredths( 0 )
, mTimer( 0 )
{
	mListener= std::shared_ptr<Listener>( new Listener());

	for( int i = 0; i < DIGIT_NUMBER; i++ )
	{
		mLedDigits[i] = LedDigit( sizeLed, distanceLed, colorOn, colorOff );
	}

	mLedDigits[0].setShowNull( false );
}

LedTimer::~LedTimer()
{
}

bool LedTimer::update()
{
	if( isRunning())
	{
		unsigned short timer = (int)( ci::app::getElapsedSeconds() * 100 );
		mHundredths -= math<unsigned short>::min( timer - mTimer, mHundredths );
		mTimer = timer;

		_setTime();

		if( mHundredths == 0 )
		{
			stop();
			mListener->callCallback();
		}

		return true;
	}

	return false;
}

void LedTimer::draw( ci::Vec2f pos )
{
	for( int i = 0; i < DIGIT_NUMBER; i++ )
	{
		Vec2f posDigit = _getPosDigit( i, pos );
		mLedDigits[i].draw( posDigit );
	}

	Vec2f posSeparator = _getPosSeparator( pos );
	mLedSeparator.draw( posSeparator );
}

float LedTimer::getWidth()
{
	float widthDigit     = mLedDigits[0].getWidth();
	float widthSeparator = mLedSeparator.getWidth();

	return 4 * widthDigit + 2 * mDistanceDigit + widthSeparator + 2 * mDistanceSeparator;
}

float LedTimer::getHeight()
{
	float heightDigit = mLedDigits[0].getHeight();

	return heightDigit;
}

void LedTimer::setActive( bool active )
{
	mActive = active;

	for( int i = 0; i < DIGIT_NUMBER; i++ )
	{
		mLedDigits[i].setActive( active );
	}

	mLedSeparator.setActive( active );
}

bool LedTimer::getActive()
{
	return mActive;
}

void LedTimer::setTime( unsigned short second, unsigned short hundredths )
{
	stop();

	if( second     < 59
	 && hundredths < 99 )
		mHundredths = 100 * second + hundredths;
	else
		mHundredths = 0;

	_setTime();
}

unsigned short LedTimer::getSecond()
{
	return mHundredths / 100;
}

unsigned short LedTimer::getHundredths()
{
	return mHundredths % 100;
}

void LedTimer::start()
{
	mRun   = true;
	mTimer = (int)( ci::app::getElapsedSeconds() * 100 );
}

void LedTimer::stop()
{
	mRun   = false;
	mTimer = 0;
}

bool LedTimer::isRunning()
{
	return mRun;
}

ci::Vec2f LedTimer::_getPosDigit( int digit, ci::Vec2f pos )
{
	float widthDigit     = mLedDigits[0].getWidth();
	float widthSeparator = mLedSeparator.getWidth();
	Vec2f posDigit = pos;

	switch( digit )
	{
	case 0: 
		// do nothing
		break;
	case 1: 
		posDigit.x += 1 * widthDigit + 1 * mDistanceDigit;
		break;
	case 2: 
		posDigit.x += 2 * widthDigit + 1 * mDistanceDigit + widthSeparator + mDistanceSeparator * 2;
		break;
	case 3: 
		posDigit.x += 3 * widthDigit + 2 * mDistanceDigit + widthSeparator + mDistanceSeparator * 2;
		break;
	}

	return posDigit;
}

ci::Vec2f LedTimer::_getPosSeparator( ci::Vec2f pos )
{
	float widthDigit = mLedDigits[0].getWidth();
	Vec2f posSeparator  = pos;

	posSeparator.x += 2 * widthDigit + 1 * mDistanceDigit + mDistanceSeparator;

	return posSeparator;
}

void LedTimer::_setTime()
{
	unsigned short second     = getSecond();
	unsigned short hundredths = getHundredths();

	mLedDigits[0].setNumber( second     / 10 );
	mLedDigits[1].setNumber( second     % 10 );
	mLedDigits[2].setNumber( hundredths / 10 );
	mLedDigits[3].setNumber( hundredths % 10 );
}

} // namespace DisplayTest
