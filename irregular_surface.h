#pragma once

#include "coordinate.h"
#include "colors.h"

struct irregular_surface
{
	bool									should_fill;
	pen_t									plot_pen;
	color_t									fill_color;

	std::vector<std::vector<coordinate> >	points;

	irregular_surface()
		: should_fill(true), plot_pen(thin_dots), fill_color(white)
	{
		// the default path
		points.push_back(std::vector<coordinate>());
	}

	void add_point(const coordinate& coord)
	{
		points.back().push_back(coord);
	}

	void break_path()
	{
		points.push_back(std::vector<coordinate>());
	}

	void clear()
	{
		points.clear();
		points.push_back(std::vector<coordinate>());
	}
};
