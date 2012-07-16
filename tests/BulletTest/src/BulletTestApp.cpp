/*
 Copyright (C) 2012 Gabor Papp

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
#include "cinder/params/Params.h"

#include "btBulletDynamicsCommon.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const btVector3 toBullet( const Vec3f& v )
{
	return btVector3( v.x, v.y, v.z );
}

const btQuaternion toBullet( const Quatf& q )
{
	return btQuaternion( q.v.x, q.v.y, q.v.z, q.w );
}

const btTransform toBulletTransform( const ci::Matrix44f& m )
{
	btTransform trans;
	trans.setFromOpenGLMatrix( m.m );
	return trans;
}

Matrix44f fromBullet( const btTransform &m )
{
	Matrix44f matrix;
	m.getOpenGLMatrix( matrix.m );
	return matrix;
}

class BulletTestApp : public AppBasic
{
	public:
		void prepareSettings( Settings *settings );
		void setup();

		void keyDown( KeyEvent event );

		void update();
		void draw();

	private:
		params::InterfaceGl mParams;

		shared_ptr< btBroadphaseInterface > mBroadphaseRef;
		shared_ptr< btDefaultCollisionConfiguration > mCollisionConfigurationRef;
		shared_ptr< btCollisionDispatcher > mDispatcherRef;
		shared_ptr< btSequentialImpulseConstraintSolver > mSolverRef;
		shared_ptr< btDiscreteDynamicsWorld > mDynamicsWorldRef;

		shared_ptr< btCollisionShape > mGroundShapeRef;
		shared_ptr< btCollisionShape > mFallShapeRef;

		shared_ptr< btDefaultMotionState > mGroundMotionStateRef;
		shared_ptr< btRigidBody > mGroundRigidBodyRef;

		shared_ptr< btDefaultMotionState > mFallMotionStateRef;
		shared_ptr< btRigidBody > mFallRigidBodyRef;

		float mFps;
};

void BulletTestApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 640, 480 );
}

void BulletTestApp::setup()
{
	gl::disableVerticalSync();

	mParams = params::InterfaceGl( "Parameters", Vec2i( 200, 300 ) );

	mParams.addParam( "Fps", &mFps, "", true );

	gl::enableDepthRead();
	gl::enableDepthWrite();

	mBroadphaseRef = shared_ptr< btBroadphaseInterface >( new btDbvtBroadphase() );
	mCollisionConfigurationRef = shared_ptr< btDefaultCollisionConfiguration >( new btDefaultCollisionConfiguration() );
	mDispatcherRef = shared_ptr< btCollisionDispatcher >( new btCollisionDispatcher( mCollisionConfigurationRef.get() ) );
	mSolverRef = shared_ptr< btSequentialImpulseConstraintSolver >( new btSequentialImpulseConstraintSolver );
	mDynamicsWorldRef = shared_ptr< btDiscreteDynamicsWorld >( new btDiscreteDynamicsWorld(
				mDispatcherRef.get(),
				mBroadphaseRef.get(),
				mSolverRef.get(),
				mCollisionConfigurationRef.get() ) );

	mDynamicsWorldRef->setGravity( btVector3( 0, -10, 0 ) );

	mGroundShapeRef = shared_ptr< btCollisionShape >(
			new btStaticPlaneShape( btVector3( 0, 1, 0 ), 1 ) );

	mFallShapeRef = shared_ptr< btCollisionShape >( new btSphereShape( 1 ) );

	mGroundMotionStateRef = shared_ptr< btDefaultMotionState >(
			new btDefaultMotionState( btTransform( btQuaternion( 0, 0, 0, 1 ),
												   btVector3( 0 , -1, 0 ) ) ) );
	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI( 0, mGroundMotionStateRef.get(),
							mGroundShapeRef.get(),
							btVector3( 0, 0, 0 ) );
	mGroundRigidBodyRef = shared_ptr< btRigidBody >( new btRigidBody( groundRigidBodyCI ) );
	mDynamicsWorldRef->addRigidBody( mGroundRigidBodyRef.get() );

	mFallMotionStateRef = shared_ptr< btDefaultMotionState >(
			new btDefaultMotionState( btTransform( btQuaternion( 0, 0, 0, 1 ),
												   btVector3( 0, 50, 0 ) ) ) );
	btScalar mass = 1;
	btVector3 fallInertia( 0, 0, 0 );
	mFallShapeRef->calculateLocalInertia( mass, fallInertia );
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI( mass,
			mFallMotionStateRef.get(), mFallShapeRef.get(), fallInertia );
	mFallRigidBodyRef = shared_ptr< btRigidBody >( new btRigidBody( fallRigidBodyCI ) );
	mDynamicsWorldRef->addRigidBody( mFallRigidBodyRef.get() );
}

void BulletTestApp::update()
{
	mDynamicsWorldRef->stepSimulation( 1 / 60.f, 10 );

	mFps = getAverageFps();
}

void BulletTestApp::draw()
{
	gl::clear( Color::black() );

	btTransform btTrans;
	mFallRigidBodyRef->getMotionState()->getWorldTransform( btTrans );

	console() << "sphere: " << fromBullet( btTrans ).getTranslate() << endl;

	params::InterfaceGl::draw();
}

void BulletTestApp::keyDown( KeyEvent event )
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

		case KeyEvent::KEY_ESCAPE:
			quit();
			break;

		default:
			break;
	}
}

CINDER_APP_BASIC( BulletTestApp, RendererGl( RendererGl::AA_NONE ) )

