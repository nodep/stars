#include "stl.h"
#pragma hdrstop

#include "ps_canvas.h"

#include "pi.h"
#include "point.h"
#include "config.h"
#include "calcs.h"
#include "text.h"
#include "star.h"

post_script_canvas* canvas = 0;

void post_script_canvas::set_color(const color_t col)
{
	if (col != _color)
	{
		if (col > black && col < white)
			_ps_file << ((double)col / 85) + 0.85 << " setgray" << std::endl;
		else if (col == white)
			_ps_file << "1 setgray" << std::endl;
		else if (col == black)
			_ps_file << "0 setgray" << std::endl;
		else if (col == red)
			_ps_file << "1 0 0 setrgbcolor" << std::endl;
		else if (col == green)
			_ps_file << "0 1 0 setrgbcolor" << std::endl;
		else if (col == blue)
			_ps_file << "0 0 1 setrgbcolor" << std::endl;

		_color = col;
	}
}

void post_script_canvas::set_pen_direct(const pen_t pen)
{
	set_color(black);

	if (pen != _pen)
	{
		switch (pen)
		{
		case thin_solid:
			_ps_file << "0.06 setlinewidth [] 0 setdash" << std::endl;
			break;
		case thin_dots:
			_ps_file << "0.07 setlinewidth [0.25] 0 setdash" << std::endl;
			break;
		case thin_dashed:
			_ps_file << "0.07 setlinewidth [0.5] 0 setdash" << std::endl;
			break;
		case thick_solid:
			_ps_file << "0.10 setlinewidth [] 0 setdash" << std::endl;
			break;
		}

		_pen = pen;
	}
}

void post_script_canvas::set_pen_width(const double& width)
{
	if (_pen != other  ||  _curr_pen_width != width)
	{
		_ps_file << width << " setlinewidth [] 0 setdash" << std::endl;
		_pen = other;
		_curr_pen_width = width;
	}
}

void post_script_canvas::set_font(const std::string& font_name, const double& height)
{
	// set the font only of it is not already the current font
	if (_curr_font_name != font_name  ||  _curr_font_height != height)
	{
		_ps_file << "/" << font_name << " findfont " << height << " scalefont setfont" << std::endl;
		_curr_font_name = font_name;
		_curr_font_height = height;
	}
}

void post_script_canvas::line(const point& start, const point& end)
{
	_ps_file << "newpath " << start << " moveto ";
	_ps_file << end << " lineto stroke" << std::endl;
}

void post_script_canvas::arc(const point& center, const double& start, const double& width, const double& r)
{
	double in_start = 180 / pi * start;
	double in_width = 180 / pi * width;

	_ps_file << "newpath " << center << " " << r << " " << in_start << " ";
	_ps_file << in_start + in_width;
	if (width > 0)
		_ps_file << " arc";
	else
		_ps_file << " arcn";

	// close the path & stroke it
	_ps_file << " stroke" << std::endl;
}

std::string post_script_canvas::get_circle_path(const dekart& center, const double& r)
{
	std::stringstream sstr;
	sstr << "newpath " << center << " " << r << " 0 180 arc ";
	sstr << center << " " << r << " 180 0 arc closepath" << std::endl;

	return sstr.str();
}

struct path
{
	std::ofstream&	output;
	bool			is_in_subpath_first;

	path(std::ofstream& o)
		: is_in_subpath_first(true), output(o)
	{
		output << "newpath" << std::endl;
	}

	void close_subpath()
	{
		if (!is_in_subpath_first)
		{
			output << "closepath" << std::endl;
			is_in_subpath_first = true;
		}
	}

	void fill()
	{
		output << "eofill" << std::endl;
	}

	void stroke()
	{
		output << "stroke" << std::endl;
	}

	void operator () (const coordinate& coord)
	{
		if (is_in_subpath_first)
		{
			output << coord << " moveto" << std::endl;
			is_in_subpath_first = false;
		} else {
			output << coord << " lineto" << std::endl;
		}
	}
};

