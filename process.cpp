#include "stl.h"
#pragma hdrstop

#include "process.h"
#include "config.h"
#include "ps_canvas.h"
#include "calcs.h"
#include "constellation_boundary.h"
#include "storage.h"
#include "pi.h"
#include "asterism_line.h"
#include "ngc.h"
#include "functors.h"
#include "custom_objects.h"

const double DEC_STEP = 10.0;

void process::coordinate_net()
{
	log_stream << "drawing coordinate net...\n";

	// init the default pen
	canvas->set_pen_direct(thin_solid);

	// draw the declination circles
	canvas->comment("declination circles");
	double dec;
	for (dec = DEC_STEP; dec <= cfg->get_max_pole_distance(); dec += DEC_STEP)
	{
		if (dec == 90.0)
			canvas->set_pen_direct(thick_solid);
		else
			canvas->set_pen_direct(thin_solid);

		canvas->stroke_circle(point(0, 0), dec * canvas->get_extent() / cfg->get_max_pole_distance());
	}

	// draw the last bounding circle if it is not in the net
	if ((int(cfg->get_max_pole_distance()) % 10) != 0)
	{
		canvas->set_pen_direct(thin_solid);
		canvas->stroke_circle(point(0, 0), canvas->get_extent());
	}

	// create the thicker pen for the last circle
	canvas->set_pen_width(0.5);

	// draw the last black circle
	canvas->stroke_circle(point(0, 0), cfg->get_map_radius() + cfg->get_ra_text_height() + 2);

	// draw the meridians
	canvas->comment("meridians");

	const double RA_STEP = 0.5;	// half hour

	point end_pt, start_pt;
	end_pt.r = canvas->get_extent();

	canvas->set_pen_direct(thin_solid);

	for (double ra = 0; ra < 24; ra += RA_STEP)
	{
		end_pt.alpha = pi * ra / 12;
		start_pt.alpha = end_pt.alpha;

		if (ra == 0  ||  ra == 6  ||  ra == 12  ||  ra == 18)
			start_pt.r = 3.0 / cfg->get_max_pole_distance() * canvas->get_extent();
		else if ((int)(ra * 2) == (int)ra * 2)
			start_pt.r = 10.0 / cfg->get_max_pole_distance() * canvas->get_extent();
		else
			start_pt.r = 30.0 / cfg->get_max_pole_distance() * canvas->get_extent();

		canvas->line(start_pt, end_pt);
	}

	canvas->comment("right ascension values");

	// print the values of RA
	char tmp_txt[5];
	text_object txt;
	txt.valid_positions.push_back(text_object::position_t());
	double chr_angle = char_angle('0', cfg->get_ra_text_height(), canvas->get_extent() + 1, false);
	int hour;
	for (hour = 0; hour < 24; ++hour)
	{
		// the number to print
		sprintf_s(tmp_txt, sizeof(tmp_txt), "%i", hour);

		txt.text = tmp_txt;
		txt.height = cfg->get_ra_text_height();
		txt.valid_positions.back().set_direct(point(canvas->get_extent() + cfg->get_ra_text_height() + 1, hour2rad(hour) - (hour > 9 ? chr_angle : chr_angle / 2)), txt);

		// print the text
		txt.print();
	}
}

void process::declination_numbers()
{
	text_object txt;
	txt.height = cfg->get_decl_text_height();
	txt.valid_positions.push_back(text_object::position_t());

	text_object::position_t& vp(txt.valid_positions.front());

	point text_pos;
	double text_dist = txt.height / 3;

	int numbers = 80;
	for (double dec = DEC_STEP; dec < cfg->get_max_pole_distance(); dec += DEC_STEP)
	{
		text_pos.r = dec * canvas->get_extent() / cfg->get_max_pole_distance() + txt.height + text_dist;
		text_pos.alpha = 3*pi/2 + cut_angle(text_dist, text_pos.r);

		char buff[10];
		sprintf_s(buff, sizeof(buff), "%iÊ", numbers);	// degree sign

		numbers -= 10;

		txt.text = buff;
		vp.set_direct(text_pos, txt);

		// see if the text overlaps any other text
		if (!store->text_overlaps_object(txt, vp) && store->get_overlapping_text(txt) == store->texts.end())
		{
			txt.print();
		}
	}
}

void process::run()
{
	stopwatch_t swatch;
	swatch.start();

	canvas->start_drawing();

	if (cfg->draw_milky_way())
		milky_way();

	if (cfg->draw_net())
	{
		coordinate_net();
	} else {
		// just draw a circle
		canvas->set_pen_direct(thin_solid);
		canvas->stroke_circle(point(0, 0), cfg->get_map_radius());
	}

	if (cfg->draw_ecliptic())
		ecliptic();

	if (cfg->draw_galactic_equator())
		galactic_equator();

	if (cfg->draw_const_bounds())
		constellation_boundaries();

	if (cfg->draw_stars())
		stars();

	if (cfg->draw_radiants())
		radiants();

	if (cfg->print_fixed_text())
		fixed_texts();

	if (cfg->draw_ngc_type() != config::ngc_none)
		ngc();

	if (cfg->draw_custom_objects())
		custom_objects();

	if (!store->texts.empty())
	{
		init_text_positions();
		find_text_positions();
		print_texts();
	}

	if (cfg->draw_net())
	{
		// the declination numbers
		declination_numbers();
	}

	canvas->end_drawing();

	log_stream << "Total time taken: " << swatch.get_msec() / 1000.0 << "sec\n";
}

