#pragma once
#include <vector>
#include <cmath>
#include <algorithm>

/* 
* A container for handy general utility functions used throughout the code
*/
namespace Util
{
	// transpose a matrix given by a vector of vectors
	void transpose_data_matrix(const std::vector<std::vector<double>>& in, std::vector<std::vector<double>>& transpose);

	// calculate naturual exponent of every element in a matrix
	void exp_matrix(std::vector<std::vector<double>>& mat);
}

