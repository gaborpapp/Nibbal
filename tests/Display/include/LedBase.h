#pragma once

#include "cinder/Color.h"
#include "cinder/Vector.h"

namespace DisplayTest
{

class LedBase
{
public:
	LedBase();
	LedBase( float sizeLed, float distanceLed, ci::ColorA colorOn, ci::ColorA colorOff );
	virtual ~LedBase();

	virtual void  draw( ci::Vec2f pos ) = 0;

	virtual float getWidth()  = 0;
	virtual float getHeight() = 0;

	void  setActive( bool active );
	bool  getActive();

protected:
	bool           mActive;

	float          mSizeLed;
	float          mDistanceLed;
	ci::ColorA     mColorOn;
	ci::ColorA     mColorOff;
};

} // namespace DisplayTest