#include "LedBase.h"

using namespace ci;

namespace Nibbal
{

LedBase::LedBase()
: mActive( false )
, mSizeLed( 0.0f )
, mDistanceLed( 0.0f )
, mColorOn( ColorA( 0, 0, 0, 0 ))
, mColorOff( ColorA( 0, 0, 0, 0 ))
{
}

LedBase::LedBase( float sizeLed, float distanceLed, ColorA colorOn, ColorA colorOff )
: mActive( false )
, mSizeLed( sizeLed )
, mDistanceLed( distanceLed )
, mColorOn( colorOn )
, mColorOff( colorOff )
{
}

LedBase::~LedBase()
{
}

void LedBase::setActive( bool active )
{
	mActive = active;
}

bool LedBase::getActive()
{
	return mActive;
}

} // namespace Nibbal
