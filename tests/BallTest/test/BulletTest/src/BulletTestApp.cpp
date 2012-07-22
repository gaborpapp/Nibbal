// Includes
#include "cinder/app/AppBasic.h"
#include "cinder/gl/Light.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"
#include "cinder/ObjLoader.h"
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
		TT_RING,
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
	void                        checkPoint();

	ci::Surface                 mSurface;
	ci::CameraPersp             mCamera;
	ci::gl::Light              *mLight;

	void                        loadModels();
	ci::TriMesh                 mTorus;

	bullet::CollisionObjectRef  mGround;
	bullet::CollisionObjectRef  mBackBaord;
	bullet::CollisionObjectRef  mTest;
	bullet::CollisionObjectRef  mRing;
	bullet::CollisionObjectRef  mBox;
	btTransform                 mGroundTransform;
	bullet::DynamicsWorldRef    mWorld;

	ci::gl::Texture             mTexGround;
	ci::gl::Texture             mTexBackBoard;
	ci::gl::Texture             mTexRing;
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
	int                         mPoint;
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
	case TT_RING       : mTexRing.bind();       break;
	case TT_BASKETBALL : mTexBasketBall.bind(); break;
	}
}

void BulletTestApp::unbindTexture( TextureType type )
{
	switch( type )
	{
	case TT_GROUND     : mTexGround.unbind();     break;
	case TT_BACKBOARD  : mTexBackBoard.unbind();  break;
	case TT_RING       : mTexRing.unbind();       break;
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
		else if( i == 2 || i == 3 )
			type = TT_RING;
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
//	gl::translate( Vec3f( 0.0f, 13.0f, 0.0f ));
//	gl::rotate( Quatf( 0.0f, 0.0f, 1.14f, 1.0f ));
//	gl::drawTorus( 20, 3, 200, 200 );

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

	// Create the ring box
	{
		mBox = bullet::createRigidBox( mWorld, Vec3f( 5.f, 1.0f, 5.0f ), 0.0f, Vec3f( 0.0f, 75.0f, 247.5f ));
		btRigidBody* boxBody = bullet::toBulletRigidBody( mBox );
		boxBody->setFriction( 0.95f );
		boxBody->setRestitution( .65f );
	}

	// Create the ring
	{
		mRing = bullet::createRigidTorus( mWorld, 0.5f, 10.f, 200, 0.0f, Vec3f( 0.0f, 75.0f, 234.0f ), Quatf( Vec3f::xAxis(), M_PI / 2. ) );
		btRigidBody* boxBody = bullet::toBulletRigidBody( mRing );
		boxBody->setFriction( 0.95f );
		boxBody->setRestitution( .65f );
		
// 		mRing = bullet::createRigidMesh( mWorld, mTorus, Vec3f( 1.0f, 2.0f, 1.0f ), 0.0f, 0.0f, Vec3f( 0.0f, 50.0f, 0.0f ));
// 		btRigidBody* boxBody = bullet::toBulletRigidBody( mRing );
// 		boxBody->setFriction( 0.95f );
// 		boxBody->setRestitution( .65f );

// 		float radiusIn     =  0.5f;
// 		float radiusOut    =  8.0f;
// 		float subdivisions = 36.f;
// 		Vec3f forward( 0.0      , 0.0, 1.0 );
// 		Vec3f side   ( radiusOut, 0.0, 0.0 );
// 
// 		float gap = sqrt( 2.0f * radiusOut * radiusOut - 2.0f * radiusIn * radiusIn * cos(( 2.0f * SIMD_PI ) / subdivisions ));
// 
// 		btTransform t;
// 		for( int x = 0; x < (int)subdivisions; x++ )
// 		{
// 			float angle = ( x * 2.0f * SIMD_PI ) / subdivisions;
// 			Vec3f position = side;
// 			position.rotate( forward, angle );
// 			Quatf q( forward, angle );
// 			
// 			mRing = bullet::createRigidCylinder( mWorld, Vec3f( radiusIn, 3* ( SIMD_PI / subdivisions ) + 0.5f * gap, radiusIn ), subdivisions, 0.0f, position, q );
// 			btRigidBody* boxBody = bullet::toBulletRigidBody( mRing );
// 			boxBody->setFriction( 0.95f );
// 			boxBody->setRestitution( .65f );
// 		}

//		mRing = bullet::createRigidCylinder( mWorld, Vec3f( 1, 1, 1 ), 16, 0.0f, Vec3f( 0, 10, 0 ), Quatf( Vec3f::xAxis(), M_PI / 2 ));

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

void BulletTestApp::loadModels()
{
	ObjLoader loader = ObjLoader( loadResource( RES_OBJ_TORUS )->createStream());
	loader.load( &mTorus );
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
	settings->setWindowSize( 800, 600 );
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

	// Load meshes
	loadModels();

	// Create a Bullet dynamics world
	mWorld = bullet::createWorld();

	// Load texture
	mTexGround     = gl::Texture( loadImage( loadResource( RES_TEX_GROUND     )));
	mTexBackBoard  = gl::Texture( loadImage( loadResource( RES_TEX_BACKBOARD  )));
	mTexRing       = gl::Texture( loadImage( loadResource( RES_TEX_RING       )));
	mTexBasketBall = gl::Texture( loadImage( loadResource( RES_TEX_BASKETBALL )));

	// Parameters
	mFrameRate      = 0.0f;
	mPoint          = 0;
	mViewX          = -27;
	mViewY          = -64;
	mViewZ          = 0;
	mStrengthX      = 0;
	mStrengthY      = 44;
	mStrengthZ      = 33;
// 	mStrengthY      = 0;
// 	mStrengthZ      = 0;
	mParams = params::InterfaceGl( "Params", Vec2i( 200, 250) );
	mParams.addParam( "Frame Rate", &mFrameRate, "", true );
	mParams.addParam( "Point"     , &mPoint    , "", true );
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
	for( bullet::Iter object = mWorld->begin(); object != mWorld->end(); )
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

	checkPoint();
}

void BulletTestApp::checkPoint()
{
	uint32_t i = 0;
	for( bullet::Iter object = mWorld->begin(); object != mWorld->end(); ++object, i++ )
	{
		if( i <= 3 )
			continue;

		if( object->getUserData())
			continue;

		// check vertical position
		if( object->getPosition().y >= mRing->getPosition().y - 1
		 && object->getPosition().y <= mRing->getPosition().y + 1 )
		{
			// check horisontal position

			if( object->getPosition().x >= mRing->getPosition().x - 8
			 && object->getPosition().x <= mRing->getPosition().x + 8
			 && object->getPosition().z <= mRing->getPosition().z + 8
			 && object->getPosition().z <= mRing->getPosition().z + 8 )
			{
				mPoint++;
				object->setUserData( 1 );
			}
		}
	}
}

CINDER_APP_BASIC( BulletTestApp, RendererGl )
