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
	mTime = (float)ci::app::getElapsedSeconds();
	mIdealThrow.setup();

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

	Vec3f velIdeal = mIdealThrow.calcDirection( pos );
//	Vec3f velIdeal = vel;

	mBall = bullet::createRigidSphere( mWorld, size, 32, 1.0f, pos );
	btRigidBody* body = bullet::toBulletRigidBody( mBall );
	body->setAngularFactor( 0.82f );
	body->setRestitution( .75f );
	body->setLinearVelocity( btVector3( velIdeal.x, velIdeal.y, velIdeal.z ));
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
	float time = (float)ci::app::getElapsedSeconds();
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

Cube::Cube( Vec3f pos000, float size )
: mCenter( pos000 + Vec3f( size / 2, size / 2, size / 2 ))
, mSize( size )
{
}

void Cube::addDirection( CornerType cornerType, ci::Vec3f direction )
{
	mDirections.insert( make_pair( cornerType, shared_ptr< Vec3f >( new Vec3f( direction ))));
}

Vec3f Cube::getCorner( CornerType cornerType )
{
	float offset = mSize / 2;

	switch( cornerType )
	{
	case C_000 : return mCenter + Vec3f( -offset, -offset, -offset ); break; // left  lower back
	case C_001 : return mCenter + Vec3f( -offset, -offset,  offset ); break; // left  lower front
	case C_100 : return mCenter + Vec3f(  offset, -offset, -offset ); break; // right lower back
	case C_101 : return mCenter + Vec3f(  offset, -offset,  offset ); break; // right lower front
	case C_010 : return mCenter + Vec3f( -offset,  offset, -offset ); break; // left  upper back
	case C_011 : return mCenter + Vec3f( -offset,  offset,  offset ); break; // left  upper front
	case C_110 : return mCenter + Vec3f(  offset,  offset, -offset ); break; // right upper back
	case C_111 : return mCenter + Vec3f(  offset,  offset,  offset ); break; // right upper front
	}

	return mCenter;
}

bool Cube::isContain( Vec3f pos )
{
	Vec3f pos000 = getCorner( C_000 );
	Vec3f pos011 = getCorner( C_011 );
	Vec3f pos101 = getCorner( C_101 );

	if( pos.x >= pos000.x
	 && pos.x <= pos101.x
	 && pos.y >= pos000.y
	 && pos.y <= pos011.y
	 && pos.z >= pos000.z
	 && pos.z <= pos101.z )
		return true;

	return false;
}

Vec3f Cube::getInterpolation( Vec3f pos )
{
	Vec3f pos000 = getCorner( C_000 );
	Vec3f pos001 = getCorner( C_001 );
	Vec3f pos010 = getCorner( C_010 );
	Vec3f pos011 = getCorner( C_011 );
	Vec3f pos100 = getCorner( C_100 );
	Vec3f pos101 = getCorner( C_101 );
	Vec3f pos110 = getCorner( C_110 );
	Vec3f pos111 = getCorner( C_111 );

	float x0 = pos000.x;
	float x1 = pos100.x;
	float y0 = pos000.y;
	float y1 = pos010.y;
	float z0 = pos000.z;
	float z1 = pos001.z;

	float xd = ( pos.x - x0 ) / ( x1 - x0 );
	float yd = ( pos.y - y0 ) / ( y1 - y0 );
	float zd = ( pos.z - z0 ) / ( z1 - z0 );

	Vec3f c00 = _getDirection( C_000 ) * ( 1 - xd ) + _getDirection( C_100 ) * xd;
	Vec3f c10 = _getDirection( C_010 ) * ( 1 - xd ) + _getDirection( C_110 ) * xd;
	Vec3f c01 = _getDirection( C_001 ) * ( 1 - xd ) + _getDirection( C_101 ) * xd;
	Vec3f c11 = _getDirection( C_011 ) * ( 1 - xd ) + _getDirection( C_111 ) * xd;

	Vec3f c0  = c00 * ( 1 - yd ) + c10 * yd;
	Vec3f c1  = c01 * ( 1 - yd ) + c11 * yd;

	Vec3f c   = c0  * ( 1 - zd ) + c1  * zd;

	app::console() << " pos: " << pos << " vec: " << c << endl;

	return c;
}

Vec3f Cube::_getDirection( CornerType cornerType )
{
	map< CornerType, shared_ptr< Vec3f > >::iterator it = mDirections.lower_bound( cornerType );

	if( it != mDirections.end() && !( mDirections.key_comp()( cornerType, it->first )))
	{
		return *(it->second);
	}

	return Vec3f( 0.0f, 0.0f, 0.0f );
}

void IdealThrow::setup()
{
	shared_ptr< Cube > cube = shared_ptr< Cube >( new Cube( Vec3f( 0.0f, 2.0f, 0.0f ), 0.5f ));

	mCubes.push_back( cube );

//	mDirections.insert( make_pair( shared_ptr< Vec3f >( new Vec3f( 0.0f, 2.0f, 1.0f ), shared_ptr< Vec3f >( new Vec3f(  0.0f, 6.0f, 3.6f )));

	cube->addDirection( Cube::C_000, Vec3f(  0.0f, 6.5f, 4.7f  ));
	cube->addDirection( Cube::C_001, Vec3f(  0.0f, 6.5f, 4.4f  ));
	cube->addDirection( Cube::C_101, Vec3f( -0.4f, 6.5f, 4.4f  ));
	cube->addDirection( Cube::C_100, Vec3f( -0.4f, 6.5f, 4.7f  ));

	cube->addDirection( Cube::C_010, Vec3f(  0.0f, 6.2f, 4.63f ));
	cube->addDirection( Cube::C_011, Vec3f(  0.0f, 6.1f, 4.35f ));
	cube->addDirection( Cube::C_111, Vec3f( -0.4f, 6.1f, 4.35f ));
	cube->addDirection( Cube::C_110, Vec3f( -0.4f, 6.1f, 4.63f ));
}

Vec3f IdealThrow::calcDirection( Vec3f pos )
{
	Cube *cube = _getCube( pos );

	if( cube )
	{
		return cube->getInterpolation( pos );
	}

	app::console() << "no cube found" << endl;
	return Vec3f( 0.0f, 0.0f, 0.0f );
}

Cube *IdealThrow::_getCube( Vec3f dir )
{
	for( vector< shared_ptr< Cube > >::iterator p = mCubes.begin(); p != mCubes.end(); ++p )
	{
		if( (*p)->isContain( dir ))
			return (*p).get();
	}

	return 0;
}

} // namespace Nibbal
