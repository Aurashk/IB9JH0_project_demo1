#include "Asset.h"

Asset::Asset(const std::string& name, const std::vector<double>& price_history, unsigned int num_history)
	:
	m_num_history(num_history),
	m_name(name),
	m_history(price_history),
	m_initial_price(price_history[0]),
	m_price(price_history[0])
{}

Asset::Asset(const Asset& a)
	:
	// copying an asset is like constructing a new asset with the same parameter - the observers are lost
	Asset(a.name(), a.m_history, a.m_num_history)
{
}

Asset& Asset::operator=(const Asset& a)
{
	// deal with self assignment
	if (this == &a)
		return *this;

	// use the copy constructor because we don't need to clean up the existing asset (doesn't allocate memory)
	*this = Asset(a);
	return *this;
}

void Asset::tick_update(unsigned int current_tick)
{
	// set the price to the current tick
	m_price = m_history[current_tick];

	// iterate over all observers 
	for (auto it = m_active_asset_observers.begin(); it != m_active_asset_observers.end(); ++it)
	{
		// call the update function
		(*it)->update(m_price, current_tick);

		// if the observer is triggered we don't need the pointer anymore
		// so we erase it from the observer queue
		if ((*it)->finished())
			m_active_asset_observers.erase(it);
	}

}

double Asset::price() const
{
	return m_price;
}

const std::string& Asset::name() const
{
	return m_name;
}

void Asset::connect_observer(std::unique_ptr<AssetObserver> obs)
{
	m_active_asset_observers.push_back(obs.get());
	m_asset_observers_storage.push_back(std::move(obs));
}