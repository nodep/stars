#include "stl.h"
#pragma hdrstop

#include "text.h"
#include "calcs.h"
#include "config.h"
#include "storage.h"
#include "ps_canvas.h"
#include "asterism_line.h"
#include "matrix.h"

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

std::vector<text_object>::iterator text_object::position_selector::overlaps_another() const
{
	return store->get_overlapping_text(*parent);
}

bool text_object::overlaps_alinman(const position_t& on_pos) const
{
	// the asterism lines
	point startPt, endPt, cornerPt;
	double startAngle, endAngle, theta;
	bool found;

	std::vector<asterism_line>::const_iterator linesIter = store->asterism_lines.begin();
	while (linesIter != store->asterism_lines.end())
	{
		found = false;
		if (linesIter->start_star->coord.conv2point() == bound_to)
		{
			found = true;
			cornerPt = linesIter->end_star->coord.conv2point();
		} else if (linesIter->end_star->coord.conv2point() == bound_to) {
			found = true;
			cornerPt = linesIter->start_star->coord.conv2point();
		}

		if (found)
		{
			theta = calc_theta(bound_to, cornerPt);

			switch (on_pos.position_code)
			{
			case position_code_e::right:
				startPt = on_pos.left_down_bound;
				endPt.alpha = on_pos.left_down_bound.alpha;
				endPt.r = on_pos.right_up_bound.r;
				break;
			case position_code_e::left:
				startPt = on_pos.right_up_bound;
				endPt.alpha = on_pos.right_up_bound.alpha;
				endPt.r = on_pos.left_down_bound.r;
				break;
			case position_code_e::up:
				startPt.alpha = on_pos.right_up_bound.alpha;
				startPt.r = on_pos.left_down_bound.r;
				endPt = on_pos.left_down_bound;
				break;
			case position_code_e::down:
				startPt.alpha = on_pos.left_down_bound.alpha;
				startPt.r = on_pos.right_up_bound.r;
				endPt = on_pos.right_up_bound;
				break;
			case position_code_e::right_up:
				endPt.alpha = on_pos.left_down_bound.alpha;
				endPt.r = on_pos.right_up_bound.r;
				startPt.alpha = on_pos.right_up_bound.alpha;
				startPt.r = on_pos.left_down_bound.r;
				break;
			case position_code_e::right_down:
				startPt = on_pos.right_up_bound;
				endPt = on_pos.left_down_bound;
				break;
			case position_code_e::left_up:
				startPt = on_pos.left_down_bound;
				endPt = on_pos.right_up_bound;
				break;
			case position_code_e::left_down:
				startPt.alpha = on_pos.left_down_bound.alpha;
				startPt.r = on_pos.right_up_bound.r;
				endPt.alpha = on_pos.right_up_bound.alpha;
				endPt.r = on_pos.left_down_bound.r;
				break;
			}

			startAngle = calc_theta(bound_to, startPt);
			endAngle = calc_theta(bound_to, endPt);

			if (is_in_angle(startAngle, endAngle, theta))
				return true;
		}

		++linesIter;
	}

	return false;
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
/*
void text_object::set_position_direct(const point& p)
{
	valid_position_t valid_pos;
	valid_pos.position_code = position_code_e::other;
	valid_pos.position = p;
	valid_pos.update_angle(*text, p);
	valid_pos.update_bounds(*text, p);

	valid_positions.push_back(valid_pos);
}
*/
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

/*
void text_object::update_angle(valid_position_t& pos)
{
	// cache the angle
	pos.angle = 0.0;
	for (size_t chrCnt = 0; text[chrCnt] != 0; ++chrCnt)
		pos.angle += char_angle(text[chrCnt], height, pos.position.r - height, is_greek);
}

void text_object::update_bounds(valid_position_t& pos)
{
	pos.left_down_bound = pos.right_up_bound = pos.position;
	if (!has_descent())
		pos.left_down_bound.r -= height * .25;

	pos.right_up_bound.alpha += pos.angle;
	pos.right_up_bound.r -= height;
	if (!has_upcent())
		pos.right_up_bound.r += height * .25;
}
*/

/*
struct cached_overlap_text_text
{
	struct text_overlap_cache_t
	{
		size_t		text1_index;
		size_t		text1_position;
		size_t		text2_index;
		size_t		text2_position;

		bool		result;

		bool operator == (const text_overlap_cache_t& rhs) const
		{
			return text1_index == rhs.text1_index  &&  text2_index == rhs.text2_index
					&&  text1_position == rhs.text1_position  &&  text1_position == rhs.text1_position;
		}

		void build_from_texts(text_object::position_selector& text1, text_object::position_selector& text2)
		{
			text1_index = text1.parent - &store->texts.front();
			text1_position = text1.parent->curr_pos -> text1.parent->valid_positions();
			text2_index = text2.parent - &store->texts.front();
			text2_position = text2.get_current_position_index();

			if (text1_index > text2_index)
			{
				std::swap(text1_index, text2_index);
				std::swap(text1_position, text2_position);
			}
		}

		struct hash_func_t
		{
			typedef const cached_overlap_text_text::text_overlap_cache_t	argument_type;

			// bitwise distribution of the hash value
			// text1   text1 				   text2   text2
			// pos	   index				   pos	   index
			// 3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
			// 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
			size_t operator () (argument_type& arg) const
			{
				return (arg.text1_position << 28) | ((arg.text1_index & 0xfffff) << 16) | (arg.text2_position << 12) | (arg.text2_index & 0xfffff);
			}

			// comparison for stdext::hast_map
			bool operator () (argument_type& lhs, argument_type& rhs) const
			{
				return operator () (lhs) < operator () (rhs);
			}
		};
	};

	typedef bool				result_type;
	typedef const text_object&	first_argument_type;
	typedef const text_object&	second_argument_type;

	typedef std::unordered_set<text_overlap_cache_t, text_overlap_cache_t::hash_func_t> text_overlaps_cache_t;

	static text_overlaps_cache_t text_overlaps_cache;

	result_type operator () (first_argument_type text1, second_argument_type text2)
	{
		// look in the cache
		text_overlap_cache_t srch_element;
		srch_element.build_from_texts(text1, text2);
		text_overlaps_cache_t::iterator srch_result = text_overlaps_cache.find(srch_element);
		if (srch_result != text_overlaps_cache.end())
			return srch_result->result;

		// insert
		srch_element.result = overlap_text_text(text1, text2);
		text_overlaps_cache.insert(srch_element);

		return (bool) srch_element.result;
	}
};

cached_overlap_text_text::text_overlaps_cache_t cached_overlap_text_text::text_overlaps_cache;
*/

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
		{
			continue;	// position invalid - drop it
		}

		// no object overlaps -- set the overlap flags
		vp.overlaps[alinman_overlap] = overlaps_alinman(vp);
		vp.overlaps[meridian_overlap] = overlaps_meridian(vp);
		vp.overlaps[declination_overlap] = overlaps_declination(vp);
		vp.overlaps[out_of_bounds] = vp.right_up_bound.r > cfg->get_map_radius();

		// set the grade value
		int grade = 0;

		// if the text is out of the map boundaries
		if (vp.overlaps[out_of_bounds])
			grade -= 10;

		// circle through the declination circles
		if (vp.overlaps[declination_overlap])
			grade -= 8;

		// alinmans
		if (vp.overlaps[alinman_overlap])
			grade -= 4;

		// the meridians
		if (vp.overlaps[meridian_overlap])
			grade -= 2;

		vp.grade = grade;
		vp.is_perfect = (grade == 0);

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

	sprintf_s(row, sizeof(row), "%c [%-4s]  pos(alpha, r)=(%7.3f, %6.4f)",
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

// ****************************
// finding positions
// ****************************

typedef matrix<text_object::position_selector>		matrix_t;

inline std::string int2str(const int i)
{
	char buffer[20];
	sprintf_s(buffer, sizeof(buffer), "%i", i);
	return buffer;
}

std::string bytes_to_readable(const long long bytes)
{
	std::string ret_val;

	if (bytes > 1024 * 1024 * 1024)
		ret_val += int2str((int) bytes / 1024 / 1024 / 1024) + " GB";
	else if (bytes > 1024 * 1024)
		ret_val += int2str((int) bytes / 1024 / 1024) + " MB";
	else if (bytes > 1024)
		ret_val += int2str((int) bytes / 1024) + " KB";
	else
		ret_val += int2str((int) bytes) + " Bytes";

	return ret_val;
}

// special version for matrices of texts
log_t& operator << (log_t& o, matrix_t& m)
{
	o << "matrix rows=" << m.size_rows() << " cols=" << m.size_columns()
		<< "  size=" << m.size() << "(elem)  " << bytes_to_readable(sizeof(text_object::position_selector) * m.size()) << "\n";

	/*
	for (size_t row_cnt = 0; row_cnt < m.size_rows(); ++row_cnt)
	{
		for (size_t col_cnt = 0; col_cnt < m.size_columns(); ++col_cnt)
		{
			text_object& txt = *m.get(row_cnt, col_cnt);
			o << (txt.is_greek ? "G[" : " [");
			o << txt.text.substr(0, 5) << "] p:";

			switch (txt.curr_pos().position_code)
			{
			case text_object::right:		o << "R ";	break;
			case text_object::left:			o << "L ";	break;
			case text_object::up:			o << "U ";	break;
			case text_object::down:			o << "D ";	break;
			case text_object::right_up:		o << "RU";	break;
			case text_object::right_down:	o << "RD";	break;
			case text_object::left_up:		o << "LU";	break;
			case text_object::left_down:	o << "LD";	break;
			}

			o << (col_cnt == m.size_columns() - 1 ? "\n" : "\t");
		}
	}
	*/

	return o;
};

struct text_in_group_t		// the name should be changed
{
	// the overlaps of this text with all the other texts
	struct overlaps_another_t
	{
		size_t								in_position;		// this text overlaps with ...
		text_object::position_selector		with_position;		// ... a text in this position

		explicit overlaps_another_t(const size_t ip, text_object::position_selector& wp)
			: in_position(ip), with_position(wp)
		{}
	};

	std::vector<text_object>::iterator	txt_iter;		// the iterator to the text
	std::vector<overlaps_another_t>		all_overlaps;

	explicit text_in_group_t(const std::vector<text_object>::iterator& ti)
		: txt_iter(ti)
	{}

	/*
	void add_overlap(text_object::position_selector& overlap_pos_iter)
	{
		all_overlaps.push_back(overlaps_another_t(txt_iter->get_current_position_index(), overlap_pos_iter));
	}
	*/

	// ***************************
	// statistic data for the text

	size_t		total_pos_cnt;			// valid positions
	size_t		overlap_pos_cnt;		// positions with overlaps with other texts
	size_t		free_pos_cnt;			// all the free positions
	size_t		perfect_pos_cnt;		// perfect free positions (no overlaps of any kind)

	std::vector<size_t>		free_pos;		// the free positions of this text
	std::vector<size_t>		perfect_pos;	// the perfect positions of this text

	// *******************************
	// calculates the above statistics
	void calc_stats()
	{
		// set the total number of positions
		total_pos_cnt = txt_iter->valid_positions.size();

		// get indecies of overlap positions
		std::set<size_t> overlap_pos;
		std::vector<overlaps_another_t>::iterator ao_iter = all_overlaps.begin();
		while (ao_iter != all_overlaps.end())
		{
			// insert the overlapping pos index into the set
			overlap_pos.insert(ao_iter->in_position);
			++ao_iter;
		}

		overlap_pos_cnt = overlap_pos.size();
		free_pos_cnt = total_pos_cnt - overlap_pos_cnt;

		// make a vector of all the valid position indexes
		std::vector<size_t> all_pos(total_pos_cnt);
		for (size_t c = 0; c < all_pos.size(); ++c)
			all_pos[c] = c;

		// now get all the free positions
		free_pos.clear();
		free_pos.insert(free_pos.begin(), free_pos_cnt, 0); 
		std::set_difference(all_pos.begin(), all_pos.end(),
							overlap_pos.begin(), overlap_pos.end(),
							free_pos.begin());

		// now count and collect all the perfect free positions
		perfect_pos_cnt = 0;
		std::vector<size_t>::iterator free_pos_iter = free_pos.begin();
		while (free_pos_iter != free_pos.end())
		{
			if (txt_iter->valid_positions[*free_pos_iter].is_perfect)
			{
				perfect_pos.push_back(*free_pos_iter);
				++perfect_pos_cnt;
			}

			++free_pos_iter;
		}
	}

	// *****************
	// sort the group by
	struct process_order_t
	{
		typedef const text_in_group_t		first_argument_type;
		typedef const text_in_group_t		second_argument_type;
		typedef bool						result_type;

		result_type operator () (first_argument_type& lhs, second_argument_type& rhs) const
		{
			if (lhs.perfect_pos_cnt < rhs.perfect_pos_cnt)
				return true;
			if (lhs.perfect_pos_cnt > rhs.perfect_pos_cnt)
				return false;

			if (lhs.free_pos_cnt < rhs.free_pos_cnt)
				return true;
			if (lhs.free_pos_cnt > rhs.free_pos_cnt)
				return false;

			return lhs.overlap_pos_cnt < rhs.overlap_pos_cnt;
		}
	};

	bool has_perfect() const
	{
		return perfect_pos_cnt > 0;
	}

	bool has_free() const
	{
		return free_pos_cnt > 0;
	}
};

void fill_stats(std::vector<text_in_group_t>& group)
{
	// iterate all the pairs of texts in the group
	std::vector<text_in_group_t>::iterator out_text_iter = group.begin();
	std::vector<text_in_group_t>::iterator in_text_iter;
	while (out_text_iter != group.end())
	{
		in_text_iter = out_text_iter + 1;

		while (in_text_iter != group.end())
		{
			// iterate all the posible positions of these two texts
			for (auto& out_vp: out_text_iter->txt_iter->valid_positions)
			{
				for (auto& in_vp : in_text_iter->txt_iter->valid_positions)
				{
					// do they overlap?
					if (overlap_text_text(out_vp, in_vp))
					{
						// add the info about overlaps to the all_overlaps
						// container of the two texts

						assert(false);
						//!!! out_text_iter->add_overlap(in_pos);
						//!!! in_text_iter->add_overlap(out_pos);
					}
				}
			}

			++in_text_iter;
		}

		++out_text_iter;
	}

	// update the stats
	out_text_iter = group.begin();
	while (out_text_iter != group.end())
	{
		out_text_iter->calc_stats();
		++out_text_iter;
	}
}

/*
struct get_position_grate_t
{
	typedef text_object::position_selector		argument_type;
	typedef text_object::position_quality_t		result_type;

	result_type operator () (argument_type& arg) const
	{
		return (*arg).get_pos_grade();
	}
};
*/

std::string get_temp_filename()
{
	char result[L_tmpnam];
	tmpnam_s(result, sizeof(result));
	return result;
}

template <typename T>
void save(const matrix<T>& m, const std::string& fname)
{
	log_stream << "writing to " << fname << " rows: " << m._rows << " cols: " << m._columns << " bytes: " << sizeof(T) * m._data.size() + sizeof(m._rows) * 2 << "\n";

	std::ofstream out(fname, std::ios_base::binary);
	out.write((char*) &m._rows, sizeof(m._rows));
	out.write((char*) &m._columns, sizeof(m._columns));

	out.write((char*) &m._data.front(), sizeof(T) * m._data.size());
}

template <typename T>
void load(matrix<T>& m, const std::string& fname)
{
	m.clear();

	std::ifstream out(fname, std::ios_base::binary);
	out.read((char*) &m._rows, sizeof(m._rows));
	out.read((char*) &m._columns, sizeof(m._columns));

	const size_t size = m._rows * m._columns;

	log_stream << "reading from " << fname << " rows: " << m._rows << " cols: " << m._columns << " bytes: " << sizeof(T) * size + sizeof(m._rows) * 2 << " size==" << size << "\n";

	m._data.clear();
	m._data.insert(m._data.end(), m._rows * m._columns, T());
	out.read((char*)&*m._data.begin(), sizeof(T) * m._data.size());
}

/*
void optimize_text_group(group_t& grp)
{
	// build a matrix of the texts that fall into the criteria
	matrix_t the_matrix;

	// copy all the texts into a vector of text_in_group_t
	std::vector<text_in_group_t> group;
	for (auto& txt : grp)
		group.push_back(text_in_group_t(txt));

	fill_stats(group);

	// order for processing: by perfect then by free then by valid
	std::sort(group.begin(), group.end(), text_in_group_t::process_order_t());
}
*/