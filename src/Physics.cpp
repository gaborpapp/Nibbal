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
	mGridThrow = shared_ptr< Grid >( new Grid );

	mWorld->getWorld()->setGravity( btVector3( 0.0f, -9.8f, 0.0f ));
}

void Physics::draw()
{
	if ( mDrawGrid )
		mGridThrow->draw();
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

	Vec3f velIdeal = mGridThrow->calcDirection( pos );
	if( velIdeal == Vec3f())
		velIdeal = vel;
	//velIdeal = vel;

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
	//mWorld->update( 1, 240 );
	mTime = time;
}

Grid::Grid()
: mPos000( Vec3f())
, mNumCorner( Vec3i())
, mSize( 0 )
, mDirections( 0 )
{
	_setup( Vec3f( -0.5f, 1.5f, 0.0f ), Vec3i( 3, 3, 3 ), 0.5f);

	setDirection( Vec3i( 0, 0, 0 ), Vec3f( 0.45f, 6.9f, 4.8f   ));
	setDirection( Vec3i( 1, 0, 0 ), Vec3f(  0.0f, 6.9f, 4.8f   ));
	setDirection( Vec3i( 2, 0, 0 ), Vec3f( -0.4f, 6.9f, 4.8f   ));

	setDirection( Vec3i( 0, 1, 0 ), Vec3f(  0.45f, 6.5f, 4.7f  ));
	setDirection( Vec3i( 1, 1, 0 ), Vec3f(  0.0f , 6.5f, 4.7f  ));
	setDirection( Vec3i( 2, 1, 0 ), Vec3f( -0.4f , 6.5f, 4.7f  ));

	setDirection( Vec3i( 0, 2, 0 ), Vec3f(  0.45f, 6.1f, 4.7f  ));
	setDirection( Vec3i( 1, 2, 0 ), Vec3f(  0.0f , 6.2f, 4.63f ));
	setDirection( Vec3i( 2, 2, 0 ), Vec3f( -0.4f , 6.1f, 4.63f ));

	setDirection( Vec3i( 0, 0, 1 ), Vec3f( 0.45f, 6.75f, 4.5f  ));
	setDirection( Vec3i( 1, 0, 1 ), Vec3f(  0.0f, 6.75f, 4.5f  ));
	setDirection( Vec3i( 2, 0, 1 ), Vec3f( -0.4f, 6.75f, 4.5f  ));

	setDirection( Vec3i( 0, 1, 1 ), Vec3f(  0.45f, 6.5f, 4.2f  ));
	setDirection( Vec3i( 1, 1, 1 ), Vec3f(  0.0f , 6.5f, 4.4f  ));
	setDirection( Vec3i( 2, 1, 1 ), Vec3f( -0.4f , 6.5f, 4.4f  ));

	setDirection( Vec3i( 0, 2, 1 ), Vec3f(  0.45f, 6.1f, 4.2f  ));
	setDirection( Vec3i( 1, 2, 1 ), Vec3f(  0.0f , 6.1f, 4.35f ));
	setDirection( Vec3i( 2, 2, 1 ), Vec3f( -0.4f , 6.1f, 4.35f ));

	setDirection( Vec3i( 0, 0, 2 ), Vec3f(  0.55f, 6.55f, 4.3f ));
	setDirection( Vec3i( 1, 0, 2 ), Vec3f(  0.0f , 6.55f, 4.3f ));
	setDirection( Vec3i( 2, 0, 2 ), Vec3f( -0.55f, 6.55f, 4.3f ));

	setDirection( Vec3i( 0, 1, 2 ), Vec3f(  0.55f, 6.2f, 4.1f  ));
	setDirection( Vec3i( 1, 1, 2 ), Vec3f(  0.0f , 6.2f, 4.1f  ));
	setDirection( Vec3i( 2, 1, 2 ), Vec3f( -0.55f, 6.2f, 4.1f  ));

	setDirection( Vec3i( 0, 2, 2 ), Vec3f(  0.55f, 6.0f, 3.8f  ));
	setDirection( Vec3i( 1, 2, 2 ), Vec3f(  0.0f , 6.0f, 3.8f  ));
	setDirection( Vec3i( 2, 2, 2 ), Vec3f( -0.55f, 6.0f, 3.8f  ));
}

Grid::~Grid()
{
	_freeArray();
}

void Grid::setDirection( Vec3i corner, Vec3f direction )
{
	if( ! _isValidCorner( corner ))
		return;

	mDirections[corner.x][corner.y][corner.z] = direction;
}

Vec3f Grid::getDirection( Vec3i corner )
{
	if( ! _isValidCorner( corner ))
		return Vec3f( 0.0f, 0.0f, 0.0f );

	return mDirections[corner.x][corner.y][corner.z];
}

Vec3f Grid::calcDirection( ci::Vec3f pos )
{
	Vec3i corner = _getCorner( pos );

	if( ! _isValidCorner( corner ))
	{
		app::console() << "no section found" << endl;
		return Vec3f( 0.0f, 0.0f, 0.0f );
	}

	return _getInterpolation( corner, pos );
}

void Grid::draw()
{
	for( int x = 0; x < mNumCorner.x; ++x )
	{
		for( int y = 0; y < mNumCorner.y; ++y )
		{
			for( int z = 0; z < mNumCorner.z; ++z )
			{
				Vec3f corner = Vec3f( mPos000.x + x * mSize
				                    , mPos000.y + y * mSize
				                    , mPos000.z + z * mSize );

				Vec3f dir = getDirection( Vec3i( x, y, z ) );
				gl::color( Color::white() );
				gl::drawSphere( corner, .01f );
				gl::color( Color( 1, 0, 0 ) );
				gl::drawVector( corner, corner + .1 * dir );
			}
		}
	}
}

