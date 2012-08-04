#pragma once

#include "cinder/Cinder.h"

#include "PParams.h"

#include "AdDisplay.h"
#include "AssimpLoader.h"
#include "Audio.h"
#include "CinderBullet.h"
#include "Crowd.h"
#include "Display.h"
#include "KinectPlayer.h"
#include "ListenerMap.h"
#include "Physics.h"

namespace Nibbal {

class Scene
{
	public:
		enum EventType
		{
			ET_NO_USER,
			ET_NEW_USER,
			ET_WIN,
			ET_LOSE
		};

		void setup( Physics *physics );

		void update();
		void draw();

		void throwBall();
		void initGame();
		void startGame();
		void stopGame();

		template<typename T>
		void addCallback( EventType type, void (T::* callbackFunction)(), T * callbackObject )
		{
			mListenerMap->addCallback<T>( (int)type, callbackFunction, callbackObject );
		}

	private:
		void eventNoUser();
		void eventNewUser();
		void eventGoal();
		void eventMiss();
		void eventTimeOver();

		void setupPhysics( Physics *physics );

		mndl::assimp::AssimpLoader mSceneAiMesh;

		Nibbal::Display          mDisplay;
		AdDisplay                mAdDisplay;
		KinectPlayer             mKinectPlayer;
		Crowd                    mCrowd;
		Audio                    mAudio;
		unsigned short           mGoals;

		ci::params::PInterfaceGl mParams;

		std::shared_ptr<ListenerMap> mListenerMap;

		int mPeopleNumVariants;
		int mPeopleSeed;
};

} // namespace Nibbal
