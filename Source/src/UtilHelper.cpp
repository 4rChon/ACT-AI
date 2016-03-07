#include "UtilHelper.h"

double normaliseValues(std::vector<double> valueArr, std::vector<double> coeffArr)
{
	double total = 0.0;
	for (std::size_t i = 0; i < valueArr.size(); i++)
		total += 1 / (1 + std::exp(-coeffArr[i] * valueArr[i]));

	return ((2 / valueArr.size()) * total) - 1;
}