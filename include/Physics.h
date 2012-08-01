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
	Grid();
	~Grid();

	void      setDirection( ci::Vec3i corner, ci::Vec3f direction );
	ci::Vec3f getDirection( ci::Vec3i corner );
	ci::Vec3f calcDirection( ci::Vec3f pos );
	ci::Vec3f calcDirection( ci::Vec3f pos, ci::Vec3f velIdeal, ci::Vec3f vel );

	void draw();

private:
	void _setup( ci::Vec3f pos000, ci::Vec3i numCorner, float size, ci::Vec3f posSensitive, float radiusSensitive, float radiusBorder );
	void _allocArray();
	void _freeArray();
	bool      _isValidCorner( ci::Vec3i corner );
	ci::Vec3i _getCorner( ci::Vec3f pos );
	ci::Vec3i _getCorner( ci::Vec3i corner, Offset offset );
	ci::Vec3f _getPos( ci::Vec3i corner );
	ci::Vec3f _getTrilinearInterpolation( ci::Vec3i corner, ci::Vec3f pos );
	float     _getLinearInterpolation( float x0, float y0, float x1, float y1, float x );

private:
	// grid for perfect throwing
	ci::Vec3f mPos000;
	ci::Vec3i mNumCorner;
	float     mSize;
	ci::Vec3f ***mDirections;  // 3D array

	// sensitive are for perfect throwing
	ci::Vec3f mPosSensitive;
	float     mRadiusSensitive;
	float     mRadiusBorder;
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

		void enableGridDisplay( bool enable = true ) { mDrawGrid = enable; }
		void setDirectionDeflection( float deflection ) { mDeflectionLimit = deflection; }

	private:
		bullet::DynamicsWorldRef   mWorld;
		bullet::CollisionObjectRef mBall;
		float                      mTime;
		std::shared_ptr< Grid >    mGridThrow;

		bool mDrawGrid;
		float mDeflectionLimit;
};

} // namespace Nibbal
