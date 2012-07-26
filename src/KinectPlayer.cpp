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
	mPlayerAiMesh.disableTextures();
}

void KinectPlayer::update()
{
	mPlayerAiMesh.update();
}

void KinectPlayer::draw()
{
	gl::enableWireframe();
	mPlayerAiMesh.draw();
	gl::disableWireframe();
}

} // namespace Nibbal
