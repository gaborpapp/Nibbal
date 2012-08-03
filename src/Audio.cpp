#include <assert.h>

#include "Audio.h"

using namespace ci;
using namespace std;

namespace Nibbal {

Audio::~Audio()
{
	stopAll();

	mTracks.clear();
	mAudios.clear();
}

void Audio::setup( const fs::path &audioFolder )
{
	_loadAudios( audioFolder );
}

void Audio::play( string name, float volume /* = 1.0f */, bool loop /* = false */ )
{
	audio::TrackRef track = _findTrack( name );

	if( ! track )
	{
		audio::SourceRef aud = mAudios[ name ];

		if( ! aud )
			return;

		track = audio::Output::addTrack( aud );
		mTracks.insert( make_pair( name, track ));
	}

	track->setTime( 0 );
	track->setLooping( loop );
	track->play();
}

void Audio::stop( string name )
{
	audio::TrackRef track = _findTrack( name );

	if( ! track )
		return;

	track->stop();
	track->setTime( 0 );
}

void Audio::stopAll()
{
	for( std::map< std::string, ci::audio::TrackRef >::iterator it = mTracks.begin(); it != mTracks.end(); ++it )
	{
		it->second->stop();
	}
}

void Audio::setVolume( string name, float volume )
{
	audio::TrackRef track = _findTrack( name );

	if( ! track )
		return;

	track->setVolume( volume );
}

float Audio::getVolume( string name )
{
	audio::TrackRef track = _findTrack( name );

	if( ! track )
		return 0.0f;

	return track->getVolume();
}

audio::TrackRef Audio::_findTrack( string name )
{
	std::map< std::string, ci::audio::TrackRef >::iterator it = mTracks.lower_bound( name );

	if( it != mTracks.end() && !( mTracks.key_comp()( name, it->first )))
	{
		return it->second;
	}

	return audio::TrackRef();
}

void Audio::_loadAudios( const fs::path folder )
{
	for( fs::directory_iterator it( folder ); it != fs::directory_iterator(); ++it )
	{
		if( fs::is_regular_file( *it ))
		{
			string name = folder.string() + "/" + it->path().filename().string();
			audio::SourceRef aud = audio::load( name );
			string n = it->path().stem().string();
			mAudios.insert( make_pair( it->path().stem().string(), aud ));
		}
	}
}

} // namespace Nibbal
