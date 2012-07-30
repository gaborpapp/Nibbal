#pragma once

#include "cinder/Cinder.h"

#include "PParams.h"

#include "AssimpLoader.h"
#include "Display.h"
#include "AdDisplay.h"
#include "CinderBullet.h"
#include "Physics.h"

namespace Nibbal {

class Scene
{
	public:
		void setup( Physics *physics );

		void update();
		void draw();

	private:
		void setupPhysics( Physics *physics );
		void startGame();

		mndl::assimp::AssimpLoader mSceneAiMesh;

		Nibbal::Display          mDisplay;
		AdDisplay                mAdDisplay;

		ci::params::PInterfaceGl mParams;
};

} // namespace Nibbal
