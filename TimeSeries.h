#pragma once

#include "TimeDependentObject.h"
#include <vector>

// A general class which keeps track of time through ticks
// This is intended to be used as a base class to 
// some time-controlled process
class TimeSeries
{
private:
	// a dynamic array of timed objects that this class will update
	std::vector<TimeDependentObject*> m_timed_objects;

	// Amount of ticks that have passed since start of the time series
	unsigned int m_current_tick;

protected:
	// this is protected because we only intend to use this class as a base class
	TimeSeries();

	virtual void tick_update();

	void add_timed_object(TimeDependentObject* const  tdo);

public:

	// get the current tick of the time series
	unsigned int get_current_tick() const;
};

