#pragma once

#include "cinder/Cinder.h"
#include "cinder/Vector.h"
#include "CinderBullet.h"

namespace Nibbal {

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
};

} // namespace Nibbal