void process::great_circle(const coordinate& circle_center_coord, const pen_t p)
{
	// the point of the circle center
	double circle_dec	= pi * circle_center_coord.delta / 180.0;
	double circle_ra	= pi * circle_center_coord.alpha / 12.0;

	// set the clipping region to be within the map boundaries
	canvas->clip_extent();

	double ctg_theta_a;
	double cos_gamma_a;
	double sin_gamma_a;
	double cos_gamma;
	double sin_gamma;
	double dec;
	coordinate		coord;
	point			point_first;

	point			point_start;
	point			point_end;
	const double	dots_frequency = cfg->get_circle_dots_frequency();

	// set the pen
	canvas->set_pen_direct(p);

	size_t point_count = 0;
	for (double ra = .0; ra < pi * 2.0; ra += dots_frequency)
	{
		cos_gamma = cos(ra);
		sin_gamma = sin(ra);
		ctg_theta_a = cos(circle_dec) / sin(circle_dec);
		cos_gamma_a = cos(circle_ra);
		sin_gamma_a = sin(circle_ra);

		dec = ctg_theta_a * cos_gamma_a * cos_gamma + ctg_theta_a * sin_gamma_a * sin_gamma;
		dec = atan(-dec);

		coord.alpha = (ra / pi) * 12.0;
		coord.delta = (dec / pi) * 180.0;

		point_start = point_end;
		point_end = coord.conv2point();

		++point_count;

		if (point_count == 1)
			point_first = point_end;
		else
			canvas->line(point_start, point_end);
	}

	canvas->line(point_end, point_first);

	// restore the clipping region
	canvas->clip_restore();
}

void process::ecliptic()
{
	log_stream << "drawing ecliptic...\n";

	// these should be the coordinates of the north ecliptic pole for 2000.0
	great_circle(coordinate(18.0, 66.5608), thick_solid);
}

void process::galactic_equator()
{
	log_stream << "drawing galactic equator...\n";

	// RA 12h 51.4m  dec 27ø 7.7'
	// these should be the coordinates of the north galactic pole for 2000.0
	canvas->comment("galactic equator");
	great_circle(coordinate(12.0 + 51.4 / 60.0, 27.0 + 7.7 / 60.0), thin_solid);
}

void process::milky_way()
{
	log_stream << "reading & drawing the milky way isophotes from <" << cfg->get_milky_way_isophotes_file().c_str() << ">...\n";

	// open the file 
	std::ifstream mw_isoph(cfg->get_milky_way_isophotes_file().c_str());

	if (!mw_isoph.is_open())
		throw "unable to open file " + cfg->get_milky_way_isophotes_file();

	char line[128];

	irregular_surface curr_isoph;
	bool is_first_dot = true;
	for (;;)
	{
		// read the line
		mw_isoph.getline(line, sizeof(line));

		if (mw_isoph.eof())
			break;

		if (*line == 's')
		{
			curr_isoph.break_path();
		} else if (*line != '\0') {
			if (is_first_dot)
			{
				size_t intensity = 10 - atoi(line + 18);
				curr_isoph.fill_color = (color_t) intensity;
			}

			curr_isoph.add_point(coordinate(atof(line), atof(line + 7)));

			is_first_dot = false;
		} else {
			// add path to storage
			store->isophotes.push_back(curr_isoph);

			curr_isoph.clear();

			is_first_dot = true;
		}
	}

	if (!curr_isoph.points.empty())
		store->isophotes.push_back(curr_isoph);

	canvas->comment("milky way");

	canvas->clip_extent();

	// now draw
	std::vector<irregular_surface>::iterator i = store->isophotes.begin();
	while (i != store->isophotes.end())
	{
		// set the pen/fill styles
		switch (cfg->get_milky_way_line_style())
		{
		case config::mws_solid:
			i->should_fill = false;
			i->plot_pen = thin_solid;
			break;
		case config::mws_dots:
			i->should_fill = false;
			i->plot_pen = thin_dots;
			break;
		case config::mws_fill:
			i->should_fill = true;	// the color has already been set
			break;
		}

		canvas->draw(*i);
		++i;
	}

	canvas->clip_restore();
}

void process::constellation_boundaries()
{
	log_stream << "reading & drawing constellation boundaries...\n";

	std::ifstream inFile("eq1875.dat");

	if (inFile.is_open() == false)
		throw "unable to open constellation boundary file (eq1875.dat)";

	char str[100];

	inFile.getline(str, 100);

	std::string first(str);

	constellation_boundary cb;

	// add the constellation boundarIes
	while (!first.empty())
	{
		first = cb.read(inFile, first);

		if (cb.has_lines())
			store->constellation_boundaries.push_back(cb);

		cb.clear();
	}

    canvas->comment("constellation boundaries");

	if (cfg->get_const_bounds_style() == config::ls_dots)
		canvas->set_pen_direct(thin_dots);
	else
		canvas->set_pen_direct(thin_solid);

	// set the clipping path
	canvas->clip_extent();

	// draw the constellation boundaries
	std::for_each(store->constellation_boundaries.begin(), store->constellation_boundaries.end(),
					std::mem_fun_ref(&constellation_boundary::paint));

	// restore the clipping path
	canvas->clip_restore();

	inFile.close();
}

struct get_star_position
{
	typedef const star&			argument_type;
	typedef point				result_type;

	result_type operator () (argument_type s) const
	{
		return s.coord.conv2point();
	}
};

