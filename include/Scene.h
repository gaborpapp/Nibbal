#pragma once

#include "cinder/Cinder.h"

#include "PParams.h"

#include "AssimpLoader.h"
#include "Display.h"
#include "AdDisplay.h"
#include "CinderBullet.h"
#include "KinectPlayer.h"
#include "Crowd.h"
#include "Audio.h"
#include "Physics.h"

namespace Nibbal {

class Scene
{
	public:
		void setup( Physics *physics );

		void update();
		void draw();

		void throwBall();

	private:
		void eventGoal();
		void eventMiss();
		void eventTimeOver();

	private:
		void setupPhysics( Physics *physics );
		void startGame();

		mndl::assimp::AssimpLoader mSceneAiMesh;

		Nibbal::Display          mDisplay;
		AdDisplay                mAdDisplay;
		KinectPlayer             mKinectPlayer;
		Crowd                    mCrowd;
		Audio                    mAudio;

		ci::params::PInterfaceGl mParams;
};

} // namespace Nibbal
