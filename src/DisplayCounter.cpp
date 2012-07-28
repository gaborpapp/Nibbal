#include "cinder/gl/gl.h"
#include "DisplayCounter.h"

using namespace ci;
using namespace std;

namespace Nibbal
{

DisplayCounter::DisplayCounter()
: LedCounter()
, mSizeBorder( 0.0f )
, mColorBack()
{
}

DisplayCounter::DisplayCounter( float sizeBorder, ci::ColorA colorBack, unsigned short countDigit, float distanceDigit, float sizeLed, float distanceLed, ci::ColorA colorOn, ci::ColorA colorOff )
: LedCounter( countDigit, distanceDigit, sizeLed, distanceLed, colorOn, colorOff )
, mSizeBorder( sizeBorder )
, mColorBack( colorBack )
{
}

void DisplayCounter::draw( Vec2f pos )
{
	gl::color( mColorBack );
	gl::drawSolidRect( Rectf( pos, Vec2f( pos.x + getWidth(), pos.y + getHeight())));

	LedCounter::draw( Vec2f( pos.x + mSizeBorder, pos.y + mSizeBorder ));
}

float DisplayCounter::getWidth()
{
	return LedCounter::getWidth() + 2 * mSizeBorder;
}

float DisplayCounter::getHeight()
{
	return LedCounter::getHeight() + 2 * mSizeBorder;
}

} // namespace Nibbal
