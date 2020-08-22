#include "stl.h"
#pragma hdrstop

#include "text.h"
#include "calcs.h"
#include "config.h"
#include "storage.h"
#include "ps_canvas.h"
#include "asterism_line.h"

const double sq_root = sqrt(2.0);
const size_t max_matrix_mem_usage = 1024 * 1024 * 2;

inline double calc_theta(const point& A, const point& B)
{
	double cLength = line_length(A, B);
	double beta = acos((A.r*A.r + cLength*cLength - B.r*B.r) / (2 * A.r * cLength));

	if (is_in_angle(A.alpha, A.alpha + pi, B.alpha))
		return 2 * pi - beta;
	else
		return beta;
}

bool text_object::overlaps_asterism(const position_t& on_pos) const
{
	// the asterism lines
	size_t overlap_cnt = 0;

	for (const auto& al: store->asterism_lines)
	{
		// iterate through points on the line between the the two stars,
		// and calc how many times those points fall into the text
		dekart pt_dek(al.start_star->coord.conv2point().conv2dekart());
		dekart end(al.end_star->coord.conv2point().conv2dekart());
		point p;

		const double llength = line_length(al.start_star->coord.conv2point(), al.end_star->coord.conv2point());
		const double steps = llength / 0.2;
		const double x_step = (end.x - pt_dek.x) / steps;
		const double y_step = (end.y - pt_dek.y) / steps;

		for (size_t s = 0; s < size_t(steps); s++)
		{
			p = pt_dek.conv2polar();

			if (is_in_angle(on_pos.left_down_bound.alpha, on_pos.right_up_bound.alpha, p.alpha)
					&&  on_pos.left_down_bound.r > p.r && on_pos.right_up_bound.r < p.r)
				++overlap_cnt;

			pt_dek.x += x_step;
			pt_dek.y += y_step;
		}
	}

	return overlap_cnt > 0;
}

bool text_object::overlaps_declination(const position_t& pos) const
{
	double dec_step = cfg->get_map_radius() / cfg->get_max_pole_distance() * 10;
	for (double dec = dec_step; dec <= cfg->get_map_radius(); dec += dec_step)
	{
		if (pos.left_down_bound.r - dec > 0  &&  pos.right_up_bound.r - dec < 0)
			return true;
	}

	return false;
}

bool text_object::overlaps_meridian(const position_t& on_pos) const
{
	size_t cnt = 0;
	const double div12limit = 3 / cfg->get_max_pole_distance() * canvas->get_extent();
	const double div2limit = 10 / cfg->get_max_pole_distance() * canvas->get_extent();
	const double restLimit = 30 / cfg->get_max_pole_distance() * canvas->get_extent();
	for (double ra = 0; ra < 2 * pi; ra += pi / 24, ++cnt)
	{
		if (is_in_angle(on_pos.position.alpha, on_pos.position.alpha + on_pos.angle, ra))
		{
			if (cnt % 12 == 0)
			{
				if (on_pos.right_up_bound.r > div12limit)
					return true;
			} else if (cnt % 2 == 0) {
				if (on_pos.right_up_bound.r > div2limit)
					return true;
			} else if (on_pos.right_up_bound.r > restLimit) {
				return true;
			}
		}
	}

	return false;
}

void text_object::init_position(position_t& pos)
{
	double objectR = bound_to.r;
	pos.position = bound_to;	// gets corrected later on

	// the height of the text with descent in mind
	double actual_height;
	if (has_descent())
		actual_height = height;
	else
		actual_height = height * 0.75;

	if (!has_upcent())
		actual_height -= height * 0.25;

	// correct the position
	switch (pos.position_code)
	{
	case position_code_e::right:
	case position_code_e::left:
		pos.position.r += actual_height / 2;
		if (!has_descent())
			pos.position.r += height * .25;

		pos.update_angle(*this);

		if (pos.position_code == position_code_e::right)
			pos.position.alpha += cut_angle(bound_diameter + cfg->get_text_distance(), objectR);
		else
			pos.position.alpha -= cut_angle(bound_diameter + cfg->get_text_distance(), objectR) + pos.angle;

		break;
	case position_code_e::up:
		pos.position.r -= bound_diameter + cfg->get_text_distance();
		if (!has_descent())
			pos.position.r += height * 0.25;

		pos.update_angle(*this);
		
		pos.position.alpha -= pos.angle / 2;

		break;
	case position_code_e::down:
		pos.position.r += bound_diameter + cfg->get_text_distance() + height;
		pos.update_angle(*this);
		pos.position.alpha -= pos.angle / 2;

		break;
	case position_code_e::right_up:
	case position_code_e::left_up:
		pos.position.r -= (bound_diameter + cfg->get_text_distance()) / sq_root;
		if (!has_descent())
			pos.position.r += height * 0.25;

		pos.update_angle(*this);

		if (pos.position_code == position_code_e::right_up)
			pos.position.alpha += cut_angle((bound_diameter + cfg->get_text_distance()) / sq_root, objectR);
		else
			pos.position.alpha -= cut_angle((bound_diameter + cfg->get_text_distance()) / sq_root, objectR) + pos.angle;

		break;
	case position_code_e::right_down:
	case position_code_e::left_down:
		pos.position.r += (bound_diameter + cfg->get_text_distance()) / sq_root + height;

		pos.update_angle(*this);

		if (pos.position_code == position_code_e::right_down)
			pos.position.alpha += cut_angle((bound_diameter + cfg->get_text_distance()) / sq_root, objectR);
		else
			pos.position.alpha -= cut_angle((bound_diameter + cfg->get_text_distance()) / sq_root, objectR) + pos.angle;

		break;
	default:
		assert(false);
	}

	pos.update_bounds(*this);
}

