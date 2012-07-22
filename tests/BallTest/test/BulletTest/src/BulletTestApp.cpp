#pragma once

// Includes
#include "cinder/app/AppBasic.h"
#include "cinder/gl/Light.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"
#include "cinder/params/Params.h"
#include "cinder/Utilities.h"
#include "CinderBullet.h"
#include "Resources.h"

// Bullet physics sample application
class BulletTestApp : public ci::app::AppBasic
{
	enum TextureType
	{
		TT_GROUND,
		TT_BACKBOARD,
		TT_BASKETBALL,
	};

public:

	void      draw();
	void      keyDown   ( ci::app::KeyEvent   event );
	void      mouseDown ( ci::app::MouseEvent event );
	void      mouseWheel( ci::app::MouseEvent event );
	void      prepareSettings( ci::app::AppBasic::Settings *settings );
	void      resize( ci::app::ResizeEvent event );
	void      setup();
	void      shutdown();
	void      update();
	
private:
	void                        initTest();

	ci::Surface                 mSurface;
	ci::CameraPersp             mCamera;
	ci::gl::Light              *mLight;

	bullet::CollisionObjectRef  mGround;
	bullet::CollisionObjectRef  mBackBaord;
	bullet::CollisionObjectRef  mTest;
	bullet::CollisionObjectRef  mRing;
	btTransform                 mGroundTransform;
	bullet::DynamicsWorldRef    mWorld;

	ci::gl::Texture             mTexGround;
	ci::gl::Texture             mTexBackBoard;
	ci::gl::Texture             mTexBasketBall;
	
	void                        bindTexture  ( TextureType type );
	void                        unbindTexture( TextureType type );

	float                       mFrameRate;
	int                         mViewX;
	int                         mViewY;
	int                         mViewZ;
	int                         mStrengthX;
	int                         mStrengthY;
	int                         mStrengthZ;
	ci::params::InterfaceGl     mParams;

};

using namespace bullet;
using namespace ci;
using namespace ci::app;
using namespace std;

void BulletTestApp::bindTexture( TextureType type )
{
	switch( type )
	{
	case TT_GROUND     : mTexGround.bind();     break;
	case TT_BACKBOARD  : mTexBackBoard.bind();  break;
	case TT_BASKETBALL : mTexBasketBall.bind(); break;
	}
}

void BulletTestApp::unbindTexture( TextureType type )
{
	switch( type )
	{
	case TT_GROUND     : mTexGround.unbind();     break;
	case TT_BACKBOARD  : mTexBackBoard.unbind();  break;
	case TT_BASKETBALL : mTexBasketBall.unbind(); break;
	}
}

void BulletTestApp::draw()
{
	gl::setViewport( getWindowBounds() );
	gl::clear( ColorAf::black() );
	gl::setMatrices( mCamera );
	gl::pushMatrices();
	gl::rotate( Vec3f( (float)mViewX, (float)mViewY, (float)mViewZ ) );
	uint32_t i = 0;
	for( bullet::Iter object = mWorld->begin(); object != mWorld->end(); ++object, i++ )
	{
		TextureType type = TT_GROUND;

		if( i == 0 )
			type = TT_GROUND;
		else if( i == 1 )
			type = TT_BACKBOARD;
		else
			type = TT_BASKETBALL;

		gl::pushMatrices();
		glMultMatrixf( object->getTransformMatrix() );
		bindTexture( type );
		gl::draw( object->getVboMesh() );
		unbindTexture( type );
		gl::popMatrices();
	}

	// draw the same torus
// 	gl::translate( Vec3f( 0.0f, 10.0f, 0.0f ));
// 	gl::rotate( Quatf( 0.0f, 0.0f, 1.14f, 1.0f ));
// 	gl::drawTorus( 20, 3, 12, 12 );

	gl::popMatrices();

	mParams.draw();
}

