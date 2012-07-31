#pragma once

#include <string>

#include "cinder/Cinder.h"
//#include "cinder/gl/Texture.h"
#include "cinder/Quaternion.h"

#include "PParams.h"

#include "CiNI.h"
#include "AssimpLoader.h"
#include "Physics.h"

namespace Nibbal {

class KinectPlayer
{
	public:
		void setup( const ci::fs::path &path = "" );

		void update();
		void draw( Physics *physics );

		void throwBall( Physics *physics );

	private:

		mndl::ni::OpenNI mNI;
		mndl::ni::UserTracker mNIUserTracker;

		ci::gl::Texture mDepthTexture;

		mndl::assimp::AssimpLoader mPlayerAiMesh;
		void setupNode( const std::string &name, const ci::Quatf &qrot );
		void transformNode( const std::string &nodeName, unsigned userId, XnSkeletonJoint skelJoint );
		void detectThrowing();

		mndl::assimp::AssimpLoader mBallAiMesh;

		ci::params::PInterfaceGl mParams;
		float mSmoothing;
		float mMinOriConf;
		bool mDrawDepth;

		float mArmAngleMin;
		float mArmAngleMax;
		float mArmAngleNorm;

		ci::Vec3f mDirection;
		ci::Vec3f mPosition;
};

} // namespace Nibbal