void text_object::connect() const
{
	// draw a small frame for the text, and connect it
	// with the object the text is bound to

	point start, end;
	point textLD = curr_pos().left_down_bound;
	point textRU = curr_pos().right_up_bound;

	// move the bounds a bit
	textLD.r += 0.1;
	textLD.alpha -= cut_angle(0.1, textLD.r);
	textRU.r -= 0.1;
	textRU.alpha += cut_angle(0.1, textRU.r);

	// the height of the text with descent in mind
	double actual_height;
	if (has_descent())
		actual_height = height;
	else
		actual_height = height * 0.75;

	canvas->set_pen_width(0.1);

	switch (curr_pos().position_code)
	{
	case position_code_e::right:
		end.r = textRU.r;
		end.alpha = textLD.alpha;
		canvas->line(textLD, end);

		start.r = (textLD.r + textRU.r) / 2;
		start.alpha = textLD.alpha;
		break;
	case position_code_e::left:
		end.r = textLD.r;
		end.alpha = textRU.alpha;
		canvas->line(textRU, end);

		start.r = (textLD.r + textRU.r) / 2;
		start.alpha = textRU.alpha;

		break;
	case position_code_e::up:
		end = start = textLD;
		start.r = end.r = textLD.r - actual_height * 0.05;
		start.alpha += curr_pos().angle / 3;
		end.alpha += curr_pos().angle * 0.67;
		canvas->line(start, end);

		start.alpha = textLD.alpha + curr_pos().angle / 2;

		break;
	case position_code_e::down:
		end = start = textLD;
		start.r = end.r = textRU.r + actual_height * 0.1;
		start.alpha += curr_pos().angle / 3;
		end.alpha += curr_pos().angle * 0.67;
		canvas->line(start, end);

		start.alpha = textLD.alpha + curr_pos().angle / 2;

		break;
	case position_code_e::right_up:
		start = textLD;
		end.r = (textRU.r + textLD.r) / 2;
		end.alpha = textLD.alpha;
		canvas->line(start, end);
		end.r = textLD.r;
		end.alpha = textLD.alpha - cut_angle((textRU.r - textLD.r) / 2, textLD.r);
		canvas->line(start, end);

		break;
	case position_code_e::right_down:
		start.r = textRU.r;
		start.alpha = textLD.alpha;
		end.r = (textRU.r + textLD.r) / 2;
		end.alpha = textLD.alpha;
		canvas->line(start, end);
		end.r = textRU.r;
		end.alpha = textLD.alpha - cut_angle((textRU.r - textLD.r) / 2, textLD.r);
		canvas->line(start, end);

		break;
	case position_code_e::left_up:
		start.r = textLD.r;
		start.alpha = textRU.alpha;
		end.r = (textRU.r + textLD.r) / 2;
		end.alpha = textRU.alpha;
		canvas->line(start, end);
		end.r = textLD.r;
		end.alpha = textRU.alpha + cut_angle((textRU.r - textLD.r) / 2, textLD.r);
		canvas->line(start, end);

		break;
	case position_code_e::left_down:
		start = textRU;
		end.r = (textRU.r + textLD.r) / 2;
		end.alpha = textRU.alpha;
		canvas->line(start, end);
		end.r = textRU.r;
		end.alpha = textRU.alpha + cut_angle((textRU.r - textLD.r) / 2, textLD.r);
		canvas->line(start, end);

		break;
	default:		// other
		throw "Internal error -- text_object::connect() with other";
		break;
	}

	canvas->line(start, curr_pos().position);
}

void text_object::print() const
{
	canvas->draw(*this);
}

void text_object::position_t::update_angle(text_object& text)
{
	// cache the angle
	angle = 0.0;
	for (size_t chrCnt = 0; text.text[chrCnt] != 0; ++chrCnt)
		angle += char_angle(text.text[chrCnt], text.height, position.r - text.height, text.is_greek);
}