void process::stars()
{
	log_stream << "reading & drawing stars...\n";

	// open the stars catalog
	std::ifstream stars_file("stars.dat");

	if (!stars_file.is_open())
		throw "can't open stars catalog file <stars.dat>";

	char linebuff[1024];
	std::string stars_line;
	stars_line.reserve(120);

	star s;

	while (!stars_file.eof())
	{
		stars_file.getline(linebuff, sizeof(linebuff));

		stars_line = linebuff;

		// the first empty line is eof
		if (stars_file.rdstate() == std::istream::failbit)
			break;

		s.decode(stars_line.c_str());

		// check the magnitude and break if the star is fainter than needed
		if (cfg->get_star_magnitude_limit() < s.magnitude)
			break;

		// we need the star only if it is in the map boundaries
		if (is_in_map_boundaries(s.coord))
			store->store_star(s);
	}

	// update the star diameters
	std::for_each(store->stars.begin(), store->stars.end(), update_diameter<star>());

	// the frames
	std::vector<point> star_positions(store->stars.size());

	// fill the vector with star positions
	std::transform(store->stars.begin(), store->stars.end(), star_positions.begin(), get_star_position());

	double length;
	std::vector<point>::iterator out_pos_iter = star_positions.begin();
	std::vector<point>::iterator in_pos_iter;
	std::vector<star>::iterator out_star_iter = store->stars.begin();
	std::vector<star>::iterator in_star_iter;
	while (out_pos_iter != star_positions.end())
	{
		in_star_iter = out_star_iter + 1;
		in_pos_iter = out_pos_iter + 1;
		while (in_pos_iter != star_positions.end())
		{
			// calc the distance between the stars
			length = line_length(*in_pos_iter, *out_pos_iter);

			// the length + the diam of the fainter star < diam of the brighter star
			if (length + in_star_iter->diameter < out_star_iter->diameter)
				in_star_iter->overlap_code = star::oc_total;
			else if (length <= (in_star_iter->diameter + out_star_iter->diameter))
				in_star_iter->overlap_code = star::oc_partial;

			++in_star_iter;
			++in_pos_iter;
		}

		++out_star_iter;
		++out_pos_iter;
	}

	// draw the asterisms so the lines don't overlap the star
	if (cfg->draw_asterism())
		asterisms();

	// now draw
	std::vector<star>::iterator star_iter = store->stars.begin();
	while (star_iter != store->stars.end())
	{
		// draw only the stars that are not totaly overlaped by other stars
		if (star_iter->overlap_code != star::oc_total)
			canvas->draw(*star_iter);

		++star_iter;
	}

	// collect the texts
	if (cfg->print_star_names())
	{
		std::vector<star>::const_iterator star_iter = store->stars.begin();

		text_object	text;
		while (star_iter != store->stars.end())
		{
			if (star_iter->is_variable  &&  cfg->get_variable_symbol_type() == config::vst_var)
			{
				// set the bounding object
				text.set_bounding(*star_iter);

				// set the text
				text.text = "var";
				text.height = cfg->get_var_text_height();

				// add the text to the vector
				store->texts.push_back(text);
			}

			if (!star_iter->name.empty()  &&  !(cfg->ignore_pleiades()  &&  star_iter->is_pleiade))
			{
				// set the bounding object
				text.set_bounding(*star_iter);

				// set the text
				text.text = star_iter->name;
				text.height = cfg->get_star_name_height();

				// add the text to the vector
				store->texts.push_back(text);
			}

			++star_iter;
		}
	}

	// get the star designations
	if (cfg->print_bayer_dsgn()  ||  cfg->get_var_dsgn_type() != config::vdt_none  ||  cfg->print_flamsteed_dsgn())
		star_designations();
}

struct greek_text
{
	size_t	txt_index;

	greek_text(const size_t ti)
		: txt_index(ti)
	{}

	text_object& get_text() const
	{
		return store->texts[txt_index];
	}

	bool operator < (const greek_text& rhs) const
	{
		return store->texts[txt_index].text < store->texts[rhs.txt_index].text;
	}

	bool operator > (const greek_text& rhs) const
	{
		return store->texts[txt_index].text > store->texts[rhs.txt_index].text;
	}

	bool operator <= (const greek_text& rhs)
	{
		return store->texts[txt_index].text <= store->texts[rhs.txt_index].text;
	}

	bool operator >= (const greek_text& rhs)
	{
		return store->texts[txt_index].text >= store->texts[rhs.txt_index].text;
	}
};

