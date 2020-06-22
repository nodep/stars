#pragma once

struct dekart
{
	double	x;
	double	y;

	dekart(const double new_x = 0.0, const double new_y = 0.0)
		: x(new_x), y(new_y)
	{}

	dekart operator + (const dekart& rhs) const
	{
		return dekart(x + rhs.x, y + rhs.y);
	}

	dekart operator - (const dekart& rhs) const
	{
		return dekart(x - rhs.x, y - rhs.y);
	}
};

inline std::ostream& operator << (std::ostream& o, const dekart& d)
{
	o << d.x << " " << d.y;
	return o;
}
