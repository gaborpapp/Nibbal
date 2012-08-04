
#pragma once

#include "cinder/Cinder.h"
#include "cinder/Filesystem.h"
#include "FmodexPlayer.h"

namespace Nibbal {

class Audio
{
	public:
		~Audio();
		void setup( const ci::fs::path &audioFolder );

		void  play( std::string name, float volume = 1.0f, bool loop = false );
		void  stop( std::string name );
		void  stopAll();
		void  setVolume( std::string name, float volume );
		float getVolume( std::string name );

	private:
		void _loadPlayers( const ci::fs::path folder );

	private:
		std::map< std::string, std::shared_ptr< FmodexPlayer > > mPlayers;
};

} // namespace Nibbal