void process::star_designations()
{
	log_stream << "collecting star designations...\n";

	// this should happen only on some strange situations
	if (store->stars.empty())
		return;

	// this string is an index of greek letters into their ASCII values
	// in the Symbol font
	const std::string greek_map =	" ALP a BET b GAM g DEL d EPS e ZET z ETA h THE J IOT i KAP k LAM l MU  m"
									" NU  n XI  x OMI o PI  p RHO r SIG s TAU t UPS u PHI j CHI c PSI y OME w";

	text_object text;
	std::string bayer, bayerNum;
	std::string flamsteed, outText;
	size_t index;
	bool isGreek = false;
	double outHeight;
	std::vector<star>::const_iterator i = store->stars.begin();
	while (i != store->stars.end())
	{
		if (cfg->get_star_dsgn_mag_limit() > i->magnitude  &&  !i->fb_code.empty())
		{
			text.set_bounding(*i);				// set the bounding object

			// set the text
			flamsteed = i->fb_code.substr(0, 3);

			// kill the spaces
			index = flamsteed.find_first_not_of(" ");
			if (index == std::string::npos)
			{
				flamsteed = "";
			} else {
				// trim the string
				if (index > 0)
					flamsteed = flamsteed.substr(index);

				index = flamsteed.find_last_not_of(" ");
				flamsteed = flamsteed.substr(0, index + 1);
			}

			bayer = i->fb_code.substr(3, 3);
			bayerNum = i->fb_code[6];

			if (bayer.find_last_not_of(" ") == std::string::npos)
				bayer = "";

			outText = "";
			if (cfg->print_bayer_dsgn()   &&  !bayer.empty())
			{
				index = greek_map.find(" " + bayer);

				if (index == std::string::npos)
					throw std::string("unknown greek letter ") + bayer;

				outText = greek_map[index + 5];
				if (bayerNum[0] != ' ')
					outText += bayerNum;

				outHeight = cfg->get_bayer_dsgn_height();

				isGreek = true;
			}

			if (cfg->get_var_dsgn_type() != config::vdt_none  &&  outText.empty()
				&&  !i->var_name.empty()  &&  !(cfg->ignore_pleiades()  &&  i->is_pleiade))
			{
				if (cfg->get_var_dsgn_type() == config::vdt_not_v_num)
				{
					if (!(i->var_name[0] == 'V'  &&  isdigit(i->var_name[1])))
						outText = i->var_name;
				} else {
					outText = i->var_name;
				}

				outHeight = cfg->get_flamsteed_dsgn_height();

				isGreek = false;
			}

			if (cfg->print_flamsteed_dsgn()  &&  outText.empty()  &&  !flamsteed.empty()
				&& !(cfg->ignore_pleiades()  &&  i->is_pleiade))
			{
				outText = flamsteed;
				outHeight = cfg->get_flamsteed_dsgn_height();
				isGreek = false;
			}

			if (!outText.empty())
			{
				text.height = outHeight;
				text.text = outText;
				text.is_greek = isGreek;

				// add the text to the vector
				store->texts.push_back(text);
			}
		}

		++i;
	}

	if (cfg->print_bayer_dsgn()  &&  cfg->get_bayer_optimization_type() != config::bot_none)
	{
		// collect the indexes of all the greek texts
		std::vector<greek_text> greeks;

		std::vector<text_object>::iterator text_iter = store->texts.begin();
		while (text_iter != store->texts.end())
		{
			if (text_iter->is_greek)
				greeks.push_back(text_iter - store->texts.begin());

			++text_iter;
		}

		// now sort the indexes to greek texts
		std::sort(greeks.begin(), greeks.end());

		// now find designations of stars that overlap one another and have the same
		// greek letter for the designation, but a different number
		// i.e. beta1 cygni & beta2 cygni

		std::vector<greek_text>::iterator first_iter = greeks.begin();
		std::vector<greek_text>::iterator second_iter = first_iter + 1;

		// the indexes of the texts to erase after we're done
		std::vector<size_t> erase_texts;

		point start_pt, end_pt;
		double rad_summ;
		std::string new_text;
		size_t cases_found_count = 0;
		while (first_iter != greeks.end())
		{
			second_iter = first_iter + 1;

			// iterate through all the text that have the same greek letter
			while (second_iter != greeks.end())
			{
				std::string& first_text = first_iter->get_text().text;
				std::string& second_text = second_iter->get_text().text;

				if (first_text[0] != second_text[0])
					break;

				// if length == 2 && first char is alpha && second char is digit
				if (first_text.length() == 2  &&  second_text.length() == 2
					&& isalpha(first_text[0])  &&  isalpha(second_text[0])
					&& isdigit(first_text[1])  &&  isdigit(second_text[1]))
				{
					++cases_found_count;

					// calc the distance between the two stars
					start_pt = first_iter->get_text().bound_to;
					end_pt   = second_iter->get_text().bound_to;

					rad_summ = first_iter->get_text().bound_diameter + second_iter->get_text().bound_diameter;

					// if the stars overlap
					if (line_length(start_pt, end_pt) < rad_summ)
					{
						new_text = first_text + "," + second_text[1];

						// bind the the new text to the brighter star, and erase the
						// fainter star's designation text
						if (first_iter->get_text().bound_diameter > second_iter->get_text().bound_diameter)
						{
							if (cfg->get_bayer_optimization_type() == config::bot_reduce)
								first_text = new_text[0];
							else
								first_text = new_text;

							erase_texts.push_back(second_iter->txt_index);
						} else {
							if (cfg->get_bayer_optimization_type() == config::bot_reduce)
								second_text = new_text[0];
							else
								second_text = new_text;

							erase_texts.push_back(first_iter->txt_index);
						}

					}
				}

				++second_iter;
			}


			++first_iter;
		}

		// sort in descending order
		std::sort(erase_texts.begin(), erase_texts.end(), [](const auto& l, const auto& r) {return l > r; });

		// now erase the texts
		std::vector<size_t>::iterator index_iter = erase_texts.begin();
		while (index_iter != erase_texts.end())
		{
			store->texts.erase(store->texts.begin() + *index_iter);
			++index_iter;
		}

		log_stream << "optimized " << cases_found_count << " Bayer designations\n";
	}
}

struct star_designation		// flamsteed or bayer
{
	const star*		star_ptr;
	std::string			code;

	star_designation()
		: star_ptr(0)
	{}

	star_designation(const star* s, const std::string& desgn)
		: star_ptr(s), code(desgn)
	{}

	bool operator < (const star_designation& rhs) const
	{
		return code < rhs.code;
	}

	bool operator > (const star_designation& rhs) const
	{
		return code > rhs.code;
	}

	bool operator <= (const star_designation& rhs) const
	{
		return code <= rhs.code;
	}

	bool operator >= (const star_designation& rhs) const
	{
		return code >= rhs.code;
	}
};

struct normalize_designation
{
	typedef std::string	argument_type;
	typedef std::string	result_type;

	std::string operator () (const std::string& code)
	{
		std::string ret_val = code;
		if (isalpha(ret_val[0])  &&  isdigit(ret_val[2]))
		{
			ret_val[3] = ret_val[2];
			ret_val[2] = ' ';
		}

		return ret_val;
	}
};

