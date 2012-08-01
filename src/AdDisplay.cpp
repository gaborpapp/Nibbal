#include <assert.h>

#include "cinder/app/App.h"
#include "cinder/CinderMath.h"
#include "cinder/ImageIo.h"

#include "Resources.h"

#include "AdDisplay.h"

using namespace ci;
using namespace std;

namespace Nibbal {

void AdDisplay::setup( const fs::path &adFolder, Vec2i size )
{
	app::console() << "loading ads from " << adFolder.string() << endl;
	for ( fs::directory_iterator it( adFolder );
			it != fs::directory_iterator(); ++it )
	{
		if ( fs::is_regular_file( *it ) &&
				( ( it->path().extension().string() == ".png" ) ||
				  ( it->path().extension().string() == ".jpg" ) ) )
		{
			app::console() << "   " << it->path().filename() << endl;
			mAds.push_back( gl::Texture( loadImage( adFolder / it->path().filename() ) ) );
		}
	}
	mCurrentAdIndex = 0;

	gl::Fbo::Format format;
	format.enableDepthBuffer( false );
	mFbo = gl::Fbo( size.x, size.y, format );

	mShader = gl::GlslProg( app::loadResource( RES_PASSTHROUGH_VERT ),
							app::loadResource( RES_ADDISPLAY_FRAG ) );
	mShader.bind();
	mShader.uniform( "tex0", 0 );
	mShader.uniform( "tex1", 1 );
	mShader.unbind();

	mParams = params::PInterfaceGl( "Advertisements", Vec2i( 200, 300 ) );
	mParams.addPersistentSizeAndPosition();

	mParams.addPersistentParam( "Keep duration", &mKeepDuration, 5, "min=1" );
	mParams.addPersistentParam( "Change duration", &mChangeDuration, 1, "min=0, step=.5" );
	vector< string > switchNames;
	switchNames.push_back( "fade" );
	switchNames.push_back( "hard from left" );
	switchNames.push_back( "hard from right" );
	switchNames.push_back( "soft from left" );
	switchNames.push_back( "soft from right" );
	mParams.addPersistentParam( "Switch type", switchNames, &mSwitchType, 3 );
	mParams.addPersistentParam( "Smooth border", &mSmoothSwitchBorder, 0.20, "min=0 max=1 step=0.01" );

	mTimelineRef = Timeline::create();
	app::timeline().add( mTimelineRef );
	setupTimeline();
}

// NOTE: could not solve it with looping + finishFn
// finishFn didn't happen at the last tween when loop was set
void AdDisplay::setupTimeline()
{
	mCrossfade = 0;
	mCurrentAdIndex += 1;
	if ( mCurrentAdIndex >= mAds.size() )
		mCurrentAdIndex = 0;
	mTimelineRef->clear();
	mTimelineRef->apply( &mCrossfade, 0.f, mKeepDuration );
	mTimelineRef->appendTo( &mCrossfade, 1.f, mChangeDuration );
	//ci::app::console() << "switched ad to " << mCurrentAdIndex << std::endl;
}

void AdDisplay::update()
{
	static float lastKeepDuration = -1;
	static float lastChangeDuration = -1;

	if ( mAds.empty() )
		return;

	if ( ( mKeepDuration != lastKeepDuration ) ||
		 ( mChangeDuration != lastChangeDuration ) )
	{
		setupTimeline();

		lastKeepDuration = mKeepDuration;
		lastChangeDuration = mChangeDuration;
	}

	Area viewport = gl::getViewport();
	gl::pushMatrices();

	mFbo.bindFramebuffer();
	gl::setMatricesWindow( mFbo.getSize(), false );
	gl::setViewport( mFbo.getBounds() );

	gl::clear( Color::black() );

	size_t otherAdIndex = mCurrentAdIndex + 1;
	if ( otherAdIndex >= mAds.size() )
		otherAdIndex = 0;
	mAds[ mCurrentAdIndex ].enableAndBind();
	mAds[ otherAdIndex ].bind( 1 );

	mShader.bind();
	mShader.uniform( "t", mCrossfade );
	mShader.uniform( "type", mSwitchType );
	mShader.uniform( "border", mSmoothSwitchBorder );

	gl::drawSolidRect( mFbo.getBounds() );

	mShader.unbind();

	mAds[ mCurrentAdIndex ].unbind();
	mAds[ otherAdIndex ].unbind( 1 );

	if ( mCrossfade >= 1.f )
		setupTimeline();

	gl::disable( GL_TEXTURE_2D );
	mFbo.unbindFramebuffer();

	gl::popMatrices();
	gl::setViewport( viewport );
}

} // namespace Nibbal
