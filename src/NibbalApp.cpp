/*
 Copyright (C) 2012 Gabor Papp, Botond Gabor Barna

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Area.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/MayaCamUI.h"

#include "AntTweakBar.h"

#include "PParams.h"

#include "Scene.h"
#include "Physics.h"
#include "AdDisplay.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Nibbal {

const float WAIT_WIN_LOSE_TIME = 5.0f;

class NibbalApp : public AppBasic
{
	public:
		void prepareSettings( Settings *settings );
		void setup();
		void shutdown();

		void keyDown( KeyEvent event );
		void mouseDown( MouseEvent event );
		void mouseDrag( MouseEvent event );
		void resize( ResizeEvent event );

		void update();
		void draw();

	private:
		params::PInterfaceGl mParams;
		void setupParams();
		void showAllParams( bool visible );
		void drawState();

		void eventNoUser();
		void eventNewUser();
		void eventWin();
		void eventLose();

		Scene mScene;
		Physics mPhysics;

		MayaCamUI mMayaCam;

		float mFps;
		bool mCameraLock;
		float mCameraFov;
		Vec3f mCameraEyePoint;
		Vec3f mCameraCenterOfInterestPoint;

		gl::Texture mForegroundLayer;
		Area mForegroundArea;
		gl::Texture mIdleText;
		gl::Texture mWinText;
		gl::Texture mLoseText;

		bool mDrawBranding;
		bool mInfiniteMode;
		bool mFullscreen;

		float mTimer;

		enum {
			STATE_IDLE,
			STATE_GAME,
			STATE_WIN,
			STATE_LOSE
		};

		int mState;
		int mStateSave;
};

void NibbalApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 640, 480 );
}

void NibbalApp::setup()
{
	gl::disableVerticalSync();

	try
	{
		setupParams();

		mPhysics.setup();
		mScene.setup( &mPhysics );
	}
	catch ( const std::exception &exc )
	{
		console() << exc.what() << endl;
		quit();
	}

	mScene.addCallback<NibbalApp>( Scene::ET_NO_USER , &NibbalApp::eventNoUser  , this );
	mScene.addCallback<NibbalApp>( Scene::ET_NEW_USER, &NibbalApp::eventNewUser , this );
	mScene.addCallback<NibbalApp>( Scene::ET_WIN     , &NibbalApp::eventWin     , this );
	mScene.addCallback<NibbalApp>( Scene::ET_LOSE    , &NibbalApp::eventLose    , this );

	CameraPersp cam;
	cam.setPerspective( mCameraFov, getWindowAspectRatio(), 0.1f, 1000.0f );
	cam.setEyePoint( mCameraEyePoint );
	cam.setCenterOfInterestPoint( mCameraCenterOfInterestPoint );
	mMayaCam.setCurrentCam( cam );

	gl::enableDepthRead();
	gl::enableDepthWrite();

	showAllParams( false );

	mForegroundLayer = gl::Texture( loadImage( getAssetPath( "gfx/foreground.png" ) ) );
	mIdleText = gl::Texture( loadImage( getAssetPath( "gfx/start.png" ) ) );
	mWinText = gl::Texture( loadImage( getAssetPath( "gfx/win.png" ) ) );
	mLoseText = gl::Texture( loadImage( getAssetPath( "gfx/try_again.png" ) ) );

	mTimer = 0;
	mState = STATE_IDLE;
	mStateSave = mState;
	mScene.initGame();

	if ( mFullscreen )
		setFullScreen( true );
}

void NibbalApp::shutdown()
{
	params::PInterfaceGl::save();
}

void NibbalApp::setupParams()
{
	// params
	fs::path paramsXml( getAssetPath( "params.xml" ));
	if ( paramsXml.empty() )
	{
#if defined( CINDER_MAC )
		fs::path assetPath( getResourcePath() / "assets" );
#else
		fs::path assetPath( getAppPath() / "assets" );
#endif
		createDirectories( assetPath );
		paramsXml = assetPath / "params.xml" ;
	}
	params::PInterfaceGl::load( paramsXml );

	mParams = params::PInterfaceGl( "Parameters", Vec2i( 200, 300 ) );
	mParams.addPersistentSizeAndPosition();

	mFps = 0;
	mParams.addParam( "Fps", &mFps, "", true );
	mParams.addSeparator();
	mParams.addPersistentParam( "Fullscreen", &mFullscreen, true );
	mParams.addPersistentParam( "Draw branding", &mDrawBranding, true );
	mParams.addPersistentParam( "Infinite mode", &mInfiniteMode, false );
	mParams.addSeparator();
	mParams.addText( "Camera" );
	mParams.addPersistentParam( "Lock camera", &mCameraLock, true );
	mParams.addPersistentParam( "Fov", &mCameraFov, 45.f, "min=20 max=90 step=.1" );
	mParams.addPersistentParam( "Eye", &mCameraEyePoint, Vec3f( 0, 2.3, -3.4 ), "", true );
	mParams.addPersistentParam( "Center of Interest", &mCameraCenterOfInterestPoint, Vec3f( 0, 2.3, 0 ), "", true );
}

void NibbalApp::update()
{
	if( mState == STATE_WIN
	 || mState == STATE_LOSE )
	{
		if( (float)App::getElapsedSeconds() - mTimer > WAIT_WIN_LOSE_TIME )
		{
			mState = mStateSave;

			if( mState == STATE_GAME )
				mScene.startGame();
			else
				mScene.stopGame();
		}
	}

	mFps = getAverageFps();
	mScene.update();
	mPhysics.update( mFps );

	CameraPersp cam = mMayaCam.getCamera();
	if ( cam.getFov() != mCameraFov )
	{
		cam.setPerspective( mCameraFov, getWindowAspectRatio(), 0.1f, 1000.0f );
		mMayaCam.setCurrentCam( cam );
	}
	mCameraEyePoint = cam.getEyePoint();
	mCameraCenterOfInterestPoint = cam.getCenterOfInterestPoint();
}

void NibbalApp::draw()
{
	gl::clear( Color::black() );

	gl::setViewport( getWindowBounds() );
	gl::setMatrices( mMayaCam.getCamera() );

	mScene.draw();

	mPhysics.draw();

	if ( mDrawBranding )
	{
		gl::setMatricesWindow( getWindowSize() );
		gl::disableDepthRead();
		gl::disableDepthWrite();
		gl::enableAlphaBlending();
		gl::color( ColorA::white() );
		gl::draw( mForegroundLayer, mForegroundArea );
		gl::disableAlphaBlending();
		gl::enableDepthRead();
		gl::enableDepthWrite();
	}

	drawState();
	params::PInterfaceGl::draw();
}

void NibbalApp::drawState()
{
	if ( !mInfiniteMode )
	{
		gl::Texture t;

		switch ( mState )
		{
		case STATE_IDLE:
			t = mIdleText;
			break;

		case STATE_WIN:
			t = mWinText;
			break;

		case STATE_LOSE:
			t = mLoseText;
			break;

		default:
			break;
		}

		if ( t )
		{
			gl::enableAlphaBlending();
			gl::draw( t, Area::proportionalFit( t.getBounds(), getWindowBounds(), true, true ) );
			gl::disableAlphaBlending();
		}
	}
}

// show/hide all bars except help, which is always hidden
void NibbalApp::showAllParams( bool visible )
{
	int barCount = TwGetBarCount();

	int32_t visibleInt = visible ? 1 : 0;
	for ( int i = 0; i < barCount; ++i )
	{
		TwBar *bar = TwGetBarByIndex( i );
		TwSetParam( bar, NULL, "visible", TW_PARAM_INT32, 1, &visibleInt );
	}

	TwDefine( "TW_HELP visible=false" );
}

void NibbalApp::keyDown( KeyEvent event )
{
	switch ( event.getCode() )
	{
		case KeyEvent::KEY_f:
			if ( !isFullScreen() )
			{
				setFullScreen( true );
				if ( mParams.isVisible() )
					showCursor();
				else
					hideCursor();
			}
			else
			{
				setFullScreen( false );
				showCursor();
			}
			mFullscreen = isFullScreen();
			break;

		case KeyEvent::KEY_s:
		{
			showAllParams( !mParams.isVisible() );
			if ( isFullScreen() )
			{
				if ( mParams.isVisible() )
					showCursor();
				else
					hideCursor();
			}
			break;
		}

		case KeyEvent::KEY_ESCAPE:
			quit();
			break;

		case KeyEvent::KEY_SPACE:
			mScene.throwBall();
			break;

		default:
			break;
	}
}

void NibbalApp::mouseDown( MouseEvent event )
{
	if ( !mCameraLock )
		mMayaCam.mouseDown( event.getPos() );
}

void NibbalApp::mouseDrag( MouseEvent event )
{
	if ( !mCameraLock )
		mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}

void NibbalApp::resize( ResizeEvent event )
{
	CameraPersp cam = mMayaCam.getCamera();
	cam.setAspectRatio( getWindowAspectRatio() );
	mMayaCam.setCurrentCam( cam );

	Area dstArea = getWindowBounds();
	Area srcArea = mForegroundLayer.getBounds();
	mForegroundArea = Area::proportionalFit( srcArea, dstArea, false, true );
}

void NibbalApp::eventNoUser()
{
	if( mState == STATE_WIN
	 || mState == STATE_LOSE )
	{
		mStateSave = STATE_IDLE;
		return;
	}

	mState     = STATE_IDLE;
	mStateSave = mState;
	mScene.stopGame();
}

void NibbalApp::eventNewUser()
{
	if( mState == STATE_WIN
	 || mState == STATE_LOSE )
	{
		mStateSave = STATE_GAME;
		return;
	}

	mState     = STATE_GAME;
	mStateSave = mState;
	mScene.startGame();
}

void NibbalApp::eventWin()
{
	mState = STATE_WIN;
	mTimer = (float)App::getElapsedSeconds();
	mScene.stopGame();
}

void NibbalApp::eventLose()
{
	mState = STATE_LOSE;
	mTimer = (float)App::getElapsedSeconds();
	mScene.stopGame();
}

} // namespace Nibbal

//CINDER_APP_BASIC( Nibbal::NibbalApp, RendererGl( RendererGl::AA_NONE ) )
CINDER_APP_BASIC( Nibbal::NibbalApp, RendererGl( RendererGl::AA_MSAA_16 ) )

