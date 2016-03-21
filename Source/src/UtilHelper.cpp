#include "UtilHelper.h"
#include <string>
#include <vector>

namespace util
{
	double normaliseValues(std::vector<double> valueArr, std::vector<double> coeffArr)
	{
		double total = 0.0;
		for (std::size_t i = 0; i < valueArr.size(); i++)
			total += 1 / (1 + std::exp(-coeffArr[i] * valueArr[i]));

		return ((2 / valueArr.size()) * total) - 1;
	}

	void getFiles(const fs::path& root, const std::string& ext, std::vector<fs::path>& ret)
	{
		if (!fs::exists(root) || !fs::is_directory(root)) return;

		fs::recursive_directory_iterator it(root);
		fs::recursive_directory_iterator endit;

		while (it != endit)
		{
			if(fs::is_regular_file(*it) && it->path().extension() == ext)
				ret.push_back(it->path().filename());
			++it;
		}
	}

	//void serialise()
	//{

	//}

	//void deserialise()
	//{

	//}
}