void draw_asterism_line(const asterism_line& al)
{
	canvas->line(al.start_star->coord.conv2point(), al.end_star->coord.conv2point());
}

void process::asterisms()
{
	log_stream << "reading & drawing asterisms from file <" + cfg->get_asterism_file_name() + ">...\n";

	std::ifstream asterism(cfg->get_asterism_file_name().c_str());

	if (!asterism.is_open())
		throw "unable to open asterism file " + cfg->get_asterism_file_name();

	char p[30];

	asterism_line aster_line;
	std::string line;
	size_t ndx;
	while (true)
	{
		*p = '\0';
		asterism.getline(p, 30);

		if (asterism.eof()  &&  *p == '\0')
			break;

		if (strlen(p) == 0)		// skip the empty records
			continue;

		line = p;

		ndx = line.find("//");
		if (ndx != std::string::npos)
			line = alltrim(line.substr(0, ndx));

		if (line.empty())
			continue;

		aster_line.start_desgn = normalize_designation() (line.substr(0, 7));
		aster_line.end_desgn   = normalize_designation() (line.substr(9, 7));

		// normalize the designations
		store->asterism_lines.push_back(aster_line);
	}

	// collect all the flamsteed & bayer designations
	std::vector<star_designation> designations;

	std::string desgn;
	std::vector<star>::const_iterator star_iter = store->stars.begin();
	while (star_iter != store->stars.end())
	{
		if (!star_iter->fb_code.empty())
		{
			desgn = star_iter->get_flamsteed();
			if (!desgn.empty())
				designations.push_back(star_designation(&*star_iter, desgn));

			desgn = star_iter->get_bayer();
			if (!desgn.empty())
				designations.push_back(star_designation(&*star_iter, desgn));
		}

		++star_iter;
	}

	// sort by designation
	std::sort(designations.begin(), designations.end());

	// find the stars in the constellation asterisms by their designations
	star_designation sd;
	std::vector<star_designation>::iterator srch_res;
	std::vector<asterism_line>::iterator aster_iter = store->asterism_lines.begin();
	while (aster_iter != store->asterism_lines.end())
	{		  
		sd.code = aster_iter->start_desgn;
		srch_res = std::lower_bound(designations.begin(), designations.end(), sd);

		if (srch_res != designations.end()  &&  srch_res->code == sd.code)
			aster_iter->start_star = srch_res->star_ptr;

		sd.code = aster_iter->end_desgn;
		srch_res = std::lower_bound(designations.begin(), designations.end(), sd);

		if (srch_res != designations.end()  &&  srch_res->code == sd.code)
			aster_iter->end_star = srch_res->star_ptr;

		++aster_iter;
	}

	// now erase all the lines that have at least one invalid star pointer
	size_t c = 0;
	while (c < store->asterism_lines.size())
	{
		asterism_line& as_line(store->asterism_lines[c]);

		if (as_line.start_star == 0)
			log_stream << "<!warning!>Designation for star <" << as_line.start_desgn << "> is not found.\n";

		if (as_line.end_star == 0)
			log_stream << "<!warning!>Designation for star <" << as_line.end_desgn << "> is not found.\n";

		if (!asterism_line::is_valid()(as_line))
			store->asterism_lines.erase(store->asterism_lines.begin() + c);
		else
			++c;
	}

	// set the pen
	if (cfg->get_asterism_style() == config::ls_solid)
		canvas->set_pen_direct(thin_solid);
	else
		canvas->set_pen_direct(thin_dots);

	// draw the lines
	std::for_each(store->asterism_lines.begin(), store->asterism_lines.end(), draw_asterism_line);
}

void process::fixed_texts()
{
	log_stream << "reading & drawing fixed position texts from <" << cfg->get_fixed_text_file_name() << ">...\n";

	std::ifstream ftext(cfg->get_fixed_text_file_name().c_str());

	if (!ftext.is_open())
		throw "unable to open file " + cfg->get_fixed_text_file_name();

	coordinate pos_coord;
	point pos_pt;

	char p[250];
	text_object text;
	text.valid_positions.push_back(text_object::position_t());
	double angle;
	while (true)
	{
		ftext.getline(p, 250);

		if (ftext.eof())
			break;

		if (strlen(p) == 0)
			continue;

		text.height = atof(p);
		pos_coord.alpha = atof(p + 8);
		pos_coord.delta = atof(p + 15);

		if (p[14] == '-')
			pos_coord.delta = -pos_coord.delta;

		text.text = p + 21;

		pos_pt = pos_coord.conv2point();
		text.valid_positions.front().set_direct(pos_pt, text);

		angle = text.valid_positions.front().angle;

		switch (p[6])
		{
		case 'C':
			pos_pt.alpha -= angle / 2;
			break;
		case 'E':
			pos_pt.alpha -= angle;
			break;
		}

		text.valid_positions.front().set_direct(pos_pt, text);

		store->fixed_texts.push_back(text);
	}

	// draw
	std::vector<text_object>::const_iterator text_iter = store->fixed_texts.begin();
	while (text_iter != store->fixed_texts.end())
	{
		canvas->draw(*text_iter);
		++text_iter;
	}
}

template <class NGCObject>
void new_ngc(NGCObject& obj, const char* line)
{
	bool should_ignore = obj.decode(line);

	if (!should_ignore  &&  obj.magnitude <= cfg->get_mag_limit(obj)  &&  is_in_map_boundaries(obj.coord))
	{
		config::ngc_type dngc_type = cfg->draw_ngc_type();

		if ((dngc_type == config::ngc_messier_only  &&  obj.is_messier)
			|| dngc_type != config::ngc_messier_only)
		{
			store->store_ngc(obj);
		} else {
			log_stream << "not ploting: " << obj.name << "\n";
		}
	}
}

