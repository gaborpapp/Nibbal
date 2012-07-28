#pragma once

#include "cinder/Cinder.h"

#include "PParams.h"

#include "AssimpLoader.h"
#include "Display.h"
#include "AdDisplay.h"

namespace Nibbal {

class Scene
{
	public:
		void setup();

		void update();
		void draw();

		//void showParams( bool show );

	private:
		void startGame();

		mndl::assimp::AssimpLoader mSceneAiMesh;

		Nibbal::Display mDisplay;
		AdDisplay mAdDisplay;

		ci::params::PInterfaceGl mParams;
};

} // namespace Nibbal
