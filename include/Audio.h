
#pragma once

#include "cinder/Cinder.h"
#include "cinder/Filesystem.h"
#include "cinder/Audio/Io.h"
#include "cinder/Audio/Output.h"

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
		ci::audio::TrackRef _findTrack( std::string name );
		void _loadAudios( const ci::fs::path folder );

	private:
		std::map< std::string, ci::audio::SourceRef > mAudios;
		std::map< std::string, ci::audio::TrackRef  > mTracks;
};

} // namespace Nibbal
