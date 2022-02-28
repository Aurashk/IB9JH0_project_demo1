#pragma once

/// <summary>
/// 
/// An interface for classes which record some quantity from an asset through time
/// 
/// </summary>
class AssetObserver
{

public:

	// update the observer for the current tick and asset value
	virtual void update(double value, unsigned int current_tick) = 0;

	// If this function returns true the observer will be dropped from the asset
	// as observing is no longer necessary
	virtual bool finished() const = 0;

};