void post_script_canvas::draw(const irregular_surface& isoph)
{
	comment("milky way");

	path isophote(_ps_file);

	std::vector<std::vector<coordinate> >::const_iterator sub_paths_iter = isoph.points.begin();
	while (sub_paths_iter != isoph.points.end())
	{
		std::vector<coordinate>::const_iterator points_iter = sub_paths_iter->begin();
		while (points_iter != sub_paths_iter->end())
		{
			isophote(*points_iter);
			++points_iter;
		}

		isophote.close_subpath();

		++sub_paths_iter;
	}

	if (isoph.should_fill)
	{
		set_color(isoph.fill_color);
		isophote.fill();
	} else {
		set_pen_direct(isoph.plot_pen);
		isophote.stroke();
	}
}

void post_script_canvas::draw(const custom_object& cu)
{
	if (cu.sign_type == "dot")
	{
		set_color(black);
		_ps_file << get_circle_path(cu.coord.conv2point(), 0.5) << "fill" << std::endl;
	} else if (cu.sign_type == "none") {
		// skip this one
	} else {
		log_stream << "<!warning!>Unknown custom object type '" << cu.sign_type << "'. Object not drawn.\n";
	}
}

void post_script_canvas::draw(const star& s)
{
	point position = s.coord.conv2point();
	double r = s.diameter;

	// set the brush to black
	set_color(black);

	// draw the star
	dekart dek = position.conv2dekart();

	// the star's name
	if (!s.name.empty())
		comment("star " + s.name);
	else if (!s.fb_code.empty())
		comment("star " + s.fb_code);

	// if the star is double/multiple
	if (s.multiple_system != 0  &&  cfg->draw_multiple_sign())
	{
		double beta = pi / 2 + position.alpha;
		double width = r * 0.15;	// the width of the line
		double length = 0.4;		// the length of the line
		double line_angle = cut_angle(width, r);

		// store the path in a string -- don't plot directly
		std::stringstream frame_stream;

		// calc the starting point
		point conv(r, beta - line_angle / 2);
		dekart start = dek + conv.conv2dekart() + point(length, beta).conv2dekart();

		frame_stream << start << " moveto" << std::endl;

		// b
		conv.r = width;
		conv.alpha = beta + pi / 2;
		frame_stream << conv << " rlineto" << std::endl;

		// d
		frame_stream << dek << " " << r << " " << rad2deg(beta + line_angle / 2) << " "
				<< rad2deg(beta + pi - line_angle / 2) << " arc" << std::endl;

		// e
		conv.r = length;
		conv.alpha = beta + pi;
		frame_stream << conv << " rlineto" << std::endl;

		// f
		conv.r = width;
		conv.alpha = beta - pi / 2;
		frame_stream << conv << " rlineto" << std::endl;

		// h
		frame_stream << dek << " " << r << " " << rad2deg(beta + pi + line_angle / 2) << " "
				<< rad2deg(beta - line_angle / 2) << " arc" << std::endl;

		// copy the path to a string
		std::string frame_path;
		frame_path = frame_stream.str();

		if (cfg->draw_star_frame()  &&  s.overlap_code != star::oc_no_overlap)
		{
			comment("multiple overlap frame");
			set_pen_width(cfg->get_star_frame_width() * 2);
			set_color(white);
			_ps_file << frame_path;
			_ps_file << "closepath stroke" << std::endl;
		}

		comment("multiple fill");
		set_color(black);
		_ps_file << frame_path;
		_ps_file << "fill" << std::endl;

	} else {
		// the white frame
		if (cfg->draw_star_frame()  &&  s.overlap_code != star::oc_no_overlap)
		{
			set_color(white);
			_ps_file << get_circle_path(dek, r + cfg->get_star_frame_width()) << "fill" << std::endl;
		}

		// the black circle
		set_color(black);
		_ps_file << get_circle_path(dek, r) << "fill" << std::endl;
	}

	// draw a white ring if the star is variable
	if (s.is_variable  &&  cfg->get_variable_symbol_type() == config::vst_ring)
	{
		double line_width = s.diameter / 5.5;
		double white_r = s.diameter * 0.65;
		
		set_pen_width(line_width);
		set_color(white);

		_ps_file << get_circle_path(dek, white_r) << "stroke" << std::endl;

		set_pen_direct(thin_solid);
		set_color(black);
	}
}

