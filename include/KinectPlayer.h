#pragma once

#include <string>

#include "cinder/Cinder.h"
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

	private:

		mndl::ni::OpenNI mNI;
		mndl::ni::UserTracker mNIUserTracker;

		mndl::assimp::AssimpLoader mPlayerAiMesh;
		void setupBone( const std::string &name, const ci::Quatf &qrot );
};

} // namespace Nibbal
