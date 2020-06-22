#pragma once

#include "point.h"

struct coordinate
{
	double	alpha;	// right ascension in hours
	double	delta;	// declination in degrees

	coordinate(const double new_alpha = 0.0, const double new_delta = 0.0)
	:	alpha(new_alpha), delta(new_delta)
	{}

	point conv2point() const;

	bool operator == (const coordinate& rhs) const
	{
		return alpha == rhs.alpha  &&  delta == rhs.delta;
	}
};

inline std::ostream& operator << (std::ostream& o, const coordinate& c)
{
	o << c.conv2point().conv2dekart();
	return o;
}
