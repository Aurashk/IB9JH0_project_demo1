#pragma once
#include <string>
#include <queue>
#include <random>
#include <ctime>
#include "TimeDependentObject.h"
#include "AssetObserver.h"
#include <memory>

class Asset : public TimeDependentObject
{
private:
	// name the asset
	std::string m_name;

	// starting price of the asset
	double m_initial_price;

	// current price of the asset
	double m_price;

	// size of m_history array
	unsigned int m_num_history;

	// array containing all historical price data for this asset
	const std::vector<double>& m_history;

	// the asset observers are stored here until the asset object is destructed
	std::vector<std::unique_ptr<AssetObserver>> m_asset_observers_storage;

	// pointers to all the active observers for the asset
	std::deque<AssetObserver*> m_active_asset_observers;

public:

	// asset constructor
	Asset(const std::string& name, const std::vector<double>& price_history, unsigned int num_history);

	// need to manually implement these functions to prevent copying of m_asset_observers_storage
	Asset(const Asset& a);
	Asset& operator=(const Asset& a);

	// Update the asset after a single tick of time has passed
	void tick_update(unsigned int current_tick) override;

	// retrieve current asset price
	double price() const;

	// retrieve name of the asset
	const std::string& name() const;

	// connect a new observer to the asset, returning a reference to it
	void connect_observer(std::unique_ptr<AssetObserver> obs);

};