#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderMath.h"

#include "KinectPlayer.h"

using namespace ci;
using namespace std;
using namespace mndl;

#define USE_KINECT 0

namespace Nibbal {

void KinectPlayer::setup( const fs::path &path )
{
#if USE_KINECT
	// setup kinect
	if ( path.empty() )
		mNI = ni::OpenNI( ni::OpenNI::Device() );
	else
		mNI = ni::OpenNI( path );

	mNIUserTracker = mNI.getUserTracker();

	mNI.setMirrored();
	mNI.start();
#endif

	mParams = params::PInterfaceGl( "Kinect Player", Vec2i( 300, 300 ) );
	mParams.addPersistentSizeAndPosition();

	mParams.addPersistentParam( "Draw depth", &mDrawDepth, false );
	mParams.addPersistentParam( "Smoothing", &mSmoothing, .7, "min=0 max=1 step=.05" );
	mParams.addPersistentParam( "Min ori confidence", &mMinOriConf, .7, "min=0 max=1 step=.05" );
	mParams.addSeparator();

	mParams.addPersistentParam( "Arm angle min", &mArmAngleMin, 0, "min=0 max=3.14 step=.01" );
	mParams.addPersistentParam( "Arm angle max", &mArmAngleMax, M_PI, "min=0 max=3.14 step=.01" );
	mArmAngleNorm = 0;
	mParams.addParam( "Arm angle normalized", &mArmAngleNorm, "", true );

	mParams.addSeparator();
	mPosition  = Vec3f( 0.0f, 2.0f, 1.0f );
	mDirection = Vec3f( 0.0f, 3.0f, 3.0f );

	mParams.addParam( "Position" , &mPosition  );
	mParams.addParam( "Direction", &mDirection );


	// load player
	mPlayerAiMesh = assimp::AssimpLoader( app::getAssetPath( "models/player/player.dae" ) );
	mPlayerAiMesh.enableSkinning();

	// setup bind pose
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

	// load basketball
	mBallAiMesh = assimp::AssimpLoader( app::getAssetPath( "models/ball/basketball.obj" ) );
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
#if USE_KINECT
	if ( mNI.checkNewDepthFrame() )
		mDepthTexture = mNI.getDepthImage();

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

	detectThrowing();
#endif

	mPlayerAiMesh.update();
}

void KinectPlayer::detectThrowing()
{
	unsigned user = mNIUserTracker.getClosestUserId();
	if ( user > 0 )
	{
        Vec3f rShoulder = mNIUserTracker.getJoint3d( user, XN_SKEL_RIGHT_SHOULDER );
        Vec3f rElbow = mNIUserTracker.getJoint3d( user, XN_SKEL_RIGHT_ELBOW );
        Vec3f rHand = mNIUserTracker.getJoint3d( user, XN_SKEL_RIGHT_HAND );
        Vec3f lHand = mNIUserTracker.getJoint3d( user, XN_SKEL_LEFT_HAND );

#if 0
		// length of right arm
        float rArmLength = rShoulder.distance( rElbow ) + rElbow.distance( rHand );

        mBallPosition = ( lHand + rHand ) / 2.f;

		// current hand shoulder distance
        float currentArmLength = rShoulder.distance( rHand );

		// height of right hand between the two limits related to the right shoulder normalized
        float handHeightPercent = math< float >::lmap( rHand.y - rShoulder.y,
				mHandHeightMin, mHandHeightMax, 0, 1 );
		handHeightPercent = math< float >::clamp( handHeightPercent, 0, 1 );

        // distance of hands
		float handsDistancePercent = math< float>::lmap( lHand.distance( rHand ),
				mHandsDistanceMin, mHandsDistanceMax, 0, 1 );
		float handsDistancePercent = math< float >::clamp( handsDistancePercent, 0, 1 );

#endif

        // arm straigthness
		Vec3f rUpper = rShoulder - rElbow;
		rUpper.normalize();
		Vec3f rLower = rHand - rElbow;
		rLower.normalize();
		float armAngle = math< float >::acos( rUpper.dot( rLower ) );
		if ( armAngle > M_PI )
			armAngle = 2 * M_PI - armAngle;

		mArmAngleNorm = lmap< float >( armAngle, mArmAngleMin, mArmAngleMax, 0, 1 );
		mArmAngleNorm = math< float >::clamp( mArmAngleNorm, 0, 1 );

/*
        // ball speed
        ballSpeedPercent = ofMap(ballPosition.distance(pBallPosition), ballSpeedMin, ballSpeedMax, 0, 1, true);

        // all parameters taken into account
        shootPercent = hasBall * handHeightPercent * handsDistancePercent * armLengthPercent * ballSpeedPercent;

*/
	}
}


void KinectPlayer::draw( Physics *physics )
{
	gl::pushModelView();
	mPlayerAiMesh.draw();
	gl::popModelView();

	gl::pushModelView();
	Matrix44f matrix = physics->getBallMatrix();
	gl::multModelView( matrix );
	mBallAiMesh.draw();
	gl::popModelView();

	if ( mDepthTexture && mDrawDepth )
	{
		gl::pushMatrices();
		gl::setMatricesWindow( app::getWindowSize() );
		gl::setViewport( app::getWindowBounds() );

		gl::color( Color::white() );
		gl::draw( mDepthTexture, Rectf( app::getWindowBounds() ) / 3.f );
		gl::popMatrices();
	}

}

void KinectPlayer::throwBall( Physics *physics )
{
//	mndl::assimp::AssimpNodeRef assimpNode = mBallAiMesh.getAssimpNode( "labda_ball" );

	physics->dropBall( mPosition, mDirection );
}

} // namespace Nibbal