void BulletTestApp::initTest()
{
	// Clean up last test
	if ( mWorld )
	{
		mWorld->clear();
	}

	mGroundTransform.setIdentity();

	// Create the ground
	{
		mGround = bullet::createRigidBox( mWorld, Vec3f( 357.0f, 1.0f, 600.0f ), 0.0f );
		btRigidBody* boxBody = bullet::toBulletRigidBody( mGround );
		boxBody->setFriction( 0.95f );
		boxBody->setRestitution( .65f );
	}

	// Create the backboard
	{
		mBackBaord = bullet::createRigidBox( mWorld, Vec3f( 110.0f, 70.0f, 1.0f ), 0.0f, Vec3f( 0.0f, 100.f, 250.f ));
		btRigidBody* boxBody = bullet::toBulletRigidBody( mBackBaord );
		boxBody->setFriction( 0.95f );
		boxBody->setRestitution( .65f );
	}

// 	{
// 		mTest = bullet::createRigidCylinder( mWorld, Vec3f( 10.0f, 50.0f, 10.0f ), 16, 0.0f, Vec3f( 0.0f, 50.0f, 0.0f ));
// 		btRigidBody* boxBody = bullet::toBulletRigidBody( mBackBaord );
// 		boxBody->setFriction( 0.95f );
// 		boxBody->setRestitution( .65f );
// 	}

	// Create the ring
	{
// 		mRing = bullet::createRigidTorus( mWorld, 3.0f, 20.f, 200, 0.0f, Vec3f( 0.0f, 10.0f, 0.0f ), Quatf( 0.0f, 0.0f, 1.14f, 1.0f ));
// 		btRigidBody* boxBody = bullet::toBulletRigidBody( mRing );
// 		boxBody->setFriction( 0.95f );
// 		boxBody->setRestitution( .65f );
	}
}

