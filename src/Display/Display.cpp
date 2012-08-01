#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Area.h"
#include "cinder/Color.h"
#include "cinder/Text.h"
#include "Display.h"

using namespace ci;
using namespace std;

namespace Nibbal
{

#if defined( CINDER_COCOA )
#define FONT_NAME "Impact"
#else
#define FONT_NAME "Impact"
#endif

const float  SIZE_X_SIDE         = 150.0f;
const float  SIZE_X_MIDDLE       = 300.0f;
const float  SIZE_Y_TOP          = 130.0f;
const float  SIZE_Y_BOTTOM       = 130.0f;

const float  DISTANCE_DIGIT      =  12.0f;
const float  DISTANCE_SEPARATOR  =   8.0f;
const float  SIZE_LED            =  12.0f;
const float  DISTANCE_LED        =   2.0f;
const float  DISTANCE_LED_BACK   =   2.0f;
const float  SIZE_BORDER_LED     =   2.0f;
const ColorA COLOR_LED_ON_TIME   = ColorA( 0.941f, 0.776f, 0.333f, 1.0f );
const ColorA COLOR_LED_ON_PERIOD = ColorA( 0.941f, 0.776f, 0.333f, 1.0f );
const ColorA COLOR_LED_ON_SCORE  = ColorA( 0.870f, 0.247f, 0.004f, 1.0f );
const ColorA COLOR_LED_OFF       = ColorA( 0.098f, 0.129f, 0.263f, 1.0f );
const ColorA COLOR_LED_BACK      = ColorA( 0.0f  , 0.0f  , 0.0f  , 1.0f );
const ColorA COLOR_TEXT_SCORE    = ColorA( 0.949f, 0.968f, 0.976f, 1.0f );
const ColorA COLOR_TEXT_PERIOD   = ColorA( 0.949f, 0.968f, 0.976f, 1.0f );
const ColorA COLOR_PANEL         = ColorA( 0.094f, 0.160f, 0.392f, 1.0f );
const ColorA COLOR_LINE          = ColorA( 0.184f, 0.239f, 0.376f, 1.0f );

/*
http://www.scoreboards-us.net/basketball%20scoreboard.htm
+---------+------------+---------+
|  HOME   |   timer    |  GUEST  |
+---------+------------+---------+
| counter |   PERIOD   | counter |
+---------+------------+---------+
*/
Display::Display()
: mActive( false )
, mDisplayTimer()
, mDisplayCounterScoreHome()
, mDisplayCounterScoreGuest()
, mDisplayCounterPeriod()
, mFontScore()
, mFontPeriod()
{
}

Display::~Display()
{
}

void Display::setup()
{
	mDisplayTimer             = DisplayTimer  ( SIZE_BORDER_LED, COLOR_LED_BACK   , DISTANCE_DIGIT, DISTANCE_SEPARATOR, SIZE_LED, DISTANCE_LED, COLOR_LED_ON_TIME  , COLOR_LED_OFF );
	mDisplayCounterScoreHome  = DisplayCounter( SIZE_BORDER_LED, COLOR_LED_BACK, 2, DISTANCE_DIGIT                    , SIZE_LED, DISTANCE_LED, COLOR_LED_ON_SCORE , COLOR_LED_OFF );
	mDisplayCounterScoreGuest = DisplayCounter( SIZE_BORDER_LED, COLOR_LED_BACK, 2, DISTANCE_DIGIT                    , SIZE_LED, DISTANCE_LED, COLOR_LED_ON_SCORE , COLOR_LED_OFF );
	mDisplayCounterPeriod     = DisplayCounter( SIZE_BORDER_LED, COLOR_LED_BACK, 1, DISTANCE_DIGIT                    , SIZE_LED, DISTANCE_LED, COLOR_LED_ON_PERIOD, COLOR_LED_OFF );

	mFontScore                = Font( FONT_NAME, 55 );
	mFontPeriod               = Font( FONT_NAME, 50 );

	gl::Fbo::Format format;
	format.enableDepthBuffer( false );
	mFbo = gl::Fbo( (int)getWidth(), (int)getHeight(), format );

	_updateFbo();
}

void Display::update()
{
	if( mDisplayTimer.update())
		_updateFbo();
}

void Display::_updateFbo()
{
	Area viewport = gl::getViewport();
	gl::pushMatrices();
	Vec2f pos = Vec2f( 0.0f, 0.0f );
	gl::enableAlphaBlending();

	mFbo.bindFramebuffer();
	gl::setMatricesWindow( Vec2i( (int)getWidth(), (int)getHeight()), false );

	glLineWidth( 2.0f );

	_drawHome( pos );
	_drawHome( pos );
	_drawTimer( pos );
	_drawGuest( pos );
	_drawPeriod( pos );

	glLineWidth( 1.0f );
	gl::disableAlphaBlending();
	mFbo.unbindFramebuffer();
	gl::popMatrices();
	gl::setViewport( viewport );
}

gl::Texture & Display::getTexture()
{
	return mFbo.getTexture();
}

float Display::getWidth()
{
	return 2 * SIZE_X_SIDE + SIZE_X_MIDDLE;
}

float Display::getHeight()
{
	return SIZE_Y_TOP + SIZE_Y_BOTTOM;
}

void Display::setActive( bool active )
{
	mActive = active;

	mDisplayTimer            .setActive( mActive );
	mDisplayCounterScoreHome .setActive( mActive );
	mDisplayCounterScoreGuest.setActive( mActive );
	mDisplayCounterPeriod    .setActive( mActive );

	_updateFbo();
}

bool Display::getActive()
{
	return mActive;
}

void Display::setTimeMax( unsigned short minutes, unsigned short seconds )
{
	mDisplayTimer.setTimeMax( minutes, seconds );
	_updateFbo();
}

void Display::setTimeAct( unsigned short minutes, unsigned short seconds )
{
	mDisplayTimer.setTimeAct( minutes, seconds );
	_updateFbo();
}

unsigned short Display::getMinutes()
{
	return mDisplayTimer.getMinutes();
}

unsigned short Display::getSeconds()
{
	return mDisplayTimer.getSeconds();
}

void Display::start()
{
	mDisplayTimer.start();
}

void Display::stop()
{
	mDisplayTimer.stop();
}

bool Display::isRunning()
{
	return mDisplayTimer.isRunning();
}

void Display::setHome( unsigned short home )
{
	mDisplayCounterScoreHome.setNumber( home );
	_updateFbo();
}

unsigned short Display::getHome()
{
	return mDisplayCounterScoreHome.getNumber();
}

void Display::setGuest( unsigned short guest )
{
	mDisplayCounterScoreGuest.setNumber( guest );
	_updateFbo();
}

unsigned short Display::getGuest()
{
	return mDisplayCounterScoreGuest.getNumber();
}

void Display::setPeriod( unsigned short period )
{
	mDisplayCounterPeriod.setNumber( period );
	_updateFbo();
}

unsigned short Display::getPeriod()
{
	return mDisplayCounterPeriod.getNumber();
}

void Display::_drawTimer( Vec2f pos )
{
	Rectf rectTimer = _getRectTimer( pos );
	gl::color( COLOR_PANEL );
	gl::drawSolidRect( rectTimer );
	gl::color( COLOR_LINE );
	gl::drawStrokedRect( rectTimer );

	mDisplayTimer.draw( Vec2f( rectTimer.x1 + ( rectTimer.getWidth()  - mDisplayTimer.getWidth())  / 2
	                         , rectTimer.y1 + ( rectTimer.getHeight() - mDisplayTimer.getHeight()) / 2 ));
}

void Display::_drawHome( Vec2f pos )
{
	// draw text
	Rectf rectTextHome  = _getRectTextHome( pos );
	gl::color( COLOR_PANEL );
	gl::drawSolidRect( rectTextHome );
	gl::color( COLOR_LINE );
	gl::drawStrokedRect( rectTextHome );

	TextBox label = TextBox().alignment( TextBox::CENTER ).font( mFontScore ).size( Vec2i( (int)rectTextHome.getWidth(), TextBox::GROW ) ).text( "HOME" );
	gl::color( COLOR_TEXT_SCORE );
	Surface surfaceLabel = label.render();

	Vec2f posLabel = Vec2f( rectTextHome.x1, rectTextHome.y1 + ( rectTextHome.getHeight() - surfaceLabel.getHeight()) / 2 );
	gl::draw( surfaceLabel, posLabel );

	// draw score
	Rectf rectScoreHome = _getRectScoreHome( pos );
	gl::color( COLOR_PANEL );
	gl::drawSolidRect( rectScoreHome );
	gl::color( COLOR_LINE );
	gl::drawStrokedRect( rectScoreHome );

	mDisplayCounterScoreHome.draw( Vec2f( rectScoreHome.x1 + ( rectScoreHome.getWidth()  - mDisplayCounterScoreHome.getWidth())  / 2
	                                    , rectScoreHome.y1 + ( rectScoreHome.getHeight() - mDisplayCounterScoreHome.getHeight()) / 2 ));
}

void Display::_drawGuest( Vec2f pos )
{
	// draw text
	Rectf rectTextGuest  = _getRectTextGuest( pos );
	gl::color( COLOR_PANEL );
	gl::drawSolidRect( rectTextGuest );
	gl::color( COLOR_LINE );
	gl::drawStrokedRect( rectTextGuest );

	TextBox label = TextBox().alignment( TextBox::CENTER ).font( mFontScore ).size( Vec2i( (int)rectTextGuest.getWidth(), TextBox::GROW ) ).text( "GUEST" );
	gl::color( COLOR_TEXT_SCORE );
	Surface surfaceLabel = label.render();

	Vec2f posLabel = Vec2f( rectTextGuest.x1, rectTextGuest.y1 + ( rectTextGuest.getHeight() - surfaceLabel.getHeight()) / 2 );
	gl::draw( surfaceLabel, posLabel );

	// draw score
	Rectf rectScoreGuest = _getRectScoreGuest( pos );
	gl::color( COLOR_PANEL );
	gl::drawSolidRect( rectScoreGuest );
	gl::color( COLOR_LINE );
	gl::drawStrokedRect( rectScoreGuest );

	mDisplayCounterScoreGuest.draw( Vec2f( rectScoreGuest.x1 + ( rectScoreGuest.getWidth()  - mDisplayCounterScoreGuest.getWidth())  / 2
	                                     , rectScoreGuest.y1 + ( rectScoreGuest.getHeight() - mDisplayCounterScoreGuest.getHeight()) / 2 ));
}

void Display::_drawPeriod( Vec2f pos )
{
	// draw text
	Rectf rectPeriod = _getRectPeriod( pos );
	gl::color( COLOR_PANEL );
	gl::drawSolidRect( rectPeriod );
	gl::color( COLOR_LINE );
	gl::drawStrokedRect( rectPeriod );

	TextBox label = TextBox().alignment( TextBox::LEFT ).font( mFontPeriod ).size( Vec2i( TextBox::GROW, TextBox::GROW ) ).text( "PERIOD" );
	gl::color( COLOR_TEXT_PERIOD );
	Surface surfaceLabel = label.render();

	Vec2f posLabel = Vec2f( rectPeriod.x1 + ( rectPeriod.getWidth()  - surfaceLabel.getWidth() - 1.5f * mDisplayCounterPeriod.getWidth()) / 2
	                      , rectPeriod.y1 + ( rectPeriod.getHeight() - surfaceLabel.getHeight()) / 2 );
	gl::draw( surfaceLabel, posLabel );

	// draw score

	mDisplayCounterPeriod.draw( Vec2f( posLabel.x    +   surfaceLabel.getWidth() + mDisplayCounterPeriod.getWidth() / 2
	                                 , rectPeriod.y1 + ( rectPeriod.getHeight()  - mDisplayCounterScoreGuest.getHeight()) / 2 ));
}

Vec2f Display::_getPosTextHome( Vec2f pos )
{
	Vec2f posTextHome = pos;
	return posTextHome;
}

Vec2f Display::_getPosScoreHome( Vec2f pos )
{
	Vec2f posScoreHome = _getPosTextHome( pos );
	posScoreHome.y += SIZE_Y_TOP;
	return posScoreHome;
}

Vec2f Display::_getPosTimer( Vec2f pos )
{
	Vec2f posTimer = _getPosTextHome( pos );
	posTimer.x += SIZE_X_SIDE;
	return posTimer;
}

Vec2f Display::_getPosPeriod( Vec2f pos )
{
	Vec2f posPeriod = _getPosScoreHome( pos );
	posPeriod.x += SIZE_X_SIDE;
	return posPeriod;
}

Vec2f Display::_getPosTextGuest( Vec2f pos )
{
	Vec2f posTextGuest = _getPosTimer( pos );
	posTextGuest.x += SIZE_X_MIDDLE;
	return posTextGuest;
}

Vec2f Display::_getPosScoreGuest( Vec2f pos )
{
	Vec2f posScoreGuest = _getPosPeriod( pos );
	posScoreGuest.x += SIZE_X_MIDDLE;
	return posScoreGuest;
}

Rectf Display::_getRectTextHome( Vec2f pos )
{
	Vec2f posTextHome = _getPosTextHome( pos );
	return Rectf( posTextHome, Vec2f( posTextHome.x + SIZE_X_SIDE, posTextHome.y + SIZE_Y_TOP ));
}

Rectf Display::_getRectScoreHome( Vec2f pos )
{
	Vec2f posScoreHome = _getPosScoreHome( pos );
	return Rectf( posScoreHome, Vec2f( posScoreHome.x + SIZE_X_SIDE, posScoreHome.y + SIZE_Y_BOTTOM ));
}

Rectf Display::_getRectTimer( Vec2f pos )
{
	Vec2f posTimer = _getPosTimer( pos );
	return Rectf( posTimer, Vec2f( posTimer.x + SIZE_X_MIDDLE, posTimer.y + SIZE_Y_TOP ));
}

Rectf Display::_getRectPeriod( Vec2f pos )
{
	Vec2f posPeriod = _getPosPeriod( pos );
	return Rectf( posPeriod, Vec2f( posPeriod.x + SIZE_X_MIDDLE, posPeriod.y + SIZE_Y_BOTTOM ));
}

Rectf Display::_getRectTextGuest( Vec2f pos )
{
	Vec2f posTextGuest = _getPosTextGuest( pos );
	return Rectf( posTextGuest, Vec2f( posTextGuest.x + SIZE_X_SIDE, posTextGuest.y + SIZE_Y_TOP ));
}

Rectf Display::_getRectScoreGuest( Vec2f pos )
{
	Vec2f posScoreGuest = _getPosScoreGuest( pos );
	return Rectf( posScoreGuest, Vec2f( posScoreGuest.x + SIZE_X_SIDE, posScoreGuest.y + SIZE_Y_BOTTOM ));
}

} // namespace Nibbal
