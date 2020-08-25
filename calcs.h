#pragma once

#include "pi.h"
#include "coordinate.h"
#include "config.h"
#include "point.h"
#include "text.h"

inline double hour2rad(const double hour)
{
	// right ascension goes clockwise only for the northern projections
	const double in_hour = (cfg->is_north() ? 24 - hour : hour);
	double ret_val = in_hour * pi / 12.0 - pi / 2.0;

	if (ret_val > 2 * pi)
		ret_val -= 2 * pi;

	return ret_val;
}

inline double deg2rad(const double deg)
{
	return deg / 180.0 * pi;
}

inline double rad2deg(const double rad)
{
	return rad / pi * 180.0;
}

inline double rad2hour(const double rad)
{
	// right ascension goes clockwise only for the northern projections
	const double in_rad = (cfg->is_north() ? 2*pi - rad : rad);
	double ret_val = in_rad * 12.0 / pi - 6;

	if (ret_val > 24.0)
		ret_val -= 24.0;

	return ret_val;
}

inline double cut_angle(const double l, const double r)
{
	double temp = l / (2 * r);
	return 2 * asin(temp > 1 ? 1.0 : temp);
}

inline double char_angle(const unsigned char chr, const double height, const double r, bool is_greek)
{
	return cut_angle((is_greek ? cfg->symbol_AR[chr] : cfg->char_AR[chr]) * height, r);
}

inline double correct_angle(const double angle)
{
	if (angle > 2 * pi)
		return angle - 2 * pi;
	else if (angle < 0)
		return angle + 2 * pi;

	return angle;
}

// checks if the given angle is between two other angles
inline bool is_in_angle(const double start, const double end, const double check)
{
	const double inStart = correct_angle(start);
	const double inEnd = correct_angle(end);
	const double inCheck = correct_angle(check);

	if (inStart < inEnd)
		return inCheck >= inStart  &&  inCheck <= inEnd;

	return inCheck >= inStart  ||  inCheck <= inEnd;
} 

inline bool is_in_radius(const double start, const double end, const double check)
{
	return check >= start  &&  check <= end;
}

inline double line_length(const point& start, const point& end)
{
	return sqrt(start.r * start.r + end.r * end.r - 2 * start.r * end.r * cos(end.alpha - start.alpha));
}

void precession(const double, const coordinate&, const double, coordinate&);

// internal
bool _overlap_text_object(const text_object& text, const text_object::position_t& vp, const point& obj_pos, const double diameter);

template <class Object>
bool overlap_text_object(const text_object& text, const text_object::position_t& vp, const Object& obj)
{
	return _overlap_text_object(text, vp, obj.coord.conv2point(), obj.diameter);
}

bool overlap_1st_text_2nd(const text_object::position_t& vp1, const text_object::position_t& vp2);

inline bool overlap_text_text(const text_object::position_t& vp1, const text_object::position_t& vp2)
{
	return overlap_1st_text_2nd(vp1, vp2)  ||  overlap_1st_text_2nd(vp2, vp1);
}

/*
struct polar_transform
{
	typedef coordinate&				result_type;
	typedef coordinate&				first_argument_type;
	typedef const coordinate&		second_argument_type;

	result_type operator () (first_argument_type T, second_argument_type T0) const
	{
		double deltaT0	= deg2rad(90.0 - T0.delta);
		double deltaT	= deg2rad(90.0 - T.delta);
		double alphaDiff = hour2rad(T0.alpha - T.alpha);

		double z = acos(sin(deltaT0) * sin(deltaT) + cos(deltaT0) * cos(deltaT) * cos(alphaDiff));
		double A = asin(sin(alphaDiff) * cos(deltaT) / sin(z));

		alphaDiff = T0.alpha - T.alpha;

		T.alpha = rad2hour(A + pi);
		T.delta = 90.0 - rad2deg(z);

		// determine the radians
		if (alphaDiff > 0)
			T.alpha *= -1;

		return T;
	};
};
*/

inline double calc_pole_distance(const coordinate& c)
{
	return cfg->is_north() ? 90.0 - c.delta : 90.0 + c.delta;
}

bool is_in_map_boundaries(const coordinate& c);
