#include "cinder/app/AppBasic.h"
#include "cinder/rand.h"

#include "Crowd.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CrowdApp : public AppBasic
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
	Crowd mCrowd;
};

void CrowdApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 800, 600 );
}

void CrowdApp::setup()
{
	mCrowd.setup( 800, 600 );
}

void CrowdApp::shutdown()
{
}

void CrowdApp::resize( ResizeEvent event )
{
}

void CrowdApp::keyDown( KeyEvent event )
{
	if ( event.getChar() == 'f' )
		setFullScreen( !isFullScreen() );
	if ( event.getCode() == KeyEvent::KEY_ESCAPE )
		quit();
}

void CrowdApp::mouseDown( MouseEvent event )
{
	if( event.isLeftDown())  mCrowd.energize( Rand::randFloat( 1.0f, 3.0f ));
	if( event.isRightDown()) mCrowd.wave    ( Rand::randFloat( 1.0f, 3.0f ));
}

void CrowdApp::mouseDrag( MouseEvent event )
{
}

void CrowdApp::update()
{
	mCrowd.update();
}

void CrowdApp::draw()
{
	gl::clear( Color::black() );
	gl::setMatricesWindow( getWindowSize());
	gl::enableAlphaBlending();

	glPushMatrix();
	glTranslatef( getWindowSize().x / 2, getWindowSize().y / 2.  /*+ 100*/,0 );
	mCrowd.draw();
	glPopMatrix();

	gl::disableAlphaBlending();
}

CINDER_APP_BASIC( CrowdApp, RendererGl( RendererGl::AA_NONE ) )

