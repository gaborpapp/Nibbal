#pragma once

#include <boost/signals2/signal.hpp>

namespace Nibbal
{

class ListenerMap
{
	typedef boost::signals2::signal<void ()> ListenerSignal;
	typedef std::map<int, ListenerSignal*>   MapType;

public:
	// Add functions with single integer argument to signal
	template<typename T>
	void addCallback( int type, void (T::* callbackFunction)(), T * callbackObject )
	{
		ListenerSignal *signal = 0;
		MapType::iterator it = mSignals.lower_bound( type );

		if( it != mSignals.end() && !( mSignals.key_comp()( type, it->first )))
		{
			signal = it->second;
		}
		else
		{
			signal = new ListenerSignal;
			mSignals.insert( make_pair( type, signal ));
		}

		signal->connect( boost::function<void ()>( boost::bind( callbackFunction, callbackObject )));
	}

	void callCallback( int type )
	{
		MapType::iterator it = mSignals.lower_bound( type );

		if( it != mSignals.end() && !( mSignals.key_comp()( type, it->first )))
		{
			it->second->operator()();
		}
	}

protected:
	MapType mSignals;
};

} //namespace Nibbal
