#pragma once

#include "dekart.h"

class point
{
public:
	double	r;
	double	alpha;

	point(const double& new_r = 0.0, const double& new_alpha = 0.0)
		: r(new_r), alpha(new_alpha)
	{}

	dekart conv2dekart() const
	{
		return dekart(cos(alpha) * r, sin(alpha) * r);
	}

	bool operator == (const point& rhs) const
	{
		return r == rhs.r  &&  alpha == rhs.alpha;
	}

	bool operator != (const point& rhs) const
	{
		return r != rhs.r  ||  alpha != rhs.alpha;
	}

	bool operator < (const point& rhs) const
	{
		if (r < rhs.r)
			return true;

		if (r > rhs.r)
			return false;

		return alpha < rhs.alpha;
	}
};

inline std::ostream& operator << (std::ostream& o, const point& p)
{
	o << p.conv2dekart();
	return o;
}