void Grid::_allocArray()
{
	if( mDirections )
		return;

	mDirections = new Vec3f** [mNumCorner.x];
	for( int x = 0; x < mNumCorner.x; ++x )
	{
		mDirections[x] = new Vec3f* [mNumCorner.y];

		for( int y = 0; y < mNumCorner.y; ++y )
		{
			mDirections[x][y] = new Vec3f[mNumCorner.z];

			for( int z = 0; z < mNumCorner.z; ++z )
			{
				mDirections[x][y][z] = Vec3f();
			}
		}
	}
}

void Grid::_freeArray()
{
	if( ! mDirections )
		return;

	for( int x = 0; x < mNumCorner.x; ++x )
	{
		for( int y = 0; y < mNumCorner.y; ++y )
		{
			delete [] mDirections[x][y];
		}

		delete [] mDirections[x];
	}

	delete [] mDirections;
}

void Grid::_setup( Vec3f pos000, Vec3i numCorner, float size )
{
	mPos000     = pos000;
	mNumCorner  = numCorner;
	mSize       = size;

	_allocArray();
}

bool Grid::_isValidCorner( ci::Vec3i corner )
{
	if( corner.x <  0
	 || corner.x >= mNumCorner.x
	 || corner.y <  0
	 || corner.y >= mNumCorner.y
	 || corner.z <  0
	 || corner.z >= mNumCorner.z )
		return false;

	return true;
}

Vec3i Grid::_getCorner( Vec3f pos )
{
	Vec3f posLocal = pos - mPos000;
	Vec3i corner = Vec3i( (int)( posLocal.x / mSize )
	                    , (int)( posLocal.y / mSize )
	                    , (int)( posLocal.z / mSize ));

	return corner;
}

Vec3i Grid::_getCornerOffset( Vec3i corner, Offset offset )
{
	switch( offset )
	{
	case O_000 : return corner + Vec3i( 0, 0, 0 ); break; // left  lower back
	case O_001 : return corner + Vec3i( 0, 0, 1 ); break; // left  lower front
	case O_100 : return corner + Vec3i( 1, 0, 0 ); break; // right lower back
	case O_101 : return corner + Vec3i( 1, 0, 1 ); break; // right lower front
	case O_010 : return corner + Vec3i( 0, 1, 0 ); break; // left  upper back
	case O_011 : return corner + Vec3i( 0, 1, 1 ); break; // left  upper front
	case O_110 : return corner + Vec3i( 1, 1, 0 ); break; // right upper back
	case O_111 : return corner + Vec3i( 1, 1, 1 ); break; // right upper front
	}

	return corner;
}

Vec3f Grid::_getPos( Vec3i corner )
{
	if( ! _isValidCorner( corner ))
		return Vec3f( 0.0f, 0.0f, 0.0f );

	return mPos000 + Vec3f( corner ) * mSize;
}

Vec3f Grid::_getInterpolation( Vec3i corner, Vec3f pos )
{
	Vec3f pos000 = _getPos( _getCornerOffset( corner, O_000 ));
	Vec3f pos001 = _getPos( _getCornerOffset( corner, O_001 ));
	Vec3f pos010 = _getPos( _getCornerOffset( corner, O_010 ));
	Vec3f pos011 = _getPos( _getCornerOffset( corner, O_011 ));
	Vec3f pos100 = _getPos( _getCornerOffset( corner, O_100 ));
	Vec3f pos101 = _getPos( _getCornerOffset( corner, O_101 ));
	Vec3f pos110 = _getPos( _getCornerOffset( corner, O_110 ));
	Vec3f pos111 = _getPos( _getCornerOffset( corner, O_111 ));

	float x0 = pos000.x;
	float x1 = pos100.x;
	float y0 = pos000.y;
	float y1 = pos010.y;
	float z0 = pos000.z;
	float z1 = pos001.z;

	float xd = ( pos.x - x0 ) / ( x1 - x0 );
	float yd = ( pos.y - y0 ) / ( y1 - y0 );
	float zd = ( pos.z - z0 ) / ( z1 - z0 );

	Vec3f c00 = getDirection( _getCornerOffset( corner, O_000 )) * ( 1 - xd ) + getDirection( _getCornerOffset( corner, O_100 )) * xd;
	Vec3f c10 = getDirection( _getCornerOffset( corner, O_010 )) * ( 1 - xd ) + getDirection( _getCornerOffset( corner, O_110 )) * xd;
	Vec3f c01 = getDirection( _getCornerOffset( corner, O_001 )) * ( 1 - xd ) + getDirection( _getCornerOffset( corner, O_101 )) * xd;
	Vec3f c11 = getDirection( _getCornerOffset( corner, O_011 )) * ( 1 - xd ) + getDirection( _getCornerOffset( corner, O_111 )) * xd;

	Vec3f c0  = c00 * ( 1 - yd ) + c10 * yd;
	Vec3f c1  = c01 * ( 1 - yd ) + c11 * yd;

	Vec3f c   = c0  * ( 1 - zd ) + c1  * zd;

	//app::console() << " pos: " << pos << " vec: " << c << endl;

	return c;
}

} // namespace Nibbal