void post_script_canvas::draw(const galaxy& gal)
{
	// the galaxy's position on the map
	point position(gal.coord.conv2point());

	// the name/code
	if (gal.is_messier)
		comment("galaxy " + gal.messier_id);
	else
		comment("galaxy " + gal.name);
	
	// set the thin solid pen
	set_pen_direct(thin_solid);

	// save the graphic state
	_ps_file << "gsave" << std::endl;

	// move the coordinate center to the center of the galaxy
	_ps_file << position << " translate" << std::endl;

	// scale to draw an ellipse
	_ps_file << "1 0.5 scale" << std::endl;

	// create the ellipse path
	_ps_file << "newpath 0 0 " << gal.diameter << " 0 180 arc ";
	_ps_file << "0 0 " << gal.diameter << " 180 0 arc closepath" << std::endl;

	// set the brush to black & stroke the path
	_ps_file << "0 setgray stroke" << std::endl;

	// restore the graphic state
	_ps_file << "grestore" << std::endl;
}

void post_script_canvas::draw(const globular_cluster& gc)
{
	// the name/code
	if (gc.is_messier)
		comment("globular cluster " + gc.messier_id);
	else
		comment("globular cluster " + gc.name);

	set_pen_direct(thin_solid);

	point center(gc.coord.conv2point());
	_ps_file << get_circle_path(center, gc.diameter) << "stroke" << std::endl;
	_ps_file << get_circle_path(center, gc.diameter * .65) << "stroke" << std::endl;

	dekart center_dek(center.conv2dekart());
	_ps_file << "newpath "	<< center_dek + dekart(0, gc.diameter) << " moveto "
							<< center_dek - dekart(0, gc.diameter) << " lineto stroke" << std::endl;
	_ps_file << "newpath "	<< center_dek + dekart(gc.diameter, 0) << " moveto "
							<< center_dek - dekart(gc.diameter, 0) << " lineto stroke" << std::endl;
}

void post_script_canvas::draw(const open_cluster& oc)
{
	// the name/code
	if (oc.is_messier)
		comment("open cluster " + oc.messier_id);
	else
		comment("open cluster " + oc.name);

	set_pen_direct(thin_solid);

	point center(oc.coord.conv2point());
	_ps_file << get_circle_path(center, oc.diameter) << "stroke" << std::endl;
	_ps_file << get_circle_path(center, oc.diameter * .65) << "stroke" << std::endl;

	double dy = sin(pi * .1) * oc.diameter;
	double dx = cos(pi * .1) * oc.diameter;
	dekart center_dek(center.conv2dekart());
	_ps_file << "newpath "	<< center_dek + dekart(dx, dy) << " moveto "
							<< center_dek - dekart(dx, dy) << " lineto stroke" << std::endl;
	_ps_file << "newpath "	<< center_dek + dekart(-dx, dy) << " moveto "
							<< center_dek - dekart(-dx, dy) << " lineto stroke" << std::endl;
}

void post_script_canvas::draw(const planetary_nebula& pn)
{
	// the name/code
	if (pn.is_messier)
		comment("planetary nebula " + pn.messier_id);
	else
		comment("planetary nebula " + pn.name);

	set_pen_direct(thin_solid);

	// draw the nebula path
	_ps_file << get_circle_path(pn.coord.conv2point(), pn.diameter) << "stroke" << std::endl;

	// draw the star in the middle
	_ps_file << get_circle_path(pn.coord.conv2point(), pn.diameter / 5) << "fill" << std::endl;
}

