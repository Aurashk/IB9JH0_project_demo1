#pragma once

class TimeDependentObject
{
public:

	virtual void tick_update(unsigned int current_tick) = 0;
};