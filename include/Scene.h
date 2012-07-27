#pragma once

#include "cinder/Cinder.h"

#include "PParams.h"

#include "AssimpLoader.h"

namespace Nibbal {

class Scene
{
	public:
		void setup();

		void update();
		void draw();

		//void showParams( bool show );

	private:

		mndl::assimp::AssimpLoader mSceneAiMesh;
		//ci::params::PInterfaceGl mParams;
};

} // namespace Nibbal
