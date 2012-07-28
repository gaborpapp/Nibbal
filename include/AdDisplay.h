#pragma once

#include <vector>

#include "cinder/Cinder.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timeline.h"

#include "PParams.h"

namespace Nibbal {

class AdDisplay
{
	public:
		void setup( const ci::fs::path &adFolder, const ci::Vec2i size );
		void showParams( bool show );

		void update();

		ci::gl::Texture &getTexture() { return mFbo.getTexture(); }

	private:
		std::vector< ci::gl::Texture > mAds;

		ci::params::PInterfaceGl mParams;

		float mKeepDuration;
		float mChangeDuration;

		ci::gl::Fbo mFbo;
		ci::gl::GlslProg mShader;

		size_t mCurrentAdIndex;

		ci::TimelineRef mTimelineRef;
		ci::Anim< float > mCrossfade;
		void setupTimeline();
};

} // namespace Nibbal
