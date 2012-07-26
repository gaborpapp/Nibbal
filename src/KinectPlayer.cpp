#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

#include "KinectPlayer.h"

using namespace ci;
using namespace std;
using namespace mndl;

namespace Nibbal {

void KinectPlayer::setup( const fs::path &path )
{
	/*
	if ( path.empty() )
		mNI = ni::OpenNI( ni::OpenNI::Device() );
	else
		mNI = ni::OpenNI( path );

	mNIUserTracker = mNI.getUserTracker();

	mNI.setMirrored();
	mNI.start();
	*/

	// load player
	mPlayerAiMesh = assimp::AssimpLoader( app::getAssetPath( "models/player/player.dae" ) );
	mPlayerAiMesh.enableSkinning();

	// setup bind pose
	Quatf q( Vec3f( 0, 1, 0 ), M_PI / 4 );
	Quatf q2( Vec3f( 1, 0, 0 ), -M_PI / 2 );
	setupBone( "root", q2 );
	setupBone( "r_humerus", q * q2 );
	setupBone( "r_ulna", q * q2 );

	q = Quatf( Vec3f( 0, 1, 0 ), -M_PI / 4 );
	setupBone( "l_ulna", q * q2 );
	setupBone( "l_humerus", q * q2 );

	setupBone( "neck", q2 );
	setupBone( "l_hip", q2 );
	setupBone( "l_knee", q2 );
	setupBone( "r_hip", q2 );
	setupBone( "r_knee", q2 );
}

void KinectPlayer::setupBone( const string &name, const Quatf &qrot )
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

void KinectPlayer::update()
{
	mPlayerAiMesh.update();
}

void KinectPlayer::draw()
{
	mPlayerAiMesh.draw();
}

} // namespace Nibbal
