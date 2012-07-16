#pragma once

#include "cinder/gl/Fbo.h"
#include "cinder/Vector.h"
#include "cinder/Font.h"
#include "DisplayTimer.h"
#include "DisplayCounter.h"

namespace DisplayTest
{

class Display
{
public:
	Display();
	~Display();

	void setup();
	void update();

	ci::gl::Fbo   *getFbo();

	float          getWidth();
	float          getHeight();

	void           setActive( bool active );
	bool           getActive();

	void           setTime( unsigned short minutes, unsigned short seconds );
	unsigned short getMinutes();
	unsigned short getSeconds();
	void           start();
	void           stop();
	bool           isRunning();

	void           setHome( unsigned short home );
	unsigned short getHome();

	void           setGuest( unsigned short guest );
	unsigned short getGuest();

	void           setPeriod( unsigned short period );
	unsigned short getPeriod();

	template<typename T>
	void addCallback( void (T::* callbackFunction)(), T * callbackObject )
	{
		mDisplayTimer.addCallback<T>( callbackFunction, callbackObject );
	}

private:
	void _updateFbo();

	void _drawTimer ( ci::Vec2f pos );
	void _drawHome  ( ci::Vec2f pos );
	void _drawGuest ( ci::Vec2f pos );
	void _drawPeriod( ci::Vec2f pos );

	ci::Vec2f _getPosTextHome   ( ci::Vec2f pos );
	ci::Vec2f _getPosScoreHome  ( ci::Vec2f pos );
	ci::Vec2f _getPosTimer      ( ci::Vec2f pos );
	ci::Vec2f _getPosPeriod     ( ci::Vec2f pos );
	ci::Vec2f _getPosTextGuest  ( ci::Vec2f pos );
	ci::Vec2f _getPosScoreGuest ( ci::Vec2f pos );

	ci::Rectf _getRectTextHome  ( ci::Vec2f pos );
	ci::Rectf _getRectScoreHome ( ci::Vec2f pos );
	ci::Rectf _getRectTimer     ( ci::Vec2f pos );
	ci::Rectf _getRectPeriod    ( ci::Vec2f pos );
	ci::Rectf _getRectTextGuest ( ci::Vec2f pos );
	ci::Rectf _getRectScoreGuest( ci::Vec2f pos );

private:
	bool           mActive;

	DisplayTimer   mDisplayTimer;
	DisplayCounter mDisplayCounterScoreHome;
	DisplayCounter mDisplayCounterScoreGuest;
	DisplayCounter mDisplayCounterPeriod;

	ci::Font       mFontScore;
	ci::Font       mFontPeriod;

	ci::gl::Fbo    mFbo;
};

} // namespace DisplayTest