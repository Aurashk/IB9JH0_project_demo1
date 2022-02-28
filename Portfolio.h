#pragma once
#include <map>
#include <string>
#include "Market.h"

class Portfolio
{
	// The portfolio of the trader is a map of strings (asset names)
	// to doubles (amount of that asset which is held)
	std::map<std::string, double> m_portfolio_map;

public:

	// retrieve liquidated total of assets in the current market
	double liquidated_total(const Market& ms);

	// add specified amount of asset to the portfolio
	void add(const std::string& name, double amount);

	// remove specified amount of asset from the portfolio
	// if there are not enough of the asset, all of the asset
	// will be removed
	double remove(const std::string& name, double amount);

	// retrieve the amount of asset with the name specified
	double operator[](const std::string& name);
};

