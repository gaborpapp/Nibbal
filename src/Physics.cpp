#include "cinder/app/App.h"
#include <assert.h>

#include "Physics.h"

using namespace ci;
using namespace std;

namespace Nibbal {

static const float BALL_RADIUS = 0.12f;
static const Vec3f RING_POS    = Vec3f( 0.0f, 2.99f, 4.64f );
static const float RING_RADIUS = 0.278f;

void Physics::setup()
{
	mWorld = bullet::createWorld();
	mBall = 0;
	mTime = ci::app::getElapsedSeconds();

	mWorld->getWorld()->setGravity( btVector3( 0.0f, -9.8f, 0.0f ));
}

void Physics::draw()
{
// 	for( bullet::Iter object = mWorld->begin(); object != mWorld->end(); ++object )
// 	{
// 		gl::pushModelView();
// 		gl::multModelView( object->getTransformMatrix());
// 		gl::draw( object->getVboMesh());
// 		gl::popModelView();
// 	}
}

void Physics::throwBall( Vec3f pos, Vec3f vel )
{
	float size = BALL_RADIUS;

	if( mBall )
	{
		mWorld->erase( mBall );
		mBall = 0;
	}

	mBall = bullet::createRigidSphere( mWorld, size, 32, 1.0f, pos );
	btRigidBody* body = bullet::toBulletRigidBody( mBall );
	body->setAngularFactor( 0.82f );
	body->setRestitution( .75f );
	body->setLinearVelocity( btVector3( vel.x, vel.y, vel.z ));
}

Vec3f Physics::getRingPos()
{
	return RING_POS;
}

float Physics::getRingRadius()
{
	return RING_RADIUS;
}

Vec3f Physics::getBallPos()
{
	if( mBall )
		return mBall->getPosition();

	return Vec3f();
}

Matrix44f Physics::getBallMatrix()
{
	if( mBall )
	{
		Matrix44f ball = mBall->getTransformMatrix();
		Matrix44f inversScale = Matrix44f::createScale( Vec4f( BALL_RADIUS, BALL_RADIUS, BALL_RADIUS, 1.0f )).inverted();

		return ball * inversScale;
	}

	return Matrix44f();
}

void Physics::addMesh( TriMesh mesh )
{
	bullet::CollisionObjectRef obj = bullet::createRigidMesh( mWorld, mesh, Vec3f( 1.0f, 1.0f, 1.0f ), 0.0f, 0.0f );
	btRigidBody* body = bullet::toBulletRigidBody( obj );
	body->setFriction( 0.95f );
	body->setRestitution( .65f );
}

void Physics::addBox( Vec3f size, Vec3f translate )
{
	bullet::CollisionObjectRef obj = bullet::createRigidBox( mWorld, size, 0.0f, translate );
	btRigidBody* body = bullet::toBulletRigidBody( obj );
	body->setFriction( 0.95f );
	body->setRestitution( .65f );
}

void Physics::update( float fps )
{
//	mWorld->update( 240 );
	float time = ci::app::getElapsedSeconds();
	mWorld->update( time - mTime, fps );
	mTime = time;
}

Vec3f Physics::getPerfectDirection( Vec3f src, Vec3f dst )
{
	float x = dst.x - src.x;
	float y = ( dst.y - src.y ) * 6;
	float z = ( dst.z - src.z );

	return Vec3f( x, y, z );
}

} // namespace Nibbal
