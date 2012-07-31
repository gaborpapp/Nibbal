#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

#include "KinectPlayer.h"

using namespace ci;
using namespace std;
using namespace mndl;

#define USE_KINECT 0

namespace Nibbal {

void BallPoint::Init()
{
	mState = S_NOT_VALID;
	mPos   = Vec3f();
	mGoal  = false;
}

void KinectPlayer::setup( Physics *physic, const fs::path &path )
{
	mListener= std::shared_ptr<Listener>( new Listener());

	mPhysics = physic;
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

	mParams = params::PInterfaceGl( "Kinect", Vec2i( 300, 300 ) );
	mParams.addPersistentSizeAndPosition();

	mParams.addPersistentParam( "Smoothing", &mSmoothing, .7, "min=0 max=1 step=.05" );
	mParams.addPersistentParam( "Min ori confidence", &mMinOriConf, .7, "min=0 max=1 step=.05" );

	mPosition  = Vec3f( 0.00f, 2.00f, 1.00f );
	mDirection = Vec3f( 0.17f, 5.99f, 3.73f );

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

	mBallPoint.Init();
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
	/*
	if ( mNI.checkNewDepthFrame() )
		mDepthTexture = mNI.getDepthImage();
	*/

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
#endif

	mPlayerAiMesh.update();
	_checkBallPoint();
}

void KinectPlayer::draw()
{
	gl::pushModelView();
	mPlayerAiMesh.draw();
	gl::popModelView();

	gl::pushModelView();
	Matrix44f matrix = mPhysics->getBallMatrix();
	gl::multModelView( matrix );
	mBallAiMesh.draw();
	gl::popModelView();
}

void KinectPlayer::throwBall()
{
//	mndl::assimp::AssimpNodeRef assimpNode = mBallAiMesh.getAssimpNode( "labda_ball" );

	mBallPoint.Init();
	mPhysics->throwBall( mPosition, mDirection );
}

void KinectPlayer::_checkBallPoint()
{
	if( mBallPoint.mGoal )
		return;

	Vec3f ballPosAct  = mPhysics->getBallPos();
	Vec3f ballPosPrev = mBallPoint.mPos;
	Vec3f ringPos     = mPhysics->getRingPos();
	float ringRadius  = mPhysics->getRingRadius();

	mBallPoint.mPos = ballPosAct;

	if( ballPosPrev.y <= ballPosAct.y )
	{
		mBallPoint.mState = BallPoint::S_NOT_VALID;
		return;
	}

	if( ballPosAct.x >= ringPos.x - ringRadius
	 && ballPosAct.x <= ringPos.x + ringRadius
	 && ballPosAct.z >= ringPos.z - ringRadius
	 && ballPosAct.z <= ringPos.z + ringRadius )
	{
		if( ballPosAct.y > ringPos.y )  // above
		{
			mBallPoint.mState = BallPoint::S_ABOVE;
		}
		else
		{
			if( mBallPoint.mState == BallPoint::S_ABOVE )
			{
				mBallPoint.mGoal = true;
				mListener->callCallback();
			}
	
			mBallPoint.mState = BallPoint::S_BELOW;
		}
	}
}

} // namespace Nibbal