void post_script_canvas::draw(const nebula& n)
{
	// the name/code
	if (n.is_messier)
		comment("nebula " + n.messier_id);
	else
		comment("nebula " + n.name);

	// draw the dashed circle
    set_pen_direct(thin_dots);
	_ps_file << get_circle_path(n.coord.conv2point(), n.diameter) << "stroke" << std::endl;
}

void post_script_canvas::draw(const radiant& rad)
{
	comment("meteor radiant " + rad.name);

	// set the fill color
	set_color(black);

	// the position of the center of the meteor radiant
	const point pos = rad.coord.conv2point();

	// draw a small (1mm) circle
	_ps_file << get_circle_path(pos, 0.5) << "fill" << std::endl;

	// draw the two lines heading from & to the center
	point start = pos;
	point end   = pos;

	const size_t length = 3;
	const size_t distance = 1;

	// 3mm long
	start.r -= distance;
	end.r -= distance + length;

	set_pen_direct(thin_solid);

	line(start, end);

	start = end = pos;

	start.r += distance;
	end.r += distance + length;

	line(start, end);

	// draw the two other lines
	point add_start((double) distance, pi / 2 + pos.alpha);
	point add_end(distance + length, pi / 2 + pos.alpha);

	dekart pos_dek = pos.conv2dekart();

	_ps_file << "newpath " << pos_dek + add_start.conv2dekart() << " moveto ";
	_ps_file << pos_dek + add_end.conv2dekart() << " lineto stroke" << std::endl;

	add_start.alpha = add_end.alpha = pos.alpha - pi / 2;

	_ps_file << "newpath " << pos_dek + add_start.conv2dekart() << " moveto ";
	_ps_file << pos_dek + add_end.conv2dekart() << " lineto stroke" << std::endl;
}

/*
void post_script_canvas::draw(const Quasar& quasar)
{
	// the name
	comment("quasar " + quasar.name);

	double x, y;
	point position(quasar.get_coordinate());
	position.conv2dekart(x, y);

	double r  = quasar.diameter;
	double r1 = r / 2.5;
	double r2 = r / 9;

	// !!! debug
	if (model->debug())
	{
	    SetPenDirect(THIN_DOTS);
		_ps_file << "newpath ";
		_ps_file << x << " " << y << " " << r << " 0 180 arc ";
		_ps_file << x << " " << y << " " << r << " 180 0 arc stroke" << endl;
	}

	_ps_file << "newpath ";
	_ps_file << x << " " << y << " " << r1 << " 270 90 arc ";
	_ps_file << x << " " << y + r - r2 << " " << r2 << " 0 180 arc ";
	_ps_file << x << " " << y << " " << r1 << " 90 270 arc ";
	_ps_file << x << " " << y - r + r2 << " " << r2 << " 180 0 arc fill" << endl;
}

void post_script_canvas::draw(const XRaySource& xraySrc)
{
	// the name
	comment("x ray source " + xraySrc.name);

	double x, y;
	point position(xraySrc.get_coordinate());
	position.conv2dekart(x, y);

	SetPenDirect(THIN_SOLID);

	_ps_file << "newpath " << x << " " << y << " " << xraySrc.diameter << " 0 180 arc ";
	_ps_file << x << " " << y << " " << xraySrc.diameter << " 180 0 arc stroke" << endl;

	SetPenWidth(xraySrc.diameter * 0.1);

	double angleStart, angleEnd;
	angleStart = position.Alpha - 3 * pi / 2 + pi / 3;
	angleEnd = position.Alpha - 3 * pi / 2 - 2 * pi / 3;
	double startX = cos(angleStart) * xraySrc.diameter * 0.75;
	double startY = sin(angleStart) * xraySrc.diameter * 0.75;
	double endX = cos(angleEnd) * xraySrc.diameter * 0.75;
	double endY = sin(angleEnd) * xraySrc.diameter * 0.75;

	_ps_file << "newpath " << x + startX << " " << y + startY << " moveto "
			<< x + endX << " " << y + endY << " lineto stroke" << endl;

	angleStart = position.Alpha - 3 * pi / 2 - pi / 3;
	angleEnd = position.Alpha - 3 * pi / 2 + 2 * pi / 3;
	startX = cos(angleStart) * xraySrc.diameter * 0.75;
	startY = sin(angleStart) * xraySrc.diameter * 0.75;
	endX = cos(angleEnd) * xraySrc.diameter * 0.75;
	endY = sin(angleEnd) * xraySrc.diameter * 0.75;

	_ps_file << "newpath " << x + startX << " " << y + startY << " moveto "
			<< x + endX << " " << y + endY << " lineto stroke" << endl;
}
*/

