#pragma once

class MarketInteractor
{
public:

	// this function will be called before any interactions with the market
	virtual void set_up() = 0;

	// interact with the market in some way 
	virtual void interact() = 0;
};