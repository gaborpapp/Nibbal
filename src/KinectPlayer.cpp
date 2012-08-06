#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderMath.h"

#include "KinectPlayer.h"

using namespace ci;
using namespace std;
using namespace mndl;

#define USE_KINECT           0
#define USE_KINECT_RECORDING 0
#define KINECT_TIME_LIMIT    0 // set to 0 to disable KINECT_TIME_LIMIT

namespace Nibbal {

void BallPoint::Init()
{
	mState = S_NOT_VALID;
	mPos   = Vec3f();
}

void KinectPlayer::setup( Physics *physic )
{
	mListenerMap = std::shared_ptr<ListenerMap>( new ListenerMap() );

	mPhysics = physic;

#if USE_KINECT
	// setup kinect
#	if USE_KINECT_RECORDING
		// use openni recording
		fs::path recordingPath = app::getAppPath();
#		if defined( CINDER_MAC )
			recordingPath /= "..";
#		endif
		recordingPath /= "nibbal-1208010957.oni";
		mNI = ni::OpenNI( recordingPath );
#	else
		mNI = ni::OpenNI( ni::OpenNI::Device() );
#	endif

	mNIUserTracker = mNI.getUserTracker();

	mNI.setMirrored();
	mNI.start();

	mDisableKinect = false;
	mTimerLimit = app::getElapsedSeconds();
#endif

	mHasBall = false;
	mIsThrowing = false;

	mParams = params::PInterfaceGl( "Kinect Player", Vec2i( 300, 300 ) );
	mParams.addPersistentSizeAndPosition();

	mParams.addText( "Kinect" );
	mParams.addPersistentParam( "Draw depth", &mDrawDepth, false );
	mParams.addPersistentParam( "Draw ideal grid", &mDrawGrid, false );
	mParams.addPersistentParam( "Smoothing", &mSmoothing, .70, "min=0 max=1 step=.05" );
	mParams.addPersistentParam( "Min ori confidence", &mMinOriConf, .7, "min=0 max=1 step=.05" );
	mParams.addSeparator();

	mParams.addText( "Physics" );
	mParams.addPersistentParam( "Ball lifetime", &mBallLifetime, 3., "min=2 max=20 step=.1" );
	mParams.addPersistentParam( "Ball velocity scale", &mBallVelocityScale, 120, "min=1 max=1000" );
	mParams.addPersistentParam( "Ball position smoothing", &mBallPositionSmoothing, .3, "min=0 max=1 step=.01" );
	mParams.addPersistentParam( "Velocity deflection limit", &mBallVelocityDeflectionLimit, .3, "min=0 max=1.57 step=.01" );
	mParams.addPersistentParam( "Enable continuous collision detection", &mEnableCcd, true );
	mParams.addPersistentParam( "Ccd motion threshold", &mCcdMotionThres, 0.125f, "min=0 max=1 step=0.001" );
	mParams.addPersistentParam( "Ccd swept sphere radius", &mCcdSweptSphereRadius, 0.05f, "min=0 max=.124 step=0.001" );
	mParams.addSeparator();
	mParams.addText( "Kinect throw detection" );
	mParams.addPersistentParam( "Arm angle min", &mArmAngleMin, 1.4, "min=0 max=3.14 step=.01" );
	mParams.addPersistentParam( "Arm angle max", &mArmAngleMax, 2.8, "min=0 max=3.14 step=.01" );

	mParams.addPersistentParam( "Hands distance min", &mHandsDistanceMin, 450, "min=0 max=5000 step=10" );
	mParams.addPersistentParam( "Hands distance max", &mHandsDistanceMax, 900, "min=0 max=5000 step=10.05" );
	mParams.addPersistentParam( "Hands normalized distance limit", &mHandsDistanceLimitNorm, .7, "min=0 max=1 step=.05" );

	mParams.addPersistentParam( "Ball speed min", &mBallSpeedMin, .005, "min=0.005 max=0.03 step=.005" );
	mParams.addPersistentParam( "Ball speed max", &mBallSpeedMax, .045, "min=0.005 max=0.1 step=.005" );
	mParams.addPersistentParam( "Throw threshold", &mThrowThreshold, .64, "min=0 max=1 step=.01" );

	mParams.addSeparator();

	mParams.addText( "Throw detection debug" );
	mArmAngleNorm = 0;
	mParams.addParam( "Arm angle normalized", &mArmAngleNorm, "", true );
	mHandsBelowShoulder = 0;
	mParams.addParam( "Hands below shoulder", &mHandsBelowShoulder, "", true );
	mBallSpeedNorm = 0;
	mParams.addParam( "Ball speed", &mBallSpeed, "", true );
	mParams.addParam( "Ball speed normalized", &mBallSpeedNorm, "", true );
	mThrowCoeff = 0;
	mParams.addParam( "Throw coeff", &mThrowCoeff, "", true );

	mParams.addSeparator();
	mBallInitialPos = mPosition = Vec3f( 0.0f, 2.0f, 0.5f );
	mDirection = Vec3f( 0.15f, 5.99f, 1.77f );

	mParams.addParam( "Position" , &mPosition  );
	mParams.addParam( "Direction", &mDirection );

	mParams.setOptions( "", "refresh=.1" );

	// load player
	mPlayerAiMesh = assimp::AssimpLoader( app::getAssetPath( "models/player/player.dae" ) );
	mPlayerAiMesh.enableSkinning();

	// setup bind pose
	// new model
	assimp::AssimpNodeRef sceneRef = mPlayerAiMesh.getAssimpNode( "Scene001" );
	assert( sceneRef );
	// NOTE: player_0004.dae has a wrong position needs to be repositioned
	sceneRef->setPosition( Vec3f( 0, 1.14, 1.1 ) );

	Quatf q0( Vec3f( 0, 1, 0 ), 0.0 );
	setupNode( "root", Quatf( 0, -M_PI / 2, 0 ) * Quatf( 0, M_PI / 2, 0 ) );
	setupNode( "neck", q0 );
	setupNode( "l_hip", q0 );
	setupNode( "l_knee", q0 );
	setupNode( "r_hip", q0 );
	setupNode( "r_knee", q0 );

	Quatf q1( Vec3f( 0, 0, 1 ), -M_PI / 4 );
	setupNode( "r_humerus", q1 );
	setupNode( "r_ulna", q1 );

	q1 = Quatf( Vec3f( 0, 0, 1 ), M_PI / 4 );
	setupNode( "l_ulna", q1 );
	setupNode( "l_humerus", q1 );

	// old model
	/*
	Quatf q( Vec3f( 0, 1, 0 ), M_PI / 4 );
	Quatf q2( Vec3f( 1, 0, 0 ), -M_PI / 2 );
	setupNode( "root", q2 );
	setupNode( "r_humerus", q * q2 );
	setupNode( "r_ulna", q * q2 );

	q = Quatf( Vec3f( 0, 1, 0 ), -M_PI / 4 );
	setupNode( "l_ulna", q * q2 );
	setupNode( "l_humerus", q * q2 );

	setupNode( "neck", q2 );
	setupNode( "l_hip", q2 );
	setupNode( "l_knee", q2 );
	setupNode( "r_hip", q2 );
	setupNode( "r_knee", q2 );
	*/

	mLeftWristNode = mPlayerAiMesh.getAssimpNode( "l_wrist" );
	mRightWristNode = mPlayerAiMesh.getAssimpNode( "r_wrist" );
	mLeftElbowNode = mPlayerAiMesh.getAssimpNode( "l_ulna" );
	mRightElbowNode = mPlayerAiMesh.getAssimpNode( "r_ulna" );

	// load basketball
	mBallAiMesh = assimp::AssimpLoader( app::getAssetPath( "models/ball/basketball.obj" ) );
	mBallNodeRef = mBallAiMesh.getAssimpNode( "labda_ball" );

	mBallPoint.Init();

	mTimelineRef = Timeline::create();
	app::timeline().add( mTimelineRef );
}

void KinectPlayer::setupNode( const string &name, const Quatf &qrot )
{
	assimp::AssimpNodeRef nodeRef = mPlayerAiMesh.getAssimpNode( name );
	if ( !nodeRef )
	{
		app::console() << "WARNING: could not find bone " << name << endl;
		return;
	}

	nodeRef->setInheritOrientation( false );
	nodeRef->setOrientation( qrot );
	nodeRef->setInitialState();
}

void KinectPlayer::transformNode( const string &nodeName, unsigned userId, XnSkeletonJoint skelJoint )
{
#if USE_KINECT
		float oriConf;
		Matrix33f ori = mNIUserTracker.getJointOrientation( userId, skelJoint, &oriConf );

		if ( oriConf >= mMinOriConf )
		{
			// change rotation coordinate directions
			ori.m10 *= -1;
			ori.m01 *= -1;
			ori.m21 *= -1;
			ori.m12 *= -1;
			Quatf q( ori );
			assimp::AssimpNodeRef nodeRef = mPlayerAiMesh.getAssimpNode( nodeName );
			// apply skeleton pose relatively to the bone bind pose
			nodeRef->setOrientation( nodeRef->getInitialOrientation() * q );
		}
#endif
}

void KinectPlayer::update()
{
	mPhysics->enableGridDisplay( mDrawGrid );
	mPhysics->enableCcd( mEnableCcd, mCcdMotionThres, mCcdSweptSphereRadius );

#if USE_KINECT
	if( KINECT_TIME_LIMIT )
	{
		if ( ( app::getElapsedSeconds() - mTimerLimit ) > KINECT_TIME_LIMIT )
			mDisableKinect = true;
	}

	if ( mDisableKinect )
		return;

	bool NIupdated = false;
	if ( mNI.checkNewDepthFrame() )
	{
		mDepthTexture = mNI.getDepthImage();
		NIupdated = true;
	}

	if( mNIUserTracker.getNumUsers() == 0 )
	{
		mListenerMap->callCallback( ET_NO_USER );
		mActUser = 0;
		return;
	}

	unsigned user = mNIUserTracker.getClosestUserId();

	if( mActUser != user )
	{
		mListenerMap->callCallback( ET_NEW_USER );
		mActUser = user;
	}

	mNIUserTracker.setSmoothing( mSmoothing );

	vector< unsigned > users = mNIUserTracker.getUsers();
	if ( !users.empty() )
	{
		unsigned userId = users[ 0 ];

		transformNode( "root", userId, XN_SKEL_TORSO );
		transformNode( "l_humerus", userId, XN_SKEL_LEFT_SHOULDER );
		transformNode( "r_humerus", userId, XN_SKEL_RIGHT_SHOULDER );
		transformNode( "l_ulna", userId, XN_SKEL_LEFT_ELBOW );
		transformNode( "r_ulna", userId, XN_SKEL_RIGHT_ELBOW );
	/*
		transformNode( "l_hip", userId, XN_SKEL_LEFT_HIP );
		transformNode( "r_hip", userId, XN_SKEL_RIGHT_HIP );
		transformNode( "l_knee", userId, XN_SKEL_LEFT_KNEE );
		transformNode( "r_knee", userId, XN_SKEL_RIGHT_KNEE );
	*/
		transformNode( "neck", userId, XN_SKEL_NECK );
	}

	// only update throwing detection at the rate of the kinect
	if ( NIupdated )
		detectThrowing();
#endif

	mPlayerAiMesh.update();
	mPhysics->setDirectionDeflection( mBallVelocityDeflectionLimit );
	_checkBallPoint();
}

void KinectPlayer::detectThrowing()
{
	if ( mActUser > 0 )
	{
		Vec3f rShoulder = mNIUserTracker.getJoint3d( mActUser, XN_SKEL_RIGHT_SHOULDER );
		Vec3f rElbow = mNIUserTracker.getJoint3d( mActUser, XN_SKEL_RIGHT_ELBOW );
		Vec3f rHand = mNIUserTracker.getJoint3d( mActUser, XN_SKEL_RIGHT_HAND );
		Vec3f lHand = mNIUserTracker.getJoint3d( mActUser, XN_SKEL_LEFT_HAND );

		// ball position in scene
		Vec3f meshLWrist = mLeftWristNode->getDerivedPosition();
		Vec3f meshRWrist = mRightWristNode->getDerivedPosition();
		Vec3f meshLElbow = mLeftElbowNode->getDerivedPosition();
		Vec3f meshRElbow = mRightElbowNode->getDerivedPosition();
		Vec3f meshLLowerArm = meshLWrist - meshLElbow;
		Vec3f meshRLowerArm = meshRWrist - meshRElbow;
		const float palmCoeff = .25f;
		Vec3f meshLHand = meshLWrist + meshLLowerArm * palmCoeff;
		Vec3f meshRHand = meshRWrist + meshRLowerArm * palmCoeff;
		Vec3f meshBallPos = ( meshLHand + meshRHand ) / 2.f;
		mBallInitialPos = lerp< Vec3f >( mBallInitialPos, meshBallPos, mBallPositionSmoothing );

		// hand below shoulder
		float handsBelowShoulder = ( rHand.y < rShoulder.y ) && ( lHand.y < rShoulder.y );

		// distance of hands
		float handsDistanceNorm = lmap< float >( lHand.distance( rHand ),
				mHandsDistanceMin, mHandsDistanceMax, 0, 1 );
		handsDistanceNorm = math< float >::clamp( handsDistanceNorm, 0, 1 );

		// arm straigthness
		Vec3f rUpper = rShoulder - rElbow;
		rUpper.normalize();
		Vec3f rLower = rHand - rElbow;
		rLower.normalize();
		float armAngle = math< float >::acos( rUpper.dot( rLower ) );
		if ( armAngle > M_PI )
			armAngle = 2 * M_PI - armAngle;

		float armAngleNorm = lmap< float >( armAngle, mArmAngleMin, mArmAngleMax, 0, 1 );
		armAngleNorm = math< float >::clamp( armAngleNorm, 0, 1 );

		// ball speed
		Vec3f ballVelocity( 0, 0, 0 );
		float ballSpeedNorm = 0;
		if ( mHasBall )
		{
			ballVelocity = mBallInitialPos - mBallInitialPrevPos;
			ballSpeedNorm = lmap< float >( ballVelocity.length(), mBallSpeedMin, mBallSpeedMax, 0, 1 );
			ballSpeedNorm = math< float >::clamp( ballSpeedNorm, 0, 1 );
		}
		mBallInitialPrevPos = mBallInitialPos;

		// if the hands are too far away, the ball is dropped
		if ( handsDistanceNorm > mHandsDistanceLimitNorm )
			mHasBall = false;
		else // hands are close, not throwing, hands below shoulder, grab the ball
		if ( handsBelowShoulder && !mIsThrowing )
			mHasBall = true;

		// all parameters taken into account
		float throwCoeff = mHasBall * ( 1 - handsBelowShoulder ) * mArmAngleNorm * ballSpeedNorm;
		if ( throwCoeff >= mThrowThreshold )
		{
			throwBall();
		}

		mArmAngleNorm = armAngleNorm;
		mHandsBelowShoulder = handsBelowShoulder;
		mBallVelocity = ballVelocity;
		mBallSpeed = ballVelocity.length();
		mBallSpeedNorm = ballSpeedNorm;
		mThrowCoeff = throwCoeff;
	}
	else
	{
		mHasBall = false;
	}
}


void KinectPlayer::draw( bool drawPlayer )
{
	if ( drawPlayer )
	{
		// draw ball
		if ( mHasBall || mIsThrowing )
		{
			gl::pushModelView();
			if ( mHasBall )
			{
				mBallNodeRef->mMeshes[ 0 ]->mMaterial.setDiffuse( Color::white() );
				gl::translate( mBallInitialPos );
			}
			else
				if ( mIsThrowing )
				{
					mBallNodeRef->mMeshes[ 0 ]->mMaterial.setDiffuse( mBallColor.value() );
					Matrix44f matrix = mPhysics->getBallMatrix();
					gl::multModelView( matrix );
				}

			gl::enableAlphaBlending();
			mBallAiMesh.draw();
			gl::disableAlphaBlending();

			gl::popModelView();

			if ( mDrawGrid )
			{
				gl::color( Color( 1, 0, 0 ) );
				gl::drawVector( mBallInitialPos, mBallInitialPos + mBallVelocity * mBallVelocityScale * .1 );
			}
		}

		gl::pushModelView();
		mPlayerAiMesh.draw();
		gl::popModelView();
	}

	if ( mDepthTexture && mDrawDepth )
	{
		gl::pushMatrices();
		gl::setMatricesWindow( app::getWindowSize() );
		gl::setViewport( app::getWindowBounds() );

		gl::enableAlphaBlending();
		gl::color( ColorA::gray( 1, .8 ) );
		Rectf trect = Rectf( app::getWindowBounds() ) / 4.f;
		trect.offset( Vec2f( app::getWindowSize() ) * Vec2f( .75f, 0 ) );
		gl::draw( mDepthTexture, trect );
		gl::disableAlphaBlending();

		gl::popMatrices();
	}

#if USE_KINECT
	if ( mDisableKinect )
	{
		gl::pushMatrices();
		gl::setMatricesWindow( app::getWindowSize() );
		gl::setViewport( app::getWindowBounds() );
		gl::drawStringCentered( "Time limit has been exceeded",
				Vec2f( app::getWindowSize() ) * Vec2f( .5, .05 ),
				Color( 1, 0, 0 ) );
		gl::popMatrices();
	}
#endif
}

void KinectPlayer::expireBallThrowing()
{
	mIsThrowing = false;
}

#ifdef CINDER_MSW
// the first pragma turns off whatever optimizations were on (Microsoft compiler has a crash without it)
#pragma optimize( "", off )
#endif

void KinectPlayer::throwBall()
{
	mBallPoint.Init();

#if USE_KINECT
	mPhysics->throwBall( mBallInitialPos, mBallVelocity * mBallVelocityScale );
#else
	mPhysics->throwBall( mPosition, mDirection );
#endif

	mBallColor = ColorA::white();
	mTimelineRef->clear();
	mTimelineRef->apply( &mBallColor, ColorA::white(), mBallLifetime - 2. );
	mTimelineRef->appendTo( &mBallColor, ColorA( 1, 1, 1, 0 ), 2. );
	mTimelineRef->add( std::bind( &KinectPlayer::expireBallThrowing, this ),
			app::timeline().getCurrentTime() + mBallLifetime );
	mHasBall = false;
	mIsThrowing = true;
}

#ifdef CINDER_MSW
#pragma optimize( "", on )
// The second pragma restores optimizations to whatever state they had before (Microsoft compiler has a crash without it)
#endif

void KinectPlayer::_checkBallPoint()
{
	if( mBallPoint.mState == BallPoint::S_GOAL
	 || mBallPoint.mState == BallPoint::S_MISS )
		return;

	Vec3f ballPosAct  = mPhysics->getBallPos();
	Vec3f ballPosPrev = mBallPoint.mPos;
	Vec3f ringPos     = mPhysics->getRingPos();
	float ringRadius  = mPhysics->getRingRadius() * 1.2f;

	mBallPoint.mPos = ballPosAct;

	if( ballPosAct.x >= ringPos.x - ringRadius	// inside the ring cylinder
	 && ballPosAct.x <= ringPos.x + ringRadius
	 && ballPosAct.z >= ringPos.z - ringRadius
	 && ballPosAct.z <= ringPos.z + ringRadius )
	{
		if( ballPosAct.y > ringPos.y )			// above the ring
		{
			mBallPoint.mState = BallPoint::S_ABOVE;
			return;
		}
		else
		{
			if( ballPosPrev.y > ballPosAct.y )	// falling down
			{
				if( mBallPoint.mState == BallPoint::S_ABOVE )
				{
					mBallPoint.mState = BallPoint::S_GOAL;
					mListenerMap->callCallback( ET_GOAL );
				}
				else
				{
					mBallPoint.mState = BallPoint::S_MISS;
					mListenerMap->callCallback( ET_MISS );
				}
			}
			else
			{
				mBallPoint.mState = BallPoint::S_NOT_VALID;
			}
		}
	}
	else
	{
		if( ballPosAct.y < ringPos.y			// below the ring
		 && ballPosPrev.y > ballPosAct.y )		// falling down
		{
			mListenerMap->callCallback( ET_MISS );
			mBallPoint.mState = BallPoint::S_MISS;
		}
	}
}

} // namespace Nibbal
