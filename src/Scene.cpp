#include <assert.h>

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

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
	mAdDisplay.setup( app::getAssetPath( "ads" ), Vec2i( 1920, 210 ));
	mCrowd.setup( app::getAssetPath( "crowd" ), Vec2i( 1280, 560 ));

	// load scene
	mSceneAiMesh = assimp::AssimpLoader( app::getAssetPath( "models/scene/scene.obj" ) );
	mSceneAiMesh.disableSkinning();
	mSceneAiMesh.disableMaterials();
	if ( mSceneAiMesh.getNumMeshes() >= 2 )
	{
		// set repeat wrapping for floor, this property is not stored in .obj
		gl::Texture &floorTexture = mSceneAiMesh.getAssimpNodeTexture( "court_Plane" );
		assert( floorTexture );
		floorTexture.bind();
		floorTexture.setWrap( GL_REPEAT, GL_REPEAT );
		floorTexture.unbind();

		// set scoreboard
		gl::Texture &scoreBoardTexture = mSceneAiMesh.getAssimpNodeTexture( "scoreboard_Plane.001" );
		assert( scoreBoardTexture );
		scoreBoardTexture = mDisplay.getTexture();

		// set ads
		gl::Texture &adsTexture = mSceneAiMesh.getAssimpNodeTexture( "adstripe_ads" );
		assert( adsTexture );
		adsTexture = mAdDisplay.getTexture();

		// set crowd
		gl::Texture &crowdTexture = mSceneAiMesh.getAssimpNodeTexture( "crowd_crowd_plane" );
		assert( crowdTexture );
		crowdTexture = mCrowd.getTexture();
	}

	mKinectPlayer.setup( physics );
	mKinectPlayer.addCallback<Scene>( &Scene::eventGoal    , this );
	mDisplay     .addCallback<Scene>( &Scene::eventTimeOver, this );

	setupPhysics( physics );

	startGame();
}

void Scene::setupPhysics( Physics *physics )
{
	// Create the ground
	{
		TriMesh mesh = mSceneAiMesh.getAssimpNodeMesh( "court_Plane" );

		AxisAlignedBox3f boundingBox = mesh.calcBoundingBox();
		Vec3f center = boundingBox.getCenter();
		Vec3f size   = boundingBox.getSize();
		size += Vec3f( 0.0f, 20.0f, 0.0f );
		center += Vec3f( 0.0f, -10.0f, 0.0f );
		physics->addBox( size, center );
	}

	// Create the hoop body structure
	{
		TriMesh mesh = mSceneAiMesh.getAssimpNodeMesh( "structure_basketball_hoop_body.001" );
		physics->addMesh( mesh );

		mesh = mSceneAiMesh.getAssimpNodeMesh( "structure_talp_basketball_hoop_body.002" );
		physics->addMesh( mesh );
	}

	// Create the lines
	{
		TriMesh mesh = mSceneAiMesh.getAssimpNodeMesh( "felfestes_lines" );
		physics->addMesh( mesh );
	}

	// Create the backboard
	{
		TriMesh mesh = mSceneAiMesh.getAssimpNodeMesh( "backboard_basketball_hoop_body" );
		physics->addMesh( mesh );
	}

	// Create the ring
	{
		TriMesh mesh = mSceneAiMesh.getAssimpNodeMesh( "ring_basketball_hoop_ring" );
		physics->addMesh( mesh );

		ci::AxisAlignedBox3f boundingBox = mesh.calcBoundingBox();
		Vec3f center = boundingBox.getCenter();
		Vec3f size   = boundingBox.getSize();
	}

	// Create the scoreboard
	{
		TriMesh mesh = mSceneAiMesh.getAssimpNodeMesh( "scoreboard_Plane.001" );
		physics->addMesh( mesh );
	}

	// Create the stripe
	{
		TriMesh mesh = mSceneAiMesh.getAssimpNodeMesh( "adstripe_ads" );

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
	mKinectPlayer.update();
	mCrowd.update();
}

void Scene::draw()
{
	mSceneAiMesh.draw();
	mKinectPlayer.draw();
}

void Scene::throwBall()
{
//	mCrowd.energize( Rand::randFloat( 1.0f, 3.0f ));
	mKinectPlayer.throwBall();
}

void Scene::eventGoal()
{
	mCrowd.wave( Rand::randFloat( 1.8f, 2.5f ));
	mDisplay.setHome( mDisplay.getHome() + 1 );
}

void Scene::eventTimeOver()
{
	app::console() << "time over" << endl;
}

} // namespace Nibbal
