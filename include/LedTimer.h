#pragma once

#include <vector>
#include "cinder/Color.h"
#include "cinder/Vector.h"
#include "LedDigit.h"
#include "LedSeparator.h"
#include "Listener.h"

namespace Nibbal
{
#define DIGIT_NUMBER 4

class LedTimer
{
public:
	LedTimer();
	LedTimer( float distanceDigit, float distanceSeparator, float sizeLed, float distanceLed, ci::ColorA colorOn, ci::ColorA colorOff );
	virtual ~LedTimer();

	bool update();
	void draw( ci::Vec2f pos );

	float          getWidth();
	float          getHeight();

	void           setActive( bool active );
	bool           getActive();

	void           setTimeMax( unsigned short minutes, unsigned short seconds );
	void           setTimeAct( unsigned short minutes, unsigned short seconds );
	unsigned short getMinutes();
	unsigned short getSeconds();

	void           start();
	void           stop();
	bool           isRunning();

	template<typename T>
	void addCallback( void (T::* callbackFunction)(), T * callbackObject )
	{
		mListener->addCallback<T>( callbackFunction, callbackObject );
	}

private:
	ci::Vec2f     _getPosDigit( int digit, ci::Vec2f pos );
	ci::Vec2f     _getPosSeparator( ci::Vec2f pos );

	void          _setTime();

private:
	bool                       mActive;
	bool                       mRun;

	LedDigit                   mLedDigits[DIGIT_NUMBER];
	LedSeparator               mLedSeparator;

	float                      mDistanceDigit;
	float                      mDistanceSeparator;

	unsigned short             mSecondsMax;
	unsigned short             mSecondsAct;
	unsigned short             mTimer;

	std::shared_ptr<Listener>  mListener;
};

} // namespace Nibbal
