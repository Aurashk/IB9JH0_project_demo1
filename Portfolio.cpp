#include "Portfolio.h"

double Portfolio::liquidated_total(const Market& ms)
{
	return ms.liquidate_portfolio(m_portfolio_map);
}

void Portfolio::add(const std::string& name, double amount)
{
	m_portfolio_map[name] += amount;
}

double Portfolio::remove(const std::string& name, double amount)
{
	// if we don't have enough to sell we just sell all the asset we own (possibly 0)
	amount = std::min<double>(m_portfolio_map[name], amount);

	m_portfolio_map[name] -= amount;

	// return the amount sold since its not necessarily amount specified
	return amount;
}

double Portfolio::operator[](const std::string& name)
{
	return m_portfolio_map[name];
}
