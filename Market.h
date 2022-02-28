#pragma once

#include <array>
#include <string>
#include <map>
#include <memory>
#include "Asset.h"
#include "MarketInteractor.h"
#include "TimeSeries.h"

// Needed for the test function only
#include "AssetHistory.h"
#include <sstream>
#include "Util.h"

class Market : public TimeSeries
{
private:

	// maximum number of ticks allowed in simulation
	unsigned int m_maximum_ticks;

	// All the different asset names in the simulation
	std::vector<std::string> m_asset_names;

	// Historical asset prices at each simulation timestep
	// m_historical_prices[i] is i'th timestep
	// m_historical_prices[i][j] is j'th asset price i'th timestep
	std::vector<std::vector<double>> m_historical_prices;

	// The assets in the simulation
	std::vector<Asset> m_assets;

	// map of asset names to the assets in the simulation
	std::map<std::string, Asset*> m_asset_map;

	// Update the simulation after a single tick of time has passed
	void tick_update();

public:

	Market(const std::vector<std::string>& asset_names, const std::vector<std::vector<double>>& historical_prices);

	////////////////////////////////////////////////////////////////////
	// Run the simulation for a chosen number of ticks
	// The interactor will interact with assets 
	// every time 'interaction_ticks' ticks have passed
  ////////////////////////////////////////////////////////////////////
	void run(unsigned int number_of_ticks, MarketInteractor& interactor, unsigned int interaction_ticks);

	// retrieve total of cash obtained by selling all the assets in the portfolio
	double liquidate_portfolio(std::map<std::string, double>& portfolio) const;

	// add an observer to the named asset
	void add_observer(const std::string& asset_name, std::unique_ptr<AssetObserver> obs);

	// retrieve a reference to all assets in the market
	const std::vector<Asset>& get_assets() const;

	// retrive an asset in the market by name
	const Asset& get_asset(const std::string& name) const;

};