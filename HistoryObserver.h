#pragma once
#include "AssetObserver.h"
#include <queue>

/// <summary>
/// 
/// A class of object which can be added to assets 
/// in order to record a specified amount of recent #
/// asset price history
/// 
/// </summary>
class HistoryObserver : public AssetObserver
{

	// maximum recent history to track
	unsigned int m_maximum_history;

	// double ended queue to store the recent history of the asset 
	std::deque<double> m_recent_history;

public:

	HistoryObserver(unsigned int maximum_history);

	// this function will be called at every time the observed asset updates
	void update(double value, unsigned int current_tick) override;

	// this will always return false to keep the observer attached to the asset
	virtual bool finished() const;

	// copy the price history into an array, amount of data specified by 'amount'
	// if total history is less than amount, copy all the available history
	// returns the amount copied
	unsigned int get_price_history(double* const history, unsigned int amount) const;
};

