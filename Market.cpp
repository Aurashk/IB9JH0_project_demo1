#include "Market.h"

Market::Market(const std::vector<std::string>& asset_names, const std::vector<std::vector<double>>& historical_prices)
	:
	m_maximum_ticks(0),
	m_asset_names(asset_names),
	m_historical_prices(historical_prices),
	TimeSeries()
{
	unsigned int number_of_assets = asset_names.size();

	// reserve space in memory for the number of assets
	m_assets.reserve(number_of_assets);

	// set up the assets
	for (unsigned int i = 0; i < number_of_assets; ++i)
	{
		// this array is where the assets are stored
		m_assets.push_back(Asset(m_asset_names[i], historical_prices[i], historical_prices[i].size()));

		// the asset map can be used to look up an asset by name
		m_asset_map[m_asset_names[i]] = &m_assets[i];

		// adding pointers to the assets here means that they will be updated at every tick
		add_timed_object(&m_assets[i]);

		// maximum ticks should be set to the longest list of historical prices
		m_maximum_ticks = std::max(m_maximum_ticks, historical_prices[i].size());
	}
}

void Market::tick_update()
{
	// everything we need to do at each tick is currently already done in the base class
	TimeSeries::tick_update();
}

void Market::run(unsigned int number_of_ticks, MarketInteractor& interactor, unsigned int interaction_ticks)
{

	// make sure we don't overrun the maximum number of ticks
	number_of_ticks = std::min(number_of_ticks, m_maximum_ticks - get_current_tick());
	
	if (number_of_ticks == 0)
			return;

	interactor.set_up();

	for (unsigned int i = 0; i < number_of_ticks; ++i)
	{
		tick_update();

		// check if we interact on this tick
		if ((get_current_tick() % interaction_ticks) == 0)
		{
			for (unsigned int j = 0; j < m_assets.size(); ++j)
				interactor.interact();
		}
	}

}

double Market::liquidate_portfolio(std::map<std::string, double>& portfolio) const
{
	double total = 0.0;

	for (unsigned int i = 0; i < m_assets.size(); ++i)
		total += portfolio[m_asset_names[i]] * m_assets[i].price();

	return total;
}

void Market::add_observer(const std::string& asset_name, std::unique_ptr<AssetObserver> obs)
{
	m_asset_map[asset_name]->connect_observer(std::move(obs));
}

const std::vector<Asset>& Market::get_assets() const
{
	return m_assets;
}

const Asset& Market::get_asset(const std::string& name) const
{
	return *(m_asset_map.at(name));
}
