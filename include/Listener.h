#pragma once

#include <boost/signals2/signal.hpp>

namespace Nibbal
{

class Listener
{
public:
	// Add functions with no argument to signal
	template<typename T>
	void addCallback( void (T::* callbackFunction)(), T * callbackObject )
	{
		mSignal.connect( boost::function<void ()>( boost::bind( callbackFunction, callbackObject )));
	}

	void callCallback()
	{
		mSignal();
	}

protected:
	 boost::signals2::signal<void ()> mSignal;
};

} //namespace Nibbal