void post_script_canvas::comment(const std::string& comment)
{
    _ps_file << "% " << comment << std::endl;
}

void post_script_canvas::start_drawing()
{
	// read the extent of the map (in mm)
	_extent = cfg->get_map_radius();

	_ps_file.open(cfg->get_output_file_name().c_str());
	if (_ps_file.is_open() == false)
		throw "Unable to open output file";

	// the EPS header
	_ps_file << "%!PS-Adobe-1.0 EPSF-1.0" << std::endl;
	size_t bound = size_t((_extent + cfg->get_spare_space() + cfg->get_ra_text_height() + 2) / 25.4 * 72.0 * 2.0 * cfg->get_scale_factor());
	_ps_file << "%%BoundingBox: 0 0 " << bound << " " << bound << " " << std::endl << std::endl;

	// we'll save the graphics state
	_ps_file << "gsave" << std::endl << std::endl;

	// we'll work in millimeters
	_ps_file << 72.0 / 25.4 * cfg->get_scale_factor() << " " << 72.0 / 25.4 * cfg->get_scale_factor() << " scale" << std::endl;

	// move the center
	bound = size_t(_extent + cfg->get_spare_space() + cfg->get_ra_text_height() + 2);
	_ps_file << bound << " " << bound << " translate" << std::endl << std::endl;
}

void post_script_canvas::end_drawing()
{
	// we'll restore the graphics state
	_ps_file << std::endl << "grestore" << std::endl;

	// eject the page
	_ps_file << "showpage" << std::endl;
}

