#include "TimeSeries.h"

TimeSeries::TimeSeries() :
	m_current_tick(0),
	m_timed_objects() 
{}

void TimeSeries::tick_update()
{
	for (unsigned int i = 0; i < m_timed_objects.size(); ++i)
		m_timed_objects[i]->tick_update(m_current_tick);

	++m_current_tick;
}

void TimeSeries::add_timed_object(TimeDependentObject* const tdo)
{
	m_timed_objects.push_back(tdo);
}

unsigned int TimeSeries::get_current_tick() const 
{ return m_current_tick; }
