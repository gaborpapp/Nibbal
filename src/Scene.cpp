#include <assert.h>
#include <sstream>

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "PParams.h"
#include "Scene.h"

using namespace ci;
using namespace std;
using namespace mndl;

namespace Nibbal {

void Scene::setup( Physics *physics )
{
	mListenerMap = std::shared_ptr<ListenerMap>( new ListenerMap() );

	mDisplay.setup();
	mAdDisplay.setup( app::getAssetPath( "ads" ), Vec2i( 1920, 210 ));
	mCrowd.setup( app::getAssetPath( "crowd" ), Vec2i( 1280, 840 ) );
	mAudio.setup( app::getAssetPath( "sounds" ));

	mParams = params::PInterfaceGl( "Scene", Vec2i( 300, 300 ) );
	mParams.addPersistentSizeAndPosition();
	stringstream ss;
	int n = mCrowd.getNumVariants();
	ss << "min=1 max=" << n;
	mParams.addPersistentParam( "People variants", &mPeopleNumVariants, n / 2, ss.str() );
	mParams.addPersistentParam( "People seed", &mPeopleSeed, 587, "min=0" );

	// load scene
	mSceneAiMesh = assimp::AssimpLoader( app::getAssetPath( "models/scene/scene.obj" ) );
	mSceneAiMesh.disableSkinning();
	mSceneAiMesh.disableMaterials();

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
		adsTexture.bind();
		adsTexture.setWrap( GL_REPEAT, GL_REPEAT );
		adsTexture.unbind();

		// set crowd with wrapping
		gl::Texture &crowdTexture = mSceneAiMesh.getAssimpNodeTexture( "crowd_crowd_plane" );
		assert( crowdTexture );
		crowdTexture = mCrowd.getTexture();
		crowdTexture.bind();
		crowdTexture.setWrap( GL_REPEAT, GL_REPEAT );
		crowdTexture.unbind();
	}

	mKinectPlayer.setup( physics );
	mKinectPlayer.addCallback<Scene>( KinectPlayer::ET_NO_USER , &Scene::eventNoUser  , this );
	mKinectPlayer.addCallback<Scene>( KinectPlayer::ET_NEW_USER, &Scene::eventNewUser , this );
	mKinectPlayer.addCallback<Scene>( KinectPlayer::ET_GOAL    , &Scene::eventGoal    , this );
	mKinectPlayer.addCallback<Scene>( KinectPlayer::ET_MISS    , &Scene::eventMiss    , this );
	mDisplay     .addCallback<Scene>(                            &Scene::eventTimeOver, this );

	setupPhysics( physics );

	mAudio.play( "crowd-sports", 0.5f, true );
	initGame();
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

		// make bigger for bounce problem in slow machine
		ci::AxisAlignedBox3f boundingBox = mesh.calcBoundingBox();
		Vec3f center = boundingBox.getCenter();
		Vec3f size   = boundingBox.getSize();
		size   += Vec3f( 0.0f, 0.0f, 1.0f );
		center += Vec3f( 0.0f, 0.0f, 0.5f );
		physics->addBox( size, center );
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

void Scene::initGame()
{
	mDisplay.setActive( true );
	mDisplay.setTimeMax( 12, 00  );
	mDisplay.setTimeAct( 11, 00  );
	mDisplay.setPeriod( 4 );
	mDisplay.setHome( 78 );
	mDisplay.setGuest( 80 );
	mGoals = 0;
}

void Scene::startGame()
{
	initGame();
	mDisplay.start();
}

void Scene::stopGame()
{
	mDisplay.stop();
}

void Scene::update()
{
	mDisplay.update();
	mAdDisplay.update();
	mKinectPlayer.update();

	mCrowd.setNumVariants( mPeopleNumVariants );
	mCrowd.setCrowdSeed( mPeopleSeed );
	mCrowd.update();
}

void Scene::draw( bool drawPlayer )
{
	mSceneAiMesh.draw();

	mKinectPlayer.draw( drawPlayer );
}

void Scene::throwBall()
{
//	mCrowd.energize( Rand::randFloat( 1.0f, 3.0f ));
	mKinectPlayer.throwBall();
}

void Scene::eventNoUser()
{
	app::console() << "No user" << endl;
	mListenerMap->callCallback( ET_NO_USER );
}

void Scene::eventNewUser()
{
	app::console() << "New user" << endl;
	mListenerMap->callCallback( ET_NEW_USER );
}

void Scene::eventGoal()
{
	app::console() << "GOAL" << endl;
	mCrowd.wave( Rand::randFloat( 1.8f, 2.5f ));
	mDisplay.setHome( mDisplay.getHome() + 1 );
	mAudio.play( "goal" );

	mGoals++;

	if( mGoals == 3 )
		mListenerMap->callCallback( ET_WIN );
}

void Scene::eventMiss()
{
	app::console() << "MISS" << endl;
	mAudio.play( "no goal" );
}

void Scene::eventTimeOver()
{
	app::console() << "time over" << endl;
	mListenerMap->callCallback( ET_LOSE );
}

} // namespace Nibbal
