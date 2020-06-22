#include "stl.h"
#pragma hdrstop

#include "constellation_boundary.h"

#include "point.h"
#include "ps_canvas.h"
#include "config.h"
#include "dekart.h"
#include "calcs.h"
#include "storage.h"
#include "pi.h"

coordinate	cb_pole1875( 0,90);
coordinate	cb_pole2000( 0, 0);
point		cb_center(0, 0);

double constellation_boundary_line::get_line_length(const point& p) const
{
	// calc the length of the line to draw
	double obim = 2 * pi * p.r;
	double maxObim = 2 * pi * canvas->get_extent();
	double retVal = obim * cfg->get_const_bound_angle() / 360.0;

	double min = maxObim * cfg->get_min_angle_length_factor();
	double max = maxObim * cfg->get_max_angle_length_factor();

	if (retVal < min)
		retVal = min;
	else if (retVal > max)
		retVal = max;

	return retVal;
}

double constellation_boundary_line::get_arc_angle(const point& p) const
{
	double maxObim = 2 * pi * canvas->get_extent();
	double minLength = maxObim * cfg->get_min_angle_length_factor();
	double maxLength = maxObim * cfg->get_max_angle_length_factor();
	double obim = 2 * pi * p.r;
	double arcLen = obim * cfg->get_const_bound_angle() / 360.0;

	if (arcLen < minLength)
		return minLength / obim * 2 * pi;
	else if (arcLen > maxLength)
		return maxLength / obim * 2 * pi;
	else
		return cfg->get_const_bound_angle() * pi / 180.0;
}

void constellation_boundary::decode(const std::string& line, coordinate& coord) const
{
	std::string temp_str;

	// read the declination
	temp_str = line.substr(9, 9);

	// if the number is in format "- 4.00000"
	// we must move the sign or else atof() will not work
	if (temp_str[1] == ' ')
	{
		temp_str[1] = temp_str[0];
		temp_str[0] = ' ';
	}

	coord.delta = atof(temp_str.c_str());

	// read the right ascension
	coord.alpha = atof(line.substr(0, 8).c_str());
}

void constellation_boundary::add(const std::string& start_point, const std::string& end_point)
{
	coordinate orig_start, orig_end;
	constellation_boundary_line cons_line;

	decode(start_point, orig_start);
	decode(end_point, orig_end);

	// calc the precession
	precession(1875.0, orig_start, 2000.0, cons_line.start);
	precession(1875.0, orig_end, 2000.0, cons_line.end);

	// is this a line or an arc
	if (orig_start.delta  ==  orig_end.delta)
		cons_line.is_line = false;
	else if (orig_start.alpha  ==  orig_end.alpha)
		cons_line.is_line = true;
	else
		throw "internal error: can't determine constellation_boundary_line::is_line";

	// check if the line is farther from the pole than allowed
	if (!is_in_map_boundaries()(cons_line.start)  &&  !is_in_map_boundaries()(cons_line.end))
		return;

	// check if we have this line in another constellation
	bool lineFound = false;
	if (!store->constellation_boundaries.empty())
	{
		std::vector<constellation_boundary>::const_iterator ci = store->constellation_boundaries.begin();

		int cnt = 0;

		while (ci != store->constellation_boundaries.end())
		{
			if (*ci != *this)
			{
				if (ci->have_line(cons_line) == true)
				{
					lineFound = true;
					break;
				}
			}

			++ci;
		}
	}

	// add the line to the vector if this is it's first instance
	if (lineFound == false)
		_lines.push_back(cons_line);
}

std::string constellation_boundary::read(std::istream& in, const std::string& first_line)
{
	// read a line from the stream
	const int LINE_SIZE = 80;
	char linePtr[LINE_SIZE];
	std::string line, prevLine(first_line);

	_code = first_line.substr(19, 4);

	line.reserve(LINE_SIZE);

	while (true)
	{
		in.getline(linePtr, LINE_SIZE);

		line = linePtr;

		// is this the same constellation as the last one?
		if (in.eof() == true  ||  line.length() != 25  ||  line.substr(19, 4) != _code)
		{
			// add the line which connects the first and the last point
			add(prevLine, first_line);

			// send the string back to the caller
			return line;
		} else {
			// add a new line to the vector
			add(prevLine, line);
		}

		prevLine = line;
	}
}

