#include <assert.h>

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

#include "Scene.h"

using namespace ci;
using namespace std;
using namespace mndl;

namespace Nibbal {

void Scene::setup( Physics *physics )
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

	setupPhysics( physics );

	startGame();
}

void Scene::setupPhysics( Physics *physics )
{
	// Create the ground
	{
		mndl::assimp::AssimpNodeRef assimpNode = mSceneAiMesh.getAssimpNode( "court_Plane" );
		mndl::assimp::AssimpMeshHelperRef assimpMeshHelper = assimpNode->mMeshes[0];
		ci::TriMesh mesh = assimpMeshHelper->mCachedTriMesh;

		ci::AxisAlignedBox3f boundingBox = mesh.calcBoundingBox();
		Vec3f center = boundingBox.getCenter();
		Vec3f size   = boundingBox.getSize();
		size += Vec3f( 0.0f, 20.0f, 0.0f );
		center += Vec3f( 0.0f, -10.0f, 0.0f );
		physics->addBox( size, center );
	}

	// Create the backboard structure
	{
		mndl::assimp::AssimpNodeRef assimpNode = mSceneAiMesh.getAssimpNode( "structure_basketball_hoop_body.001" );
		mndl::assimp::AssimpMeshHelperRef assimpMeshHelper = assimpNode->mMeshes[0];
		ci::TriMesh mesh = assimpMeshHelper->mCachedTriMesh;

		physics->addMesh( mesh );
	}

	// Create the lines
// 	{
// 		mndl::assimp::AssimpNodeRef assimpNode = mSceneAiMesh.getAssimpNode( "felfestes_lines" );
// 		mndl::assimp::AssimpMeshHelperRef assimpMeshHelper = assimpNode->mMeshes[0];
// 		ci::TriMesh mesh = assimpMeshHelper->mCachedTriMesh;
// 
// 		physics->addMesh( mesh );
// 	}

	// Create the backboard
	{
		mndl::assimp::AssimpNodeRef assimpNode = mSceneAiMesh.getAssimpNode( "backboard_basketball_hoop_body" );
		mndl::assimp::AssimpMeshHelperRef assimpMeshHelper = assimpNode->mMeshes[0];
		ci::TriMesh mesh = assimpMeshHelper->mCachedTriMesh;

		physics->addMesh( mesh );
	}

	// Create the ring
	{
		mndl::assimp::AssimpNodeRef assimpNode = mSceneAiMesh.getAssimpNode( "ring_basketball_hoop_ring" );
		mndl::assimp::AssimpMeshHelperRef assimpMeshHelper = assimpNode->mMeshes[0];
		ci::TriMesh mesh = assimpMeshHelper->mCachedTriMesh;

		physics->addMesh( mesh );
	}

	// Create the scoreboard
	{
		mndl::assimp::AssimpNodeRef assimpNode = mSceneAiMesh.getAssimpNode( "scoreboard_Plane.001" );
		mndl::assimp::AssimpMeshHelperRef assimpMeshHelper = assimpNode->mMeshes[0];
		ci::TriMesh mesh = assimpMeshHelper->mCachedTriMesh;

		physics->addMesh( mesh );
	}

	// Create the stripe
	{
		mndl::assimp::AssimpNodeRef assimpNode = mSceneAiMesh.getAssimpNode( "adstripe_ads" );
		mndl::assimp::AssimpMeshHelperRef assimpMeshHelper = assimpNode->mMeshes[0];
		ci::TriMesh mesh = assimpMeshHelper->mCachedTriMesh;

		// make bigger for the back wall
		ci::AxisAlignedBox3f boundingBox = mesh.calcBoundingBox();
		Vec3f center = boundingBox.getCenter();
		Vec3f size   = boundingBox.getSize();
		size += Vec3f( 20.0f, 50.0f, 20.0f );
		center += Vec3f( 0.0f, 25.0f, 10.0f );
		physics->addBox( size, center );
	}
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
