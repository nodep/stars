#pragma once

#include "coordinate.h"

struct star
{
	enum {objectID = 1};

	typedef enum {oc_no_overlap, oc_partial, oc_total} overlap_code_type;
	// if this star is overlaped by another, brighter star
	// oc_no_overlap	- the star is not overlapped
	// oc_partial		- the star is partially overlapped
	// oc_total			- the star is hidden by another star

	coordinate			coord;		// the coordinates of the objects
	double				magnitude;	// the objects's brightness
	double				diameter;	// the objects's diameter on the paper (in mm)
	std::string			name;		// the name of the star

	std::string			fb_code;			// the Flamsteed or Bayer number of the star
	std::string			var_name;			// the name of the star if it is a variable
	size_t				multiple_system;	// the id of the multiple system the star is in
	bool				is_variable;		// if the star is variable 
	overlap_code_type	overlap_code;		// if the star is overlaped
	bool				is_pleiade;			// if the star belongs to the Pleiades

	star()
		:	multiple_system(0), is_variable(false), overlap_code(oc_no_overlap)
	{}

	void decode(const char* row);

	std::string get_flamsteed() const
	{
		if (fb_code.empty()  ||  fb_code[0] == ' ')
			return "";

		return fb_code.substr(0, 3) + ' ' + fb_code.substr(7);
	}

	std::string get_bayer() const
	{
		if (fb_code.empty()  ||  fb_code[3] == ' ')
			return "";

		return fb_code.substr(3);
	}

	bool operator < (const star& rhs) const
	{
		return magnitude < rhs.magnitude;
	}
};
