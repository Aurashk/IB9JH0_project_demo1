#include "Util.h"

namespace Util
{
	void transpose_data_matrix(const std::vector<std::vector<double>>& in, std::vector<std::vector<double>>& transpose)
	{
		// resize the transpose dimensions
		transpose.resize(in[0].size());

		for (auto& vec : transpose)
			vec.resize(in.size());

		for (unsigned int i = 0; i < in.size(); ++i)
		{
			for (unsigned int j = 0; j < in[0].size(); ++j)
				transpose[j][i] = in[i][j];
		}
	}

	void exp_matrix(std::vector<std::vector<double>>& mat)
	{
		// apply std::exp to every row
		for (auto& row : mat)
			std::transform(row.begin(), row.end(), row.begin(), [](double x) {return std::exp(x); });
	}
}