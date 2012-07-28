#include "cinder/gl/gl.h"
#include "DisplayTimer.h"

using namespace ci;
using namespace std;

namespace Nibbal
{

DisplayTimer::DisplayTimer()
: LedTimer()
, mSizeBorder( 0.0f )
, mColorBack()
{
}

DisplayTimer::DisplayTimer( float sizeBorder, ColorA colorBack, float distanceDigit, float distanceSeparator, float sizeLed, float distanceLed, ColorA colorOn, ColorA colorOff )
: LedTimer( distanceDigit, distanceSeparator, sizeLed, distanceLed, colorOn, colorOff )
, mSizeBorder( sizeBorder )
, mColorBack( colorBack )
{
}

void DisplayTimer::draw( Vec2f pos )
{
	gl::color( mColorBack );
	gl::drawSolidRect( Rectf( pos, Vec2f( pos.x + getWidth(), pos.y + getHeight())));

	LedTimer::draw( Vec2f( pos.x + mSizeBorder, pos.y + mSizeBorder ));
}

float DisplayTimer::getWidth()
{
	return LedTimer::getWidth() + 2 * mSizeBorder;
}

float DisplayTimer::getHeight()
{
	return LedTimer::getHeight() + 2 * mSizeBorder;
}

} // namespace Nibbal
