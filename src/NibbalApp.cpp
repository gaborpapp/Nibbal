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
#include "cinder/gl/gl.h"
#include "cinder/MayaCamUI.h"
#include "AntTweakBar.h"

#include "PParams.h"

#include "Scene.h"
#include "KinectPlayer.h"
#include "AdDisplay.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Nibbal {

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

		KinectPlayer mKinectPlayer;
		Scene mScene;

		MayaCamUI mMayaCam;

		float mFps;
};

void NibbalApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 640, 480 );
}

void NibbalApp::setup()
{
	gl::disableVerticalSync();

	setupParams();

	mScene.setup();
	mKinectPlayer.setup();

	CameraPersp cam;
	cam.setPerspective( 60, getWindowAspectRatio(), 0.1f, 1000.0f );
	cam.setEyePoint( Vec3f( 0, 1, -3 ) );
	cam.setCenterOfInterestPoint( Vec3f( 0, 1, 0 ) );
	mMayaCam.setCurrentCam( cam );

	gl::enableDepthRead();
	gl::enableDepthWrite();
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
	showAllParams( false );
}

void NibbalApp::update()
{
	mKinectPlayer.update();
	mScene.update();

	mFps = getAverageFps();
}

void NibbalApp::draw()
{
	gl::clear( Color::black() );

    gl::setMatrices( mMayaCam.getCamera() );

	mScene.draw();

	mKinectPlayer.draw();

	params::PInterfaceGl::draw();
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

		default:
			break;
	}
}

void NibbalApp::mouseDown( MouseEvent event )
{
	mMayaCam.mouseDown( event.getPos() );
}

void NibbalApp::mouseDrag( MouseEvent event )
{
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}

void NibbalApp::resize( ResizeEvent event )
{
	CameraPersp cam = mMayaCam.getCamera();
	cam.setAspectRatio( getWindowAspectRatio() );
	mMayaCam.setCurrentCam( cam );
}


} // namespace Nibbal

//CINDER_APP_BASIC( Nibbal::NibbalApp, RendererGl( RendererGl::AA_NONE ) )
CINDER_APP_BASIC( Nibbal::NibbalApp, RendererGl( RendererGl::AA_MSAA_16 ) )