void BulletTestApp::keyDown( KeyEvent event )
{
	switch ( event.getCode() )
	{
	case KeyEvent::KEY_ESCAPE:
		quit();
		break;
	case KeyEvent::KEY_f:
		if ( !isFullScreen() )
			setFullScreen( true );
		else
			setFullScreen( false );
		break;
	case KeyEvent::KEY_SPACE:
		writeImage( getAppPath() / string( "frame_" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
		break;
	}
}

// Handles mouse button press
void BulletTestApp::mouseDown( MouseEvent event )
{
	float size = 5.0f;
	Vec3f position = Vec3f( 0.0f, 50.0f, 0.0f );

	CollisionObjectRef body;
	btRigidBody* shape;

	body = bullet::createRigidSphere( mWorld, size, 32, size * size, position );
	shape = bullet::toBulletRigidBody( body );
	shape->setAngularFactor( 0.82f );
	shape->setRestitution( .75f );
	shape->setLinearVelocity( btVector3( (float)mStrengthX, (float)mStrengthY, (float)mStrengthZ ));
}

void BulletTestApp::mouseWheel( MouseEvent event )
{
	mCamera.setEyePoint( mCamera.getEyePoint() + Vec3f( 0.0f, 0.0f, event.getWheelIncrement() * 20.0f ) );
}

void BulletTestApp::prepareSettings( Settings * settings )
{
	settings->setFrameRate( 1000.0f );
	settings->setFullScreen( false );
	settings->setResizable( false );
	settings->setWindowSize( 1280, 720 );
}

void BulletTestApp::resize( ResizeEvent event )
{
	// Reset camera
	mCamera.setPerspective( 60.0f, getWindowAspectRatio(), 1.0f, 5000.0f );
	mCamera.lookAt( Vec3f( 0.0f, 0.0f, -300.0f ), Vec3f::zero() );
	gl::setMatrices( mCamera );

	// Set up OpenGL
	glEnable( GL_BLEND );
	glEnable( GL_DEPTH_TEST );
	gl::enable( GL_TEXTURE_2D );
	glShadeModel( GL_SMOOTH );
	glEnable( GL_POLYGON_SMOOTH );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
	glEnable( GL_LIGHTING );
	glEnable( GL_NORMALIZE );
	gl::enableAlphaBlending();
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void BulletTestApp::setup()
{
	// Set up lighting
	mLight = new gl::Light( gl::Light::DIRECTIONAL, 0 );
	mLight->setDirection( Vec3f( 0.0f, 0.1f, 0.3f ).normalized() );
	mLight->setAmbient( ColorAf( 0.2f, 0.2f, 0.2f, 1.0f ) );
	mLight->setDiffuse( ColorAf( 1.0f, 1.0f, 1.0f, 1.0f ) );
	mLight->enable();

	// Create a Bullet dynamics world
	mWorld = bullet::createWorld();

	// Load texture
	mTexGround     = gl::Texture( loadImage( loadResource( RES_TEX_GROUND     )));
	mTexBackBoard  = gl::Texture( loadImage( loadResource( RES_TEX_BACKBOARD  )));
	mTexBasketBall = gl::Texture( loadImage( loadResource( RES_TEX_BASKETBALL )));

	// Parameters
	mFrameRate      = 0.0f;
	mViewX          = -27;
	mViewY          = -64;
	mViewZ          = 0;
	mStrengthX      = 0;
	mStrengthY      = 44;
	mStrengthZ      = 33;
	mParams = params::InterfaceGl( "Params", Vec2i( 200, 250) );
	mParams.addParam( "Frame Rate", &mFrameRate, "", true );
	mParams.addText ( "View" );
	mParams.addParam( "view_x"             , &mViewX, "min=-100 max=100 step=1 keyIncr=7 keyDecr=4" );
	mParams.addParam( "view_y"             , &mViewY, "min=-100 max=100 step=1 keyIncr=8 keyDecr=5" );
	mParams.addParam( "view_z"             , &mViewZ, "min=-100 max=100 step=1 keyIncr=9 keyDecr=6" );
	mParams.addText ( "Strength" );
	mParams.addParam( "strength_x"         , &mStrengthX, "min=-100 max=100 step=1 keyIncr=q keyDecr=a" );
	mParams.addParam( "strength_y"         , &mStrengthY, "min=-100 max=100 step=1 keyIncr=w keyDecr=s" );
	mParams.addParam( "strength_z"         , &mStrengthZ, "min=-100 max=100 step=1 keyIncr=e keyDecr=d" );

	// Initialize
	initTest();

	// Run first resize to initialize view
	resize( ResizeEvent( getWindowSize() ) );
}

void BulletTestApp::shutdown()
{
	// Clean up
	if ( mLight )
	{
		delete mLight;
	}

	// TO DO remove
	exit( 1 );
}

void BulletTestApp::update()
{
	mFrameRate = getAverageFps();

	// Update light
	mLight->update( mCamera );

// 		// Set box rotation
// 		float rotation = math<float>::sin( ( float )getElapsedSeconds() * 0.3333f ) * 0.35f	;
// 		mGroundTransform.setRotation( btQuaternion( 0.25f, 0.0f, 1.0f + rotation * 0.1f, rotation ) );
// 		mGroundTransform.setOrigin( btVector3( 0.0f, -60.0f, 0.0f ) );
// 
// 		// Apply rotation to box
// 		btRigidBody* body = bullet::toBulletRigidBody( mGround );
// 		body->getMotionState()->setWorldTransform( mGroundTransform );
// 		body->setWorldTransform( mGroundTransform );


	// Update dynamics world
	mWorld->update( mFrameRate );

	// Remove out of bounds objects
	for ( bullet::Iter object = mWorld->begin(); object != mWorld->end(); )
	{
		if ( object != mWorld->begin() && object->getPosition().y < -800.0f )
		{
			object = mWorld->erase( object );
		}
		else
		{
			++object;
		}
	}
}

CINDER_APP_BASIC( BulletTestApp, RendererGl )