void text_object::position_t::update_bounds(text_object& text)
{
	left_down_bound = right_up_bound = position;
	if (!text.has_descent())
		left_down_bound.r -= text.height * .25;

	right_up_bound.alpha += angle;
	right_up_bound.r -= text.height;
	if (!text.has_upcent())
		right_up_bound.r += text.height * .25;
}

overlap_t::overlap_t(const text_object& t1, const text_object::position_t& p1, const text_object& t2, const text_object::position_t& p2)
:	txt1(&t1 - &store->texts.front()),
	pos1(&p1 - &t1.valid_positions.front()),
	txt2(&t2 - &store->texts.front()),
	pos2(&p2 - &t2.valid_positions.front())
{
	assert(pos1 < 8 && pos2 < 8 && txt1 < 10000 && txt2 < 10000);
}

overlap_t::overlap_t(const size_t t1, const size_t p1, const size_t t2, const size_t p2)
:	txt1(t1),
	pos1(p1),
	txt2(t2),
	pos2(p2)
{
	assert(pos1 < store->texts[txt1].valid_positions.size()
		&& pos2 < store->texts[txt2].valid_positions.size()
		&& txt1 < store->texts.size()
		&& txt2 < store->texts.size());
}

bool text_object::find_valid_positions()
{
	// clear the default position
	valid_positions.clear();

	position_t vp;

	for (size_t curr_pos_code = (size_t) position_code_e::right; curr_pos_code < (size_t) position_code_e::other; ++curr_pos_code)
	{
		// set the position code
		vp.position_code = (position_code_e)curr_pos_code;

		// set the boundings & angles
		init_position(vp);

		// are we overlapping any object?
		if (store->text_overlaps_object(*this, vp))
			continue;	// position invalid - drop it

		// no object overlaps -- set the overlap flags
		vp.overlaps[asterism_overlap] = overlaps_asterism(vp);
		vp.overlaps[meridian_overlap] = overlaps_meridian(vp);
		vp.overlaps[declination_overlap] = overlaps_declination(vp);
		vp.overlaps[out_of_bounds] = vp.right_up_bound.r > cfg->get_map_radius();

		// set the grade value
		size_t grade = 0;

		// if the text is out of the map boundaries
		if (vp.overlaps[out_of_bounds])
			grade += 20;

		// circle through the declination circles
		if (vp.overlaps[declination_overlap])
			grade += 18;

		// asterisms
		if (vp.overlaps[asterism_overlap])
			grade += 14;

		// the meridians
		if (vp.overlaps[meridian_overlap])
			grade += 12;

		grade += (size_t)curr_pos_code;

		vp.grade = grade;

		valid_positions.push_back(vp);
	}

	return !valid_positions.empty();
}

log_t& operator << (log_t& o, const point& t)
{
	o << t.r << "  " << t.alpha;
	return o;
}

log_t& operator << (log_t& o, const text_object& t)
{
	static char row[1000];

	sprintf_s(row, sizeof(row), "%5i %c [%-4s]  pos(alpha, r)=(%7.3f, %6.4f)",
						int(&t - &store->texts.front()),
						t.is_greek ? 'G' : ' ',
						t.text.c_str(),
						t.bound_to.alpha, t.bound_to.r);

	o << row;

	return o;
}

log_t& operator << (log_t& o, const group_t& grp)
{
	o << "------ group with " << grp.size() << " texts\n";
	group_t::const_iterator i = grp.begin();
	while (i != grp.end())
	{
		o << **i << "\n";
		++i;
	}

	return o;
}

const char* pos_name(const text_object::position_code_e pos_code)
{
	switch (pos_code)
	{
	case text_object::position_code_e::right:			return "right  ";
	case text_object::position_code_e::left:			return "left   ";
	case text_object::position_code_e::up:				return "up     ";
	case text_object::position_code_e::down:			return "down   ";
	case text_object::position_code_e::right_up:		return "r_up   ";
	case text_object::position_code_e::right_down:		return "r_down ";
	case text_object::position_code_e::left_up:			return "l_up   ";
	case text_object::position_code_e::left_down:		return "l_down ";
	case text_object::position_code_e::other:			return "other  ";
	case text_object::position_code_e::invalid:			return "invalid";
	}

	return "???";
}

log_t& operator << (log_t& o, const overlap_t& ovp)
{
	text_object& t1(store->texts[ovp.txt1]);
	text_object& t2(store->texts[ovp.txt2]);

	static char row[1000];

	sprintf_s(row, sizeof(row), "%s->%s %s->%s",
								t1.text.c_str(), pos_name(t1.valid_positions[ovp.pos1].position_code),
								t2.text.c_str(), pos_name(t2.valid_positions[ovp.pos2].position_code));

	o << row;

	return o;
}
