#pragma once

#include "coordinate.h"

struct radiant
{
	coordinate		coord;
	size_t			diameter;
	std::string		name;
	std::string		max_date;

	void decode(const char* line)
	{
		name = line;
		name = name.substr(0, name.find(' '));

		coord.alpha = atof(line + 12) / 360.0 * 24.0;
		coord.delta = atof(line + 16);

		max_date = line + 21;

		diameter = 4;	// fixed
	}
};
