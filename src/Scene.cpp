#include <assert.h>

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

#include "Scene.h"

using namespace ci;
using namespace std;
using namespace mndl;

namespace Nibbal {

void Scene::setup()
{
	/*
	mParams = params::PInterfaceGl( "Scene", Vec2i( 300, 300 ) );
	mParams.addPersistentSizeAndPosition();
	*/

	mDisplay.setup();
	mAdDisplay.setup( app::getAssetPath( "ads" ), Vec2i( 1920, 210 ) );

	// load scene
	mSceneAiMesh = assimp::AssimpLoader( app::getAssetPath( "models/scene/scene.obj" ) );
	mSceneAiMesh.disableSkinning();
	mSceneAiMesh.disableMaterials();
	if ( mSceneAiMesh.getNumMeshes() >= 2 )
	{
		// set repeat wrapping for floor, this property is not stored in .obj
		gl::Texture &floorTexture = mSceneAiMesh.getTexture( 2 );
		assert( floorTexture );
		floorTexture.bind();
		floorTexture.setWrap( GL_REPEAT, GL_REPEAT );
		floorTexture.unbind();

		// set scoreboard
		gl::Texture &scoreBoardTexture = mSceneAiMesh.getTexture( 0 );
		assert( scoreBoardTexture );
		scoreBoardTexture = mDisplay.getTexture();

		// set ads
		gl::Texture &adsTexture = mSceneAiMesh.getTexture( 1 );
		assert( adsTexture );
		adsTexture = mAdDisplay.getTexture();
	}

	startGame();
}

void Scene::startGame()
{
	mDisplay.setActive( true );
	mDisplay.setTimeAct( 00, 00  );
	mDisplay.setTimeMax( 12, 00  );
	mDisplay.setPeriod( 4 );
	mDisplay.setHome( 78 );
	mDisplay.setGuest( 80 );
	mDisplay.start();
}

void Scene::update()
{
	mDisplay.update();
	mAdDisplay.update();
}

void Scene::draw()
{
	mSceneAiMesh.draw();
}

} // namespace Nibbal
