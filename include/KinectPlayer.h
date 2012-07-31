#pragma once

#include <string>

#include "cinder/Cinder.h"
//#include "cinder/gl/Texture.h"
#include "cinder/Quaternion.h"

#include "PParams.h"

#include "CiNI.h"
#include "AssimpLoader.h"
#include "Physics.h"
#include "Listener.h"

namespace Nibbal {

struct BallPoint
{
	enum State
	{
		S_NOT_VALID,
		S_ABOVE,
		S_BELOW,
	};

	void Init();

	State     mState;
	ci::Vec3f mPos;
	bool      mGoal;
};

class KinectPlayer
{
	public:
		void setup( Physics *physics, const ci::fs::path &path = "" );

		void update();
		void draw();

		void throwBall();

		template<typename T>
		void addCallback( void (T::* callbackFunction)(), T * callbackObject )
		{
			mListener->addCallback<T>( callbackFunction, callbackObject );
		}

	private:
		void _checkBallPoint();

	private:

		mndl::ni::OpenNI mNI;
		mndl::ni::UserTracker mNIUserTracker;

		//ci::gl::Texture mDepthTexture;

		mndl::assimp::AssimpLoader mPlayerAiMesh;
		void setupNode( const std::string &name, const ci::Quatf &qrot );
		void transformNode( const std::string &nodeName, unsigned userId, XnSkeletonJoint skelJoint );

		mndl::assimp::AssimpLoader mBallAiMesh;

		ci::params::PInterfaceGl mParams;
		float mSmoothing;
		float mMinOriConf;
		ci::Vec3f mDirection;
		ci::Vec3f mPosition;

		Physics   *mPhysics;
		BallPoint  mBallPoint;

		std::shared_ptr<Listener>  mListener;
};

} // namespace Nibbal
