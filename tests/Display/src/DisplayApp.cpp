#include "cinder/app/AppBasic.h"

#include "Display.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace DisplayTest;


class DisplayApp : public AppBasic
{
public:
	void prepareSettings( Settings *settings );
	void setup();
	void shutdown();

	void keyDown( KeyEvent event );
	void resize( ResizeEvent event );
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );

	void update();
	void draw();

private:
	DisplayTest::Display   mDisplay;
	void TimerOver();
};

void DisplayApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 800, 600 );
}

void DisplayApp::setup()
{
	mDisplay.setup();
	mDisplay.addCallback<DisplayApp>( &DisplayApp::TimerOver, this );
}

void DisplayApp::shutdown()
{
}

void DisplayApp::resize( ResizeEvent event )
{
}

void DisplayApp::keyDown( KeyEvent event )
{
	if ( event.getChar() == 'f' )
		setFullScreen( !isFullScreen() );
	if ( event.getCode() == KeyEvent::KEY_ESCAPE )
		quit();

	if ( event.getCode() == KeyEvent::KEY_RETURN )
		mDisplay.setActive( ! mDisplay.getActive());

	if ( event.getCode() == KeyEvent::KEY_SPACE )
	{
		if( mDisplay.isRunning())
			mDisplay.stop();
		else
			mDisplay.start();
	}

	if ( event.getChar() == 'r' )
	{
		mDisplay.setTimeMax( 12, 00 );
		mDisplay.setTimeAct( 11, 50 );
	}
 
	if ( event.getChar() == 'q' )
		mDisplay.setHome( mDisplay.getHome() + 1 );
	if ( event.getChar() == 'w' )
		mDisplay.setPeriod( mDisplay.getPeriod() + 1 );
	if ( event.getChar() == 'e' )
		mDisplay.setGuest( mDisplay.getGuest() + 1 );

	if ( event.getChar() == 'a' )
		mDisplay.setHome( mDisplay.getHome() - 1 );
	if ( event.getChar() == 's' )
		mDisplay.setPeriod( mDisplay.getPeriod() - 1 );
	if ( event.getChar() == 'd' )
		mDisplay.setGuest( mDisplay.getGuest() - 1 );
}

void DisplayApp::mouseDown( MouseEvent event )
{
}

void DisplayApp::mouseDrag( MouseEvent event )
{
}

void DisplayApp::update()
{
	mDisplay.update();
}

void DisplayApp::draw()
{
	gl::clear( Color::black());

	Vec2i pos  = Vec2i( 100, 100 );
	Area  area = Area( pos, Vec2i( pos.x + 300, pos.y + 150 ));

	gl::setMatricesWindow( getWindowSize());
	glEnable( GL_TEXTURE_2D );
	mDisplay.getFbo()->bindTexture();
	gl::color( Color::white());
	gl::draw( mDisplay.getFbo()->getTexture(0), area );
	mDisplay.getFbo()->unbindTexture();
}

void DisplayApp::TimerOver()
{
	console() << "Timer over" << std::endl;
}

CINDER_APP_BASIC( DisplayApp, RendererGl( RendererGl::AA_MSAA_4 ) )
