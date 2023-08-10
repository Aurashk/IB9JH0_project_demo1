#pragma once

#include <map>
#include <memory>
#include <stdexcept>
#include "MarketInteractor.h"
#include "Market.h"
#include "Portfolio.h"
#include "HistoryObserver.h"

class Trader : public MarketInteractor
{
private:

	// a reference to the market the trader trades in
	Market& m_market;

	// asset price history is copied into this array for analysis
	std::vector<double> m_price_history;

	// observers which keep track of the recent price histories for each asset
	std::map<std::string, HistoryObserver*> m_history_tracker;

	// initial amount of cash the trader is constructed with
	double m_initial_cash;

	// currency held by trader which can be used to buy assets
	double m_cash;

	// names of the assets that the trader will trade
	std::vector<std::string> m_asset_names;

	// asset portfolio of the trader
	Portfolio m_portfolio;

	// buy some amount of the named asset at given price, 
	// adding it to our portfolio
	// if cash is insufficient we buy the max we can
	void buy(const std::string& name, double price, double amount);

	// sell some amount of the named asset at given price,
	// removing it from our portfolio
	// if we don't have enough of the asset we sell the max we can
	void sell(const std::string& name, double price, double amount);

	// outline method to determine how much of the particular should be bought
	double determine_buy_amount(const Asset& asset);

	// use some systematic method to determine how much of the asset should be sold
	double determine_sell_amount(const Asset& asset);

public:

	//
	// initial_cash - cash the trader starts with
	// asset_names - asset names that will be traded
	// number_of_history - amount of recent historical asset prices the 
	//                     trader uses to make decisions
	//
	Trader(double initial_cash, const std::vector<std::string>& asset_names, unsigned int number_of_history, Market& market);

	// this function is called to set up observers before interacting with the market
	void set_up() override;

	// interact with the provided asset (decide to buy or sell and then adjust portfolio)
	void interact() override;

	// retrieve current liquidated total of assets + cash
	double liquidated_total(const Market& m);

	// retrieve current profits from intital cash if all assets were liquidated 
	double liquidated_profits(const Market& m);

	// test the trader code is working. In practice better to move this function to a seperate class e.g TraderTesting
	static void test();
};