template <class NGCObject>
struct ngc_name
{
	typedef const NGCObject&		argument_type;
	typedef void					result_type;

	void operator () (const NGCObject& obj) const
	{
		text_object text;

		if (cfg->get_ngc_names_type() == config::ngcname_messier)
		{
			text.text = obj.messier_id;
		} else if (cfg->get_ngc_names_type() == config::ngcname_ngc) {
			text.text = obj.name;
		} else if (cfg->get_ngc_names_type() == config::ngcname_both) {
			if (!obj.messier_id.empty())
				text.text = obj.messier_id;
			else
				text.text = obj.name;
		}

		text.set_bounding(obj);
		text.height = cfg->get_ngc_names_height();

		if (!text.text.empty())
			store->texts.push_back(text);
	}
};

template <class NGCObject>
struct update_add_name_draw
{
	typedef void		result_type;
	typedef NGCObject&	argument_type;

	result_type operator () (argument_type arg) const
	{
		update_diameter<NGCObject>()(arg);
		if (cfg->get_ngc_names_type() != config::ngcname_none)
			ngc_name<NGCObject>()(arg);
		canvas->draw(arg);
	}
};

void process::ngc()
{
	std::string ngc_filename(cfg->get_ngc_filename());
	if (ngc_filename.empty())
		ngc_filename = "ngc2000.dat";
	std::ifstream ngc_file(ngc_filename.c_str());

	if (!ngc_file.is_open())
		throw "unable to open NGC file: " + cfg->get_ngc_filename() ;

	log_stream << "reading & drawing NGC objects from file <" << ngc_filename << ">\n";

	char line[110];
	std::string obj_type;

	// read the NGC -> Messier cross index (ngcnames.dat)
	if (cfg->get_ngc_names_type() == config::ngcname_both  ||  cfg->get_ngc_names_type() == config::ngcname_messier)
	{
		std::ifstream ngc_names_file("ngcnames.dat");

		if (ngc_names_file.is_open() == false)
			throw "unable to open ngcnames.dat";

		std::string messier_code, ngc_code;
		size_t index;

		while (true)
		{
			ngc_names_file.getline(line, 110);

			if (ngc_names_file.eof()  ||  strlen(line) == 0)
				break;

			if (line[0] == 'M'  &&  line[1] == ' '  &&  strlen(line) == 41)
			{
				messier_code = line;
				messier_code = messier_code.substr(0, 5);
				index = messier_code.find_first_of("0123456789");
				if (index != std::string::npos)
					messier_code = 'M' + messier_code.substr(index);

				ngc_code = line + 36;
				index = ngc_code.find_first_of("0123456789");
				assert(index != std::string::npos);
				ngc_code = ngc_code.substr(index);
				store->ngc2messier[ngc_code] = messier_code;
			}
		}
	}

	while (true)
	{
		ngc_file.getline(line, 110);

		if (ngc_file.eof()  ||  strlen(line) == 0)
			break;

		obj_type = line + 7;
		obj_type = obj_type.substr(0, 2);

		if (obj_type == "Gx"  &&  cfg->draw_galaxies())
        {
            galaxy g;
			new_ngc(g, line);
        } else if (obj_type == "Gb"  &&  cfg->draw_globular_clusters()) {
            globular_cluster gc;
			new_ngc(gc, line);
		} else if (obj_type == "OC"  &&  cfg->draw_open_clusters()) {
            open_cluster oc;
			new_ngc(oc, line);
		} else if (obj_type == "Pl"  &&  cfg->draw_planetary_nebula()) {
            planetary_nebula pn;
			new_ngc(pn, line);
		} else if (obj_type == "Nb"  &&  cfg->draw_nebula()) {
            nebula n;
			new_ngc(n, line);
        }
	}

	// update diameter, add name and draw the object
	std::for_each(store->galaxies.begin(), store->galaxies.end(), update_add_name_draw<galaxy>());
	std::for_each(store->globular_clusters.begin(), store->globular_clusters.end(), update_add_name_draw<globular_cluster>());
	std::for_each(store->open_clusters.begin(), store->open_clusters.end(), update_add_name_draw<open_cluster>());
	std::for_each(store->planetary_nebulas.begin(), store->planetary_nebulas.end(), update_add_name_draw<planetary_nebula>());
	std::for_each(store->nebulas.begin(), store->nebulas.end(), update_add_name_draw<nebula>());
}

void process::print_texts()
{
	log_stream << "printing texts...\n";

	std::vector<text_object>::iterator text_iter = store->texts.begin();
	while (text_iter != store->texts.end())
	{
    	canvas->draw(*text_iter);
		++text_iter;
	}
}

void process::init_text_positions()
{
	stopwatch_t swatch;
	swatch.start();

	log_stream << "initializing text positions...\n";
	log_stream << "total moveable texts on map: " << store->texts.size() << "\n";

	// iterate through all the text and find their valid positions
	size_t cnt = 0;
	while (cnt < store->texts.size())
	{
		text_object& to(store->texts[cnt]);

		if (!to.find_valid_positions())
		{
			log_stream << "<!warning!>Can't find any position for text <" << to.text << "> -- text deleted.\n";

			// delete the text from the vector
			store->texts.erase(store->texts.begin() + cnt);
		} else {
			++cnt;
		}
	}

	log_stream << "init text time taken: " << swatch.get_msec() / 1000.0 << "sec\n";
}

typedef std::pair<std::vector<text_object>::iterator, std::vector<text_object>::iterator> connection_t;

