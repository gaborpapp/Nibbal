#pragma once

#include "cinder/Cinder.h"
#include "cinder/Vector.h"
#include "CinderBullet.h"

namespace Nibbal {

/*
corners:
layer 0
020 120 220
010 110 210
000 100 200
layer 1
021 121 221
011 111 211
001 101 201
*/
class Grid
{
	enum Offset
	{
		O_000, // left  lower back
		O_001, // left  lower front
		O_100, // right lower back
		O_101, // right lower front
		O_010, // left  upper back
		O_011, // left  upper front
		O_110, // right upper back
		O_111, // right upper front
	};

public:
	Grid::Grid();
	Grid::~Grid();

	void      setDirection( ci::Vec3i corner, ci::Vec3f direction );
	ci::Vec3f getDirection( ci::Vec3i corner );
	ci::Vec3f calcDirection( ci::Vec3f pos );

private:
	void _setup( ci::Vec3f pos000, ci::Vec3i numCorner, float size );
	void _allocArray();
	void _freeArray();
	bool      _isValidCorner( ci::Vec3i corner );
	ci::Vec3i _getCorner( ci::Vec3f pos );
	ci::Vec3i _getCornerOffset( ci::Vec3i corner, Offset offset );
	ci::Vec3f _getPos( ci::Vec3i corner );
	ci::Vec3f _getInterpolation( ci::Vec3i corner, ci::Vec3f pos );

private:
	ci::Vec3f mPos000;
	ci::Vec3i mNumCorner;
	float     mSize;

	ci::Vec3f ***mDirections;  // 3D array
};

class Physics
{
	public:
		void setup();
		void draw();

		void addMesh( ci::TriMesh mesh );
		void addBox( ci::Vec3f size, ci::Vec3f translate );
		void update( float fps );

		void throwBall( ci::Vec3f pos, ci::Vec3f vel );
		ci::Vec3f     getRingPos();
		float         getRingRadius();
		ci::Vec3f     getBallPos();
		ci::Matrix44f getBallMatrix();

	private:
		bullet::DynamicsWorldRef   mWorld;
		bullet::CollisionObjectRef mBall;
		float                      mTime;
		std::shared_ptr< Grid >    mGridThrow;
};

} // namespace Nibbal
