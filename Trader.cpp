#include "Trader.h"

Trader::Trader(double initial_cash, const std::vector<std::string>& asset_names, unsigned int number_of_history, Market& market)
	:
	m_market(market),
	m_price_history(number_of_history),
	m_initial_cash(initial_cash),
	m_cash(initial_cash),
	m_asset_names(asset_names),
	m_portfolio()
{
	// Make sure we don't allow construction with a negative amount of starting cash
	if (m_cash < 0.0)
		throw std::runtime_error("Can't have a negative amount of cash");
}

void Trader::set_up()
{
	// connect a history observer for each asset in traded by the trader
	for (auto asset : m_asset_names)
	{
		std::unique_ptr<HistoryObserver> obs = std::make_unique<HistoryObserver>(m_price_history.size());

		// store a pointer for easy access to the observer
		m_history_tracker[asset] = obs.get();

		// add observer to the market so that it records the asset history
		m_market.add_observer(asset, std::move(obs));
	}
}

void Trader::buy(const std::string& name, double price, double amount)
{
	if (amount <= 0.0)
		return;

	// this ensures we will not be able to overspend, if we don't have enough
	// cash we buy the max amount we can which is m_cash / price
	amount = std::min<double>(m_cash / price, amount);

	// add amount of asset
	m_portfolio.add(name, amount);

	// remove the cash we used to buy
	m_cash -= amount * price;

	// make sure cash is not negative due to rounding errors
	m_cash = std::max(0.0, m_cash);
}

void Trader::sell(const std::string& name, double price, double amount)
{
	if (amount <= 0.0)
		return;

	// remove amount of asset
	m_portfolio.remove(name, amount);

	// add cash from selling
	m_cash += amount * price;
}

double Trader::determine_buy_amount(const Asset& asset)
{
	// retrieve the historical prices from the asset 
	// note the amount of history we request might not be avaiable
	// amount_copied contains the actual amount of history extracted
	unsigned int amount_copied = m_history_tracker.at(asset.name())->get_price_history(m_price_history.data(), m_price_history.size());

	// not enough history to make decision if this condition satisfied, so don't buy anything
	if (amount_copied < m_price_history.size())
		return 0.0;

	// compute the moving average
	double average = 0.0;

	for (unsigned int i = 0; i < amount_copied; ++i)
		average += m_price_history[i];

	average /= (double) m_price_history.size();

	// if current price is less than 95% of moving average,
	// spend up to m_cash / number of assets buying it
	if (asset.price() < average * 0.95)
		return (m_cash / (double) m_asset_names.size()) / asset.price();

	// otherwise, don't buy anything
	return 0.0;
}

double Trader::determine_sell_amount(const Asset& asset)
{

	// retrieve the historical prices from the asset 
	// note the amount of history we request might not be avaiable
	// amount_copied contains the actual amount of history extracted
	unsigned int amount_copied = m_history_tracker.at(asset.name())->get_price_history(m_price_history.data(), m_price_history.size());

	// not enough history to make decision if this condition satisfied, so don't sell anything
	if (amount_copied < m_price_history.size())
		return 0.0;

	// compute the moving average
	double average = 0.0;

	for (unsigned int i = 0; i < amount_copied; ++i)
		average += m_price_history[i];

	average /= (double)m_price_history.size();

	// if current price is more than than 105% of moving average, we sell all we own
	if (asset.price() > average * 1.05)
		return m_portfolio[asset.name()];

	// otherwise, don't sell anything
	return 0.0;
}

void Trader::interact()
{
	for (const auto& asset_name : m_asset_names)
	{
		// note it would be better to compute the difference between 
		// buying and selling amount and just do one trade
		buy(asset_name, m_market.get_asset(asset_name).price(), determine_buy_amount(m_market.get_asset(asset_name)));
		sell(asset_name, m_market.get_asset(asset_name).price(), determine_sell_amount(m_market.get_asset(asset_name)));
	}
}

double Trader::liquidated_total(const Market& m)
{
	return m_cash + m_portfolio.liquidated_total(m);
}

double Trader::liquidated_profits(const Market& m)
{
	return liquidated_total(m) - m_initial_cash;
}

void Trader::test()
{
	/////////////////////////////////////////////
	/// write the asset history to file
	/////////////////////////////////////////////

	const std::string filename = "Market_test.csv";
	std::vector<double> mu({ 0.0, 0.0, 0.0});
	std::vector<double> sigma({ 1e-3, 1e-2, 1e-1 });

	unsigned int number_of_timesteps = 1000;
	unsigned int number_of_assets = mu.size();
	std::vector<std::string> asset_names;

	// give names to all the assets
	asset_names.resize(number_of_assets);
	std::stringstream header;
	for (unsigned int i = 0; i < asset_names.size(); ++i)
	{
		header.str(std::string());
		header.clear();
		header << "Asset " << i;
		asset_names[i] = header.str();
	}

	AssetHistory::generate_log_price_history(filename, asset_names, mu.data(), sigma.data(), number_of_timesteps);

	/////////////////////////////////////////////
	/// reading from file
	/////////////////////////////////////////////

	std::vector<std::vector<double>> data;

	data.resize(number_of_timesteps);

	for (auto& step : data)
		step.resize(mu.size());

	unsigned int m_max = data[0].size();
	unsigned int n_max = data.size();

	std::vector<double*> data_ptrs(data.size());

	// set each element of data_ptrs to point at each array (timestep)
	// in data
	std::transform(data.begin(), data.end(), data_ptrs.begin(),
		[](auto& val)
		{
			return val.data();
		});

	// empty the asset names before retrieving from file
	asset_names.resize(0);

	AssetHistory::read_price_history(filename, asset_names, data_ptrs.data(), m_max, n_max);

	std::vector<std::vector<double>> transposed_data;

	// need to transpose the data so each row represent the time-history of each asset
	Util::transpose_data_matrix(data, transposed_data);

	// since log-price was generated, calculate exponent of all values
	Util::exp_matrix(transposed_data);

	Market sim(asset_names, transposed_data);

	double initial_currency = 1000.0;
	unsigned int number_of_history = 50;

	// The trader which will trade in the market
	Trader trader(initial_currency, asset_names, number_of_history, sim);

	// number of times to print the output as the simulation runs
	unsigned int split = 10;
	unsigned int steps = number_of_timesteps / split;

	std::cout << "Starting cash " << initial_currency << std::endl;

	// run 10 simulations
	for (unsigned int i = 0; i < split; ++i)
	{
		// put the trader in the market simulation 
		// running for 10000 steps
		// the trader interacts (buys/sells) with market every 50 steps
		sim.run(steps, trader, 50);
		
		std::cout << "liquidated total after " << steps * (i + 1) << " steps: " << trader.liquidated_total(sim) << std::endl;
		std::cout << "Profits after " << steps * (i + 1) << " steps: " << trader.liquidated_profits(sim) << std::endl;
		std::cout << std::endl;
	}
}