namespace std
{
	template<> struct hash<overlap_t>
	{
		std::size_t operator()(overlap_t const& o) const noexcept
		{
			return (o.txt1 << 48) | (o.pos1 << 32) | (o.txt2 << 16) | o.txt2;
		}
	};
}

#ifdef NDEBUG
#define PRUNING_LIMIT		100000
#else
#define PRUNING_LIMIT		10000
#endif

void process::find_text_positions()
{
	stopwatch_t swatch;
	swatch.start();

	log_stream << "finding text overlap positions...\n";

	std::unordered_set<overlap_t> overlaps_set;
	std::vector<group_t> groups;

	// iterate through every pair of texts on the map, and build hash tables of overlapping texts,
	// and groups of mutually overlapping texts
	for (auto out_iter = store->texts.begin(); out_iter != store->texts.end(); out_iter++)
	{
		for (auto in_iter = out_iter + 1; in_iter != store->texts.end(); in_iter++)
		{
			//log_stream << "? " << out_iter->text << " " << in_iter->text << "\n";
			// iterate through all the positions for these two texts
			bool found = false;
			for (auto& out_vp: out_iter->valid_positions)
			{
				for (auto& in_vp: in_iter->valid_positions)
				{
					if (overlap_text_text(out_vp, in_vp))
					{
						//log_stream << "! ovp " << overlap_t(*out_iter, out_vp, *in_iter, in_vp) << "\n";

						// we don't know in which order the overlap_t is going to be initialized:
						// txt1, txt2   or   txt2, txt1
						// so add for both combinations
						overlaps_set.insert(overlap_t(*out_iter, out_vp, *in_iter, in_vp));
						overlaps_set.insert(overlap_t(*in_iter, in_vp, *out_iter, out_vp));

						out_iter->has_overlaps = in_iter->has_overlaps = true;

						found = true;
					}
				}
			}

			if (found)
			{
				//log_stream << "* current groups:\n";
				//for (const auto& grp : groups)			log_stream << grp;

				int64_t out_group = -1;
				int64_t in_group = -1;
				for (auto& grp : groups)
				{
					//log_stream << "^ checking group:\n" << grp;
					for (const auto& gtxt : grp)
					{
						//log_stream << ": " << *gtxt << "\n";
						
						if (gtxt == out_iter)
						{
							assert(out_group == -1);
							//log_stream << "$ found out " << *gtxt << "\n";
							out_group = &grp - &groups.front();
						}

						if (gtxt == in_iter)
						{
							assert(in_group == -1);
							//log_stream << "$ found in  " << *gtxt << "\n";
							in_group = &grp - &groups.front();
						}
					}
				}

				if (out_group == -1  &&  in_group == -1)
				{
					groups.push_back({ out_iter, in_iter });
					//log_stream << ". created group ID " << groups.size() << "\n" << groups.back();
				}
				else if (out_group != -1 && in_group != -1  &&  in_group != out_group)
				{
					assert(out_group != in_group);

					//log_stream << ". joining group ID " << out_group << "\n" << groups[out_group];
					//log_stream << ". and group ID " << in_group << "\n" << groups[in_group];

					// join groups
					for (const auto& txt_iter : groups[in_group])
						groups[out_group].push_back(txt_iter);

					groups[in_group].clear();

					//log_stream << ". giving\n" << groups[out_group];
				}
				else if (out_group == -1)
				{
					groups[in_group].push_back(out_iter);
					//log_stream << ". added " << *out_iter << " to group ID " << in_group << "\n" << groups[in_group];
				}
				else if (in_group == -1)
				{
					groups[out_group].push_back(in_iter);
					//log_stream << ". added " << *in_iter << " to group ID " << out_group << "\n" << groups[out_group];
				}
			}
		}
	}

	log_stream << "found " << overlaps_set.size() / 2 << " overlaps\n";

	// set the best positions for text that don't overlap any other
	for (auto& txt : store->texts)
	{
		if (!txt.has_overlaps)
		{
			size_t best_pos = 0;
			for (const auto& vp : txt.valid_positions)
			{
				if (txt.valid_positions[best_pos].grade > vp.grade)
					best_pos = &vp - &txt.valid_positions.front();
			}
			if (best_pos > 0)
				txt.valid_positions[0] = txt.valid_positions[best_pos];
		}
	}

	for (const auto& grp : groups)
		log_stream << grp;

	// remove positions which are perfect and don't overlap others in it (TODO)
	/*
	for (auto& grp : groups)
	{
		group_t new_grp;
		for (auto& txt_iter : grp)
		{
			overlap_t o(&*txt_iter - &store->texts.front(), 0,
						0, 0);
			if (txt_iter->valid_positions.front().grade == 0)
			{
				o.
			}
		}
	}
	*/

	log_stream << "********************************************\n";
	log_stream << "********************************************\n";
	log_stream << "********************************************\n";

	for (auto& grp : groups)
	{
		if (grp.empty())
			continue;

		std::vector<std::vector<uint8_t>> combinations;
		std::vector<size_t> grades;

		log_stream << "-----------------------------\n" << grp;

		size_t n = 0;
		for (auto& txt_iter : grp)
		{
			const size_t chk_ndx = &*txt_iter - &store->texts.front();

			log_stream << "text " << n++ << " valids " << txt_iter->valid_positions.size() << '\n';
			if (combinations.empty())
			{
				for (uint8_t pos = 0; pos < txt_iter->valid_positions.size(); pos++)
				{
					combinations.push_back(std::vector<uint8_t>(1, pos));
					grades.push_back(txt_iter->valid_positions[pos].grade);
				}
			}
			else
			{
				std::vector<std::vector<uint8_t>> new_combs;
				std::vector<size_t> new_grades;

				for (auto& cmb : combinations)
				{
					for (uint8_t out_pos = 0; out_pos < txt_iter->valid_positions.size(); out_pos++)
					{
						bool found_overlap = false;
						for (auto& in_pos : cmb)
						{
							const size_t cmb_ndx = &*grp[&in_pos - &cmb.front()] - &store->texts.front();

							overlap_t o(chk_ndx, out_pos,
										cmb_ndx, in_pos);

							if (overlaps_set.find(o) != overlaps_set.end())
							{
								found_overlap = true;
								break;
							}
						}

						if (!found_overlap)
						{
							std::vector<uint8_t> new_cmb(cmb);
							new_cmb.push_back(out_pos);

							new_combs.push_back(new_cmb);
							new_grades.push_back(grades[&cmb - &combinations.front()] + txt_iter->valid_positions[out_pos].grade);
						}
					}
				}

				log_stream << "combinations " << new_combs.size() << "\n";

				// prune?
				if (new_combs.size() > PRUNING_LIMIT)
				{
					// calc grades range
					size_t max = new_grades[0], min = new_grades[0];
					for (size_t g : new_grades)
					{
						if (max < g)	max = g;
						if (min > g)	min = g;
					}

					log_stream << "  - min " << min << " max " << max << "\n";

					// calc grades distribution
					std::vector<size_t> grd_by_val(max + 1, 0);
					for (size_t g : new_grades)
						grd_by_val[g]++;

					assert(new_grades.size() == new_combs.size());

					// find grade which splits the combinations so that after
					// pruning we are left with PRUNING_LIMIT combinations
					size_t partition_grade = 0;
					size_t count_summ = 0;
					for (size_t& c : grd_by_val)
					{
						//log_stream << "  - count_summ " << count_summ << "\n";
						if (count_summ <= PRUNING_LIMIT && count_summ + c > PRUNING_LIMIT)
						{
							partition_grade = &c - &grd_by_val.front();
							break;
						}
						count_summ += c;
					}

					assert(partition_grade >= min);

					log_stream << "  - prunining grade " << partition_grade << "\n";

					// now copy the new combinations and grades but only the ones better than partition_grade
					combinations.clear();
					grades.clear();
					for (size_t& grade : new_grades)
					{
						if (grade < partition_grade)
						{
							combinations.push_back(new_combs[&grade - &new_grades.front()]);
							grades.push_back(grade);
						}
					}

					assert(combinations.size());

					log_stream << "  - pruned down to " << combinations.size() << "\n";
				}
				else
				{
					combinations.swap(new_combs);
					grades.swap(new_grades);
				}
			}
		}

		// output the best combination
		size_t min = grades[0];
		for (size_t g : grades)
			if (min > g)	min = g;

		log_stream << "best grade " << min << "\n";
		auto iter = std::find(grades.begin(), grades.end(), min);
		assert(iter != grades.end());

		decltype (combinations.front()) best_comb(combinations[iter - grades.begin()]);

		for (auto& txt_iter : grp)
		{
			const size_t txt_ndx = &txt_iter - &grp.front();
			if (best_comb[txt_ndx])
				txt_iter->valid_positions.front() = txt_iter->valid_positions[best_comb[txt_ndx]];
		}
	}
}

