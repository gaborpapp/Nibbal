#pragma once

#include <string>

#include "cinder/Cinder.h"
//#include "cinder/gl/Texture.h"
#include "cinder/Quaternion.h"

#include "PParams.h"

#include "CiNI.h"
#include "AssimpLoader.h"

namespace Nibbal {

class KinectPlayer
{
	public:
		void setup( const ci::fs::path &path = "" );

		void update();
		void draw();

		void showParams( bool show );

	private:

		mndl::ni::OpenNI mNI;
		mndl::ni::UserTracker mNIUserTracker;

		//ci::gl::Texture mDepthTexture;

		mndl::assimp::AssimpLoader mPlayerAiMesh;
		void setupNode( const std::string &name, const ci::Quatf &qrot );
		void transformNode( const std::string &nodeName, unsigned userId, XnSkeletonJoint skelJoint );

		ci::params::PInterfaceGl mParams;
		float mSmoothing;
		float mMinOriConf;
};

} // namespace Nibbal
