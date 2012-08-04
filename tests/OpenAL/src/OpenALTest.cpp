#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

#include "OpenALAudio.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class OpenALTest : public AppBasic
{
	public:
		void prepareSettings( Settings *settings );
		void setup();

		void keyDown( KeyEvent event );

		void update();
		void draw();

	private:
		params::InterfaceGl mParams;

		mndl::openal::OpenALAudio mAudio;
		unsigned mSampleId;
};

void OpenALTest::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 640, 480 );
}

void OpenALTest::setup()
{
	gl::disableVerticalSync();

	mSampleId = mAudio.load( getAssetPath( "buzzer.ogg" ) );
	mParams = params::InterfaceGl( "Parameters", Vec2i( 200, 300 ) );
}

void OpenALTest::update()
{
}

void OpenALTest::draw()
{
	gl::clear( Color::black() );

	params::InterfaceGl::draw();
}

void OpenALTest::keyDown( KeyEvent event )
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
			mParams.show( !mParams.isVisible() );
			if ( isFullScreen() )
			{
				if ( mParams.isVisible() )
					showCursor();
				else
					hideCursor();
			}
			break;

		case KeyEvent::KEY_SPACE:
			mAudio.play( mSampleId );
			break;

		case KeyEvent::KEY_ESCAPE:
			quit();
			break;

		default:
			break;
	}
}

CINDER_APP_BASIC( OpenALTest, RendererGl( RendererGl::AA_NONE ) )