void process::radiants()
{
	log_stream << "reading & drawing meteor radiants...\n";

	// open the radiants catalog
	std::ifstream radiants_file(cfg->get_radiants_file_name().c_str());

	if (!radiants_file.is_open())
		throw "can't open radiants catalog file '" + cfg->get_radiants_file_name() + "'";

	char radiant_line[120];

	radiant rad;

	// reading
	while (true)
	{
		radiants_file.getline(radiant_line, sizeof(radiant_line));

		if (strlen(radiant_line) == 0)
			break;

		rad.decode(radiant_line);

		if (is_in_map_boundaries(rad.coord))
			store->radiants.push_back(rad);
	}

	// drawing
	text_object text;
	std::vector<radiant>::iterator rad_iter = store->radiants.begin();
	while (rad_iter != store->radiants.end())
	{
		canvas->draw(*rad_iter);

		text.text = rad_iter->name + ", " + rad_iter->max_date;
		text.height = cfg->get_radiants_text_size();
		text.set_bounding(*rad_iter);

		store->texts.push_back(text);

		++rad_iter;
	}
}

// this is a nice general way to read object from files
// might do this for all the other objects in the future

template <class TypeToRead>
std::istream& operator >> (std::istream& in, TypeToRead& out)
{
	char line[1250];
	std::string l;
	size_t ndx;

	do {
		in.getline(line, sizeof(line));

		if (in.eof())
			return in;

		l = line;

		// kill the comments
		ndx = l.find("//");
		if (ndx != std::string::npos)
			l = alltrim(l.substr(0, ndx));
		else
			l = alltrim(l);

	} while (l.empty());

	out = TypeToRead::import(l);

	return in;
}

void process::custom_objects()
{
	std::ifstream in(cfg->custom_objects_file_name().c_str());

	// load the custom object into the storage
	std::copy(	std::istream_iterator<custom_object>(in), std::istream_iterator<custom_object>(),
				std::back_inserter(store->custom_objects));

	text_object txt;
	txt.is_greek = false;	// is the text in greek alphabet (Symbol font)
	txt.height = 1.2;		// the size (height) of the text (descent + upcent)

	// read the object and store the texts
	std::vector<custom_object>::iterator i(store->custom_objects.begin()); 
	while (i != store->custom_objects.end())
	{
		// draw the object
		canvas->draw(*i);

		// add the text
		txt.text = i->name;
		txt.set_bounding(*i);

		store->texts.push_back(txt);

		++i;
	}
}
