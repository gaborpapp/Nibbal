#pragma once

#include "cinder/Cinder.h"
#include "cinder/Vector.h"
#include "CinderBullet.h"

namespace Nibbal {

class Cube
{
public:
	enum CornerType
	{
		C_000, // left  lower back
		C_001, // left  lower front
		C_100, // right lower back
		C_101, // right lower front
		C_010, // left  upper back
		C_011, // left  upper front
		C_110, // right upper back
		C_111, // right upper front
	};

	Cube( ci::Vec3f pos000, float size );
	
	void      addDirection( CornerType cornerType, ci::Vec3f direction );
	ci::Vec3f getCorner( CornerType cornerType );
	bool      isContain( ci::Vec3f pos );
	ci::Vec3f getInterpolation( ci::Vec3f pos );

private:
	ci::Vec3f _getDirection( CornerType cornerType );

private:
	ci::Vec3f mCenter;
	float     mSize;

	std::map< CornerType, std::shared_ptr< ci::Vec3f > > mDirections;
};

class IdealThrow
{
	public:
		void      setup();
		ci::Vec3f calcDirection( ci::Vec3f pos );

	private:
		Cube *_getCube( ci::Vec3f pos );

	private:
		std::vector< std::shared_ptr< Cube > > mCubes;
};

class Physics
{
	public:
		void setup();
		void draw();

		void addMesh( ci::TriMesh mesh );
		void addBox( ci::Vec3f size, ci::Vec3f translate );
		void update( float fps );
		ci::Vec3f getPerfectDirection( ci::Vec3f src, ci::Vec3f dst );

		void throwBall( ci::Vec3f pos, ci::Vec3f vel );
		ci::Vec3f     getRingPos();
		float         getRingRadius();
		ci::Vec3f     getBallPos();
		ci::Matrix44f getBallMatrix();

	private:
		bullet::DynamicsWorldRef   mWorld;
		bullet::CollisionObjectRef mBall;
		float                      mTime;
		IdealThrow                 mIdealThrow;
};

} // namespace Nibbal
