#pragma once

#include "coordinate.h"
#include "text.h"
#include "colors.h"

struct found_overlaps
{
	std::vector<text_object>::iterator		iter;
	size_t									count;

	found_overlaps()
		: count(0)
	{}

	found_overlaps(const std::vector<text_object>::iterator i, const size_t c)
		: iter(i), count(c)
	{}
};

class process
{
private:
	void great_circle(const coordinate& circle_center_coord, const pen_t p);

	void coordinate_net();
	void declination_numbers();
	void ecliptic();
	void galactic_equator();
	void constellation_boundaries();
	void milky_way();
	void stars();
	void star_designations();
	void asterisms();
	void fixed_texts();
	void ngc();
	void print_texts();
	void find_text_positions();
	void init_text_positions();
	void radiants();
	void custom_objects();

public:
	void run();
};
