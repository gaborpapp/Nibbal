#pragma once

#include <string>

#include "cinder/Cinder.h"
#include "cinder/gl/Texture.h"
#include "cinder/Quaternion.h"
#include "cinder/Timeline.h"

#include "PParams.h"

#include "CiNI.h"
#include "AssimpLoader.h"
#include "Physics.h"
#include "ListenerMap.h"

namespace Nibbal {

struct BallPoint
{
	enum State
	{
		S_NOT_VALID,
		S_ABOVE,
		S_GOAL,
		S_MISS,
	};

	void Init();

	State     mState;
	ci::Vec3f mPos;
};

class KinectPlayer
{
	public:
		enum EventType
		{
			ET_GOAL,
			ET_MISS,
		};

	public:
		void setup( Physics *physics );

		void update();
		void draw();

		void throwBall();

		template<typename T>
		void addCallback( EventType type, void (T::* callbackFunction)(), T * callbackObject )
		{
			mListenerMap->addCallback<T>( (int)type, callbackFunction, callbackObject );
		}

	private:
		void _checkBallPoint();

	private:

		mndl::ni::OpenNI mNI;
		mndl::ni::UserTracker mNIUserTracker;

		ci::gl::Texture mDepthTexture;

		mndl::assimp::AssimpLoader mPlayerAiMesh;
		void setupNode( const std::string &name, const ci::Quatf &qrot );
		void transformNode( const std::string &nodeName, unsigned userId, XnSkeletonJoint skelJoint );
		void detectThrowing();

		mndl::assimp::AssimpLoader mBallAiMesh;
		mndl::assimp::AssimpNodeRef mBallNodeRef;

		ci::params::PInterfaceGl mParams;
		float mSmoothing;
		float mMinOriConf;
		bool mDrawDepth;
		bool mDrawGrid;

		float mArmAngleMin;
		float mArmAngleMax;

		float mHandsDistanceMin;
		float mHandsDistanceMax;
		float mHandsDistanceLimitNorm;
		float mBallSpeedMin;
		float mBallSpeedMax;
		float mThrowThreshold;

		// throw detection debug display variables
		float mArmAngleNorm;
		float mHandsBelowShoulder;
		float mBallSpeed;
		float mBallSpeedNorm;
		float mThrowCoeff;

		void expireBallThrowing();
		float mBallLifetime;
		ci::Anim< ci::ColorA > mBallColor;

		bool mHasBall; // player has the ball
		bool mIsThrowing; // ball is currently thrown

		mndl::assimp::AssimpNodeRef mLeftWristNode;
		mndl::assimp::AssimpNodeRef mRightWristNode;
		mndl::assimp::AssimpNodeRef mLeftElbowNode;
		mndl::assimp::AssimpNodeRef mRightElbowNode;

		ci::Vec3f mBallInitialPos; // ball starting position when the user is holding it
		ci::Vec3f mBallInitialPrevPos; // ball position in previous frame
		ci::TimelineRef mTimelineRef;

		ci::Vec3f mDirection;
		ci::Vec3f mPosition;

		Physics   *mPhysics;
		BallPoint  mBallPoint;

		std::shared_ptr<ListenerMap>  mListenerMap;
};

} // namespace Nibbal