void post_script_canvas::draw(const text_object& txt)
{
	if (txt.curr_pos().position.r < txt.height)
	{
		log_stream << "<!warning!>Text " << txt.text << " is too close to the center. Text is not printed.\n";
		return;
	}

	// we don't print removed texts
	if (txt.is_removed)
	{
		log_stream << "not printing removed text " << txt.text << "\n";
		return;
	}

	comment((txt.is_greek ? "greek " : "") + std::string("text ") + txt.text);

	set_color(black);

	point textLD = txt.curr_pos().left_down_bound;
	point textRU = txt.curr_pos().right_up_bound;

	if (cfg->draw_text_frame())
	{
		set_pen_direct(thin_solid);
		arc(point(0, 0), textLD.alpha, txt.curr_pos().angle, textLD.r);
		arc(point(0, 0), textLD.alpha, txt.curr_pos().angle, textRU.r);
		line(textLD, point(textRU.r, textLD.alpha));
		line(point(textLD.r, textRU.alpha), textRU);
	}

	// set the font
	if (txt.is_greek)
		set_font("Symbol", txt.height);
	else
		set_font(cfg->get_names_font(), txt.height);

	// which half of the map is the text in
	bool is_up = true;
	if (sin(textLD.alpha) > 0   &&  sin(textRU.alpha) > 0)
	{
		is_up = true;
	} else if (sin(textLD.alpha) < 0   &&  sin(textRU.alpha) < 0) {
		is_up = false;
	} else {
		double start_angle = textLD.alpha;
		double end_angle = textRU.alpha;
		double down_angle, up_angle;
		if (start_angle > pi)
		{
			down_angle = pi*2 - start_angle;
			up_angle = end_angle;
		} else {
			up_angle = pi - start_angle;
			down_angle = end_angle - pi;
		}

		is_up = up_angle > down_angle;
	}

	// log_stream << "Text: " << txt.text << " alpha=" << txt.curr_pos().position.alpha << "\n";

	// print the chars one by one
	if (is_up)
	{
		point idealCharPos(txt.curr_pos().right_up_bound);
		point correctedCharPos;

		double cAngle;	// the angle of the current character
		for (int i = 0; txt.text[i] != '\0'; ++i)
		{
			cAngle = char_angle(txt.text[i], txt.height, idealCharPos.r, txt.is_greek);

			// don't print spaces
			if (txt.text[i] != ' ')
			{
				// save the current CTM
				_ps_file << "gsave" << std::endl;

				// correct the position of the charactes
				correctedCharPos = idealCharPos;
				if (txt.has_descent())
					correctedCharPos.r += txt.height * .25;
				if (txt.text[i] == '('  ||  txt.text[i] == ')')
					correctedCharPos.r += txt.height * .125;

				// translate
				_ps_file << correctedCharPos << " translate ";

				// rotate
				_ps_file << (idealCharPos.alpha - cAngle / 2) / pi * 180 - 90 << " rotate" << std::endl;

				// print the char
				_ps_file << "0 0 moveto (";
				if (txt.text[i] == '('  ||  txt.text[i] == ')')
					_ps_file << "\\";

				_ps_file << (char)txt.text[i] << ") show" << std::endl;

				// restore the CTM
				_ps_file << "grestore" << std::endl;
			}

			// move the angle
			idealCharPos.alpha -= cAngle;
		}
	} else {
		point idealCharPos(txt.curr_pos().position);
		point correctedCharPos;
		double cAngle;	// the angle of the current character
		for (int i = 0; txt.text[i] != '\0'; ++i)
		{
			cAngle = char_angle(txt.text[i], txt.height, txt.curr_pos().position.r - txt.height, txt.is_greek);

			// don't print spaces
			if (txt.text[i] != ' ')
			{
				// save the current CTM
				_ps_file << "gsave" << std::endl;

				// correct the position of the charactes
				correctedCharPos = idealCharPos;
				correctedCharPos.r -= txt.height * .25;
				if (txt.text[i] == '('  ||  txt.text[i] == ')')
					correctedCharPos.r -= txt.height * .125;

				// translate
				_ps_file << correctedCharPos << " translate ";

				// rotate
				_ps_file << 90 + (idealCharPos.alpha + cAngle / 2) / pi * 180 << " rotate" << std::endl;

				// print the char
				_ps_file << "0 0 moveto (";
				if (txt.text[i] == '('  ||  txt.text[i] == ')')
					_ps_file << "\\";

				_ps_file << (char)txt.text[i] << ") show" << std::endl;

				// restore the CTM
				_ps_file << "grestore" << std::endl;
			}

			// move the angle
			idealCharPos.alpha += cAngle;
		}
	}
}

void post_script_canvas::clip_extent()
{
	// save the current clipping path
	_ps_file << "clipsave" << std::endl;

	_ps_file << "newpath 0 0 " << _extent << " 0 180 arc ";
	_ps_file << "0 0 " << _extent << " 180 0 arc clip" << std::endl;
}

void post_script_canvas::clip_restore()
{
	// restore the saved clipping path
	_ps_file << "cliprestore" << std::endl;
}

void post_script_canvas::add_raw_ps()
{
	const std::string ps_file_name = cfg->read_str("AdditionalPS");
	if (!ps_file_name.empty())
	{
		log_stream << "adding contents of " << ps_file_name << " to canvas\n";

		std::ifstream ps_file(ps_file_name);
		std::string raw_ps((std::istreambuf_iterator<char>(ps_file)),
			std::istreambuf_iterator<char>());

		_ps_file << "\n% additional PS file: " << ps_file_name << "\n\n";
		_ps_file << raw_ps << "\n\n";
		_ps_file << "% additional PS ended\n";
	}
}