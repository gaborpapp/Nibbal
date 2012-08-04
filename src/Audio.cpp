#include <assert.h>

#include "Audio.h"

using namespace ci;
using namespace std;

namespace Nibbal {

Audio::~Audio()
{
	stopAll();

	mPlayers.clear();
}

void Audio::setup( const fs::path &audioFolder )
{
	_loadPlayers( audioFolder );
}

void Audio::play( string name, float volume /* = 1.0f */, bool loop /* = false */ )
{
	stop( name );

	shared_ptr< FmodexPlayer > player = mPlayers[ name ];

	if( ! player )
		return;

	player->setPosition( 0 );
	player->setVolume( volume );
	player->setLoop( loop );
	player->play();
}

void Audio::stop( string name )
{
	shared_ptr< FmodexPlayer > player = mPlayers[ name ];

	if( ! player )
		return;

	player->stop();
	player->setPosition( 0 );
}

void Audio::stopAll()
{
	for( map< string, shared_ptr< FmodexPlayer > >::iterator it = mPlayers.begin(); it != mPlayers.end(); ++it )
	{
		it->second->stop();
	}
}

void Audio::setVolume( string name, float volume )
{
	shared_ptr< FmodexPlayer > player = mPlayers[ name ];

	if( ! player )
		return;

	player->setVolume( volume );
}

float Audio::getVolume( string name )
{
	shared_ptr< FmodexPlayer > player = mPlayers[ name ];

	if( ! player )
		return 0.0f;

	return player->volume;
}

void Audio::_loadPlayers( const fs::path folder )
{
	for( fs::directory_iterator it( folder ); it != fs::directory_iterator(); ++it )
	{
		if( fs::is_regular_file( *it ))
		{
			string name = folder.string() + "/" + it->path().filename().string();
			std::shared_ptr< FmodexPlayer > player = shared_ptr< FmodexPlayer >( new FmodexPlayer );
			player->loadSound( name );
			mPlayers.insert( make_pair( it->path().stem().string(), player ));
		}
	}
}

} // namespace Nibbal