void constellation_boundary_line::draw_line() const
{
	if (cfg->get_const_bound_type() == config::full_bounds)
	{
		canvas->line(point(start.conv2point()), point(end.conv2point()));
	} else {
		point inStart(start.conv2point());
		point inEnd(end.conv2point());

		double startLength = get_line_length(inStart);
		double endLength = get_line_length(inEnd);
		double distance = line_length(inStart, inEnd);

		//canvas->line(inStart, inEnd);
		if (startLength + endLength < distance)
		{
			point endLine;

			if (inStart.r < inEnd.r)
			{
				endLine.r = inStart.r + startLength;
				endLine.alpha = inStart.alpha;
				canvas->line(inStart, endLine);

				endLine.r = inEnd.r - endLength;
				endLine.alpha = inEnd.alpha;
				canvas->line(inEnd, endLine);
			} else {
				endLine.r = inStart.r - startLength;
				endLine.alpha = inStart.alpha;
				canvas->line(inStart, endLine);

				endLine.r = inEnd.r + endLength;
				endLine.alpha = inEnd.alpha;
				canvas->line(inEnd, endLine);
			}
		} else {
			canvas->line(inStart, inEnd);
		}
	}
}

void constellation_boundary_line::draw_arc() const
{
	// calc the average radius for the two points
	point in_start(start.conv2point()), in_end(end.conv2point());
	double startRadius = line_length(cb_center, in_start);
	double end_radius = line_length(cb_center, in_end);
	double avgRadius = (startRadius + end_radius) / 2;

	// calc the start angle and the width of the arc
	double start_angle, end_angle, width;
	dekart center_dek;
	dekart start_dek;
	dekart end_dek;
	dekart rel_dek;

	// convert everything to Dekart coordinates
	center_dek = cb_center.conv2dekart();
	start_dek = in_start.conv2dekart();
	end_dek = in_end.conv2dekart();

	rel_dek = start_dek - center_dek;
	start_angle = acos(rel_dek.x / startRadius);
	if (rel_dek.y < 0)
		start_angle = 2 * pi - start_angle;

	rel_dek = end_dek - center_dek;
	end_angle = acos(rel_dek.x / end_radius);
	if (rel_dek.y < 0)
		end_angle = 2 * pi - end_angle;

	width = end_angle - start_angle;
	if (width < -pi)	// if we're drawing more than 12 hours
		width += 2 * pi;
	else if (width > pi)
		width -= 2 * pi;

	if (cfg->get_const_bound_type() == config::full_bounds)
	{
		canvas->arc(cb_center, start_angle, width, avgRadius);
	} else {
		point in_start_point(start.conv2point());
		point in_end_point(end.conv2point());

		double start_width = get_arc_angle(in_start_point);
		double end_width = get_arc_angle(in_end_point);

		if (start_width + end_width > (width < 0 ? -width : width))
		{
			if (width > 0)
				canvas->arc(cb_center, end_angle, -width, end_radius);
			else
				canvas->arc(cb_center, start_angle, width, startRadius);
		} else {
			if (width > 0)
			{
				canvas->arc(cb_center, start_angle, start_width, startRadius);
				canvas->arc(cb_center, end_angle, -end_width, end_radius);
			} else {
				canvas->arc(cb_center, start_angle, -start_width, startRadius);
				canvas->arc(cb_center, end_angle, end_width, end_radius);
			}
		}
	}
}

void constellation_boundary::paint() const
{
	// calc the precession & the center only once
	if (cb_pole2000.alpha == 0)
	{
		if (!cfg->is_north())
			cb_pole1875.delta = -90;

		precession(1875.0, cb_pole1875, 2000.0, cb_pole2000);
		cb_center = cb_pole2000.conv2point();

		/*
		canvas->stroke_circle(point(0, 0), 0.5);	// the 2000.0 pole
		canvas->stroke_circle(cb_center, 1);		// the 1875.0 pole
		*/
	}

	std::for_each(_lines.begin(), _lines.end(), std::mem_fun_ref(&constellation_boundary_line::draw));
}
