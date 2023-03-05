#include "AssetHistory.h"
#include "util.h"

// random seed is based on the current time
thread_local std::default_random_engine AssetHistory::RANDOM_GENERATOR{
	static_cast<long unsigned int>(time(0))};

using stringstream = std::stringstream;

// format the contents of array arr of double to stringstream ss
// contents are comma seperated
void format_line(const double *const arr, stringstream &ss, unsigned int m) {
	for (unsigned int i = 0; i < m - 1; ++i) {
		ss << arr[i] << ",";
	}

	ss << arr[m - 1] << "\n";
}

// read the contents of a comma seperated line and extract m numbers
void parse_line(double *const arr, const std::string &s, unsigned int m) {
	stringstream ss(s);
	unsigned int i = 0;
	std::string x;

	while (i < m && ss.good()) {
		std::getline(ss, x, ',');
		arr[i] = strtod(x.data(), NULL);
		++i;
	}
}

// read the contents of a comma seperated line and extract an unknown amount of
// numbers
void parse_line(std::vector<double> &arr, const std::string &s) {
	stringstream ss(s);
	std::string x;

	while (ss.good()) {
		std::getline(ss, x, ',');
		arr.push_back(strtod(x.data(), NULL));
	}
}

// create a formatted header ready to write to file from the array arr
void format_header(const std::vector<std::string> &arr, stringstream &ss) {
	for (unsigned int i = 0; i < arr.size() - 1; ++i) {
		ss << arr[i] << ",";
	}

	ss << arr[arr.size() - 1] << "\n";
}

// read a formatted header from string s and store each comma seperated string
// in arr
void parse_header(std::vector<std::string> &arr, const std::string &s) {
	stringstream ss(s);
	std::string x;

	while (ss.good()) {
		std::getline(ss, x, ',');
		arr.push_back(x.data());
	}
}

void AssetHistory::generate_log_price_history(
	const std::string &filename,
	const std::vector<std::string> &asset_names,
	double *mu,
	double *sigma,
	unsigned int n) {
	// the number of assets to generate for
	unsigned int m = asset_names.size();

	// open the file stream
	std::ofstream file(filename);

	// storage for output lines
	stringstream line;

	// storage for data at each time
	std::vector<double> timestep_data(m);

	// object to generate Z where Z ~ N(0, 1)
	std::normal_distribution<double> dist(0.0, 1.0);

	double ti = 0.0, Zi = 0.0;

	// write the header to file containing the asset names
	format_header(asset_names, line);
	file << line.str();

	for (unsigned int i = 0; i < n; ++i) {
		for (unsigned int j = 0; j < m; ++j) {
			Zi += dist(RANDOM_GENERATOR);

			// unit time increments
			ti = i;

			// obtain ln(price) where ln(price) ~ N(mu*t, sigma^2*t) from Z
			timestep_data[j] = ti * mu[j] + sigma[j] * Zi;
		}

		// empty the string stream
		line.str(std::string());
		line.clear();
		// load the data into the line
		format_line(timestep_data.data(), line, timestep_data.size());

		// write the line to file
		file << line.str();
	}

	file.close();
}

void AssetHistory::read_price_history(
	const std::string &filename,
	std::vector<std::string> &asset_names,
	double **const data,
	unsigned int m_max,
	unsigned int n_max) {
	// open the file stream
	std::ifstream file(filename);

	// input lines
	std::string line;

	// process the header
	std::getline(file, line);
	asset_names.resize(0);
	parse_header(asset_names, line);

	// determine how many assets to retrieve
	unsigned int m = std::min<unsigned int>(m_max, asset_names.size());

	unsigned int i = 0;

	while (i < n_max && file >> line) {
		parse_line(data[i], line, m);
		++i;
	}

	file.close();
}

void AssetHistory::test() {
	/////////////////////////////////////////////
	/// writing to file
	/////////////////////////////////////////////

	const std::string filename = "AssetHistory_test.csv";
	std::vector<double> mu({0.0, -1.0, 1.0});
	std::vector<double> sigma({1.0, 1.5, 2.0});

	unsigned int number_of_timesteps = 100;
	unsigned int number_of_assets = mu.size();
	std::vector<std::string> asset_names;

	// give names to all the assets
	asset_names.resize(number_of_assets);
	stringstream header;
	for (unsigned int i = 0; i < asset_names.size(); ++i) {
		header.str(std::string());
		header.clear();
		header << "Asset " << i;
		asset_names[i] = header.str();
	}

	generate_log_price_history(
		filename, asset_names, mu.data(), sigma.data(), number_of_timesteps);

	/////////////////////////////////////////////
	/// reading from file
	/////////////////////////////////////////////

	std::vector<std::vector<double>> data;

	data.resize(number_of_timesteps);

	for (auto &step : data)
		step.resize(mu.size());

	unsigned int m_max = data[0].size();
	unsigned int n_max = data.size();

	std::vector<double *> data_ptrs(data.size());

	// set each element of data_ptrs to point at each array (timestep)
	// in data
	std::transform(data.begin(), data.end(), data_ptrs.begin(), [](auto &val) {
		return val.data();
	});

	// empty the asset names before retrieving from file
	asset_names.resize(0);

	read_price_history(filename, asset_names, data_ptrs.data(), m_max, n_max);

	return;
}
