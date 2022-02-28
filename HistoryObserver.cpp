#include "HistoryObserver.h"

HistoryObserver::HistoryObserver(unsigned int maximum_history)
	:
	m_maximum_history(maximum_history)
{}

void HistoryObserver::update(double value, unsigned int current_tick)
{
	// update price history after getting new price
	m_recent_history.push_back(value);

	// if maximum number of history points exceeded, drop the oldest one.
	if (m_recent_history.size() > m_maximum_history)
		m_recent_history.pop_front();
}

bool HistoryObserver::finished() const
{
	return false;
}

unsigned int HistoryObserver::get_price_history(double* const history, unsigned int amount) const
{
	if (amount > m_recent_history.size())
		amount = (unsigned int) m_recent_history.size();

	std::copy_n(m_recent_history.begin(), amount, history);

	return amount;
}
