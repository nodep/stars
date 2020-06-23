#include "stl.h"
#pragma hdrstop

#include "text.h"
#include "calcs.h"
#include "config.h"
#include "storage.h"
#include "ps_canvas.h"
#include "asterism_line.h"
#include "cout_log.h"

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

std::vector<text_object>::iterator text_object::position_iterator::overlaps_another() const
{
	return store->get_overlapping_text(*parent);
}

bool text_object::overlaps_alinman(const cached_position* on_pos) const
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

			switch (on_pos->position_code)
			{
			case position_code_e::right:
				startPt = on_pos->left_down_bound;
				endPt.alpha = on_pos->left_down_bound.alpha;
				endPt.r = on_pos->right_up_bound.r;
				break;
			case position_code_e::left:
				startPt = on_pos->right_up_bound;
				endPt.alpha = on_pos->right_up_bound.alpha;
				endPt.r = on_pos->left_down_bound.r;
				break;
			case position_code_e::up:
				startPt.alpha = on_pos->right_up_bound.alpha;
				startPt.r = on_pos->left_down_bound.r;
				endPt = on_pos->left_down_bound;
				break;
			case position_code_e::down:
				startPt.alpha = on_pos->left_down_bound.alpha;
				startPt.r = on_pos->right_up_bound.r;
				endPt = on_pos->right_up_bound;
				break;
			case position_code_e::right_up:
				endPt.alpha = on_pos->left_down_bound.alpha;
				endPt.r = on_pos->right_up_bound.r;
				startPt.alpha = on_pos->right_up_bound.alpha;
				startPt.r = on_pos->left_down_bound.r;
				break;
			case position_code_e::right_down:
				startPt = on_pos->right_up_bound;
				endPt = on_pos->left_down_bound;
				break;
			case position_code_e::left_up:
				startPt = on_pos->left_down_bound;
				endPt = on_pos->right_up_bound;
				break;
			case position_code_e::left_down:
				startPt.alpha = on_pos->left_down_bound.alpha;
				startPt.r = on_pos->right_up_bound.r;
				endPt.alpha = on_pos->right_up_bound.alpha;
				endPt.r = on_pos->left_down_bound.r;
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

bool text_object::overlaps_declination(const cached_position* on_pos) const
{
	double dec_step = cfg->get_map_radius() / cfg->get_max_pole_distance() * 10;
	for (double dec = dec_step; dec <= cfg->get_map_radius(); dec += dec_step)
	{
		if (on_pos->left_down_bound.r - dec > 0  &&  on_pos->right_up_bound.r - dec < 0)
			return true;
	}

	return false;
}

bool text_object::overlaps_meridian(const cached_position* on_pos) const
{
	size_t cnt = 0;
	const double div12limit = 3 / cfg->get_max_pole_distance() * canvas->get_extent();
	const double div2limit = 10 / cfg->get_max_pole_distance() * canvas->get_extent();
	const double restLimit = 30 / cfg->get_max_pole_distance() * canvas->get_extent();
	for (double ra = 0; ra < 2 * pi; ra += pi / 24, ++cnt)
	{
		if (is_in_angle(on_pos->position.alpha, on_pos->position.alpha + on_pos->angle, ra))
		{
			if (cnt % 12 == 0)
			{
				if (on_pos->right_up_bound.r > div12limit)
					return true;
			} else if (cnt % 2 == 0) {
				if (on_pos->right_up_bound.r > div2limit)
					return true;
			} else if (on_pos->right_up_bound.r > restLimit) {
				return true;
			}
		}
	}

	return false;
}

void text_object::init_position(cached_position* p_position)
{
	double objectR = bound_to.r;
	p_position->position = bound_to;	// gets corrected later on

	// the height of the text with descent in mind
	double actual_height;
	if (has_descent())
		actual_height = height;
	else
		actual_height = height * 0.75;

	if (!has_upcent())
		actual_height -= height * 0.25;

	// correct the position
	switch (p_position->position_code)
	{
	case position_code_e::right:
	case position_code_e::left:
		p_position->position.r += actual_height / 2;
		if (!has_descent())
			p_position->position.r += height * .25;

		update_angle(p_position);

		if (p_position->position_code == position_code_e::right)
			p_position->position.alpha += cut_angle(bound_diameter + cfg->get_text_distance(), objectR);
		else
			p_position->position.alpha -= cut_angle(bound_diameter + cfg->get_text_distance(), objectR) + p_position->angle;

		break;
	case position_code_e::up:
		p_position->position.r -= bound_diameter + cfg->get_text_distance();
		if (!has_descent())
			p_position->position.r += height * 0.25;

		update_angle(p_position);
		
		p_position->position.alpha -= p_position->angle / 2;

		break;
	case position_code_e::down:
		p_position->position.r += bound_diameter + cfg->get_text_distance() + height;
		update_angle(p_position);
		p_position->position.alpha -= p_position->angle / 2;

		break;
	case position_code_e::right_up:
	case position_code_e::left_up:
		p_position->position.r -= (bound_diameter + cfg->get_text_distance()) / sq_root;
		if (!has_descent())
			p_position->position.r += height * 0.25;

		update_angle(p_position);

		if (p_position->position_code == position_code_e::right_up)
			p_position->position.alpha += cut_angle((bound_diameter + cfg->get_text_distance()) / sq_root, objectR);
		else
			p_position->position.alpha -= cut_angle((bound_diameter + cfg->get_text_distance()) / sq_root, objectR) + p_position->angle;

		break;
	case position_code_e::right_down:
	case position_code_e::left_down:
		p_position->position.r += (bound_diameter + cfg->get_text_distance()) / sq_root + height;

		update_angle(p_position);

		if (p_position->position_code == position_code_e::right_down)
			p_position->position.alpha += cut_angle((bound_diameter + cfg->get_text_distance()) / sq_root, objectR);
		else
			p_position->position.alpha -= cut_angle((bound_diameter + cfg->get_text_distance()) / sq_root, objectR) + p_position->angle;

		break;
	default:
		assert(false);
	}

	update_bounds(p_position);
}

void text_object::set_position_direct(const point& p)
{
	_current_pos = _positions_cache;
	_current_pos->position_code = position_code_e::other;
	_current_pos->position = p;
	_end_cache = _current_pos + 1;
	update_angle(_current_pos);
	update_bounds(_current_pos);
}

void text_object::connect() const
{
	// draw a small frame for the text, and connect it
	// with the object the text is bound to

	point start, end;
	point textLD = _current_pos->left_down_bound;
	point textRU = _current_pos->right_up_bound;

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

	switch (_current_pos->position_code)
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
		start.alpha += _current_pos->angle / 3;
		end.alpha += _current_pos->angle * 0.67;
		canvas->line(start, end);

		start.alpha = textLD.alpha + _current_pos->angle / 2;

		break;
	case position_code_e::down:
		end = start = textLD;
		start.r = end.r = textRU.r + actual_height * 0.1;
		start.alpha += _current_pos->angle / 3;
		end.alpha += _current_pos->angle * 0.67;
		canvas->line(start, end);

		start.alpha = textLD.alpha + _current_pos->angle / 2;

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

	canvas->line(start, _current_pos->position);
}

void text_object::print() const
{
	canvas->draw(*this);
}

void text_object::update_angle(cached_position* on_pos)
{
	// cache the angle
	on_pos->angle = 0.0;
	for (size_t chrCnt = 0; text[chrCnt] != 0; ++chrCnt)
		on_pos->angle += char_angle(text[chrCnt], height, on_pos->position.r - height, is_greek);
}

void text_object::update_bounds(cached_position* on_pos)
{
	on_pos->left_down_bound = on_pos->right_up_bound = on_pos->position;
	if (!has_descent())
		on_pos->left_down_bound.r -= height * .25;

	on_pos->right_up_bound.alpha += on_pos->angle;
	on_pos->right_up_bound.r -= height;
	if (!has_upcent())
		on_pos->right_up_bound.r += height * .25;
}

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

		void build_from_texts(const text_object& text1, const text_object& text2)
		{
			text1_index = &text1 - &store->texts.front();
			text1_position = text1.get_current_position_index();
			text2_index = &text2 - &store->texts.front();
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

std::string _report_hash()
{
	/*
	char message[100];

	sprintf(message, "hash report: buckets=%i  size=%i  size/bucket=%f",
						cached_overlap_text_text::text_overlaps_cache.bucket_count(),
						cached_overlap_text_text::text_overlaps_cache.size(), 
						(double) cached_overlap_text_text::text_overlaps_cache.size() / cached_overlap_text_text::text_overlaps_cache.bucket_count());
	*/

	return "todo !!!";
}

bool text_object::init_valid_positions()
{
	// cache all the valid positions for this text
	_current_pos = _positions_cache;
	for (size_t curr_pos_code = (size_t) position_code_e::right; curr_pos_code < (size_t) position_code_e::other; ++curr_pos_code)
	{
		// set the position code
		_current_pos->position_code = (position_code_e)curr_pos_code;

		// set the boundings & angles
		init_position(_current_pos);

		// are we overlapping any object?
		if (store->text_overlaps_object(*this))
			continue;	// position invalid - skip it

		// no object overlaps -- set the overlap flags
		_current_pos->overlaps[alinman_overlap] = overlaps_alinman(_current_pos);
		_current_pos->overlaps[meridian_overlap] = overlaps_meridian(_current_pos);
		_current_pos->overlaps[declination_overlap] = overlaps_declination(_current_pos);
		_current_pos->overlaps[out_of_bounds] = _current_pos->right_up_bound.r > cfg->get_map_radius();

		// set the grade value
		int grade = 0;

		// if the text is out of the map boundaries
		if (_current_pos->overlaps[out_of_bounds])
			grade -= 10;

		// circle through the declination circles
		if (_current_pos->overlaps[declination_overlap])
			grade -= 8;

		// alinmans
		if (_current_pos->overlaps[alinman_overlap])
			grade -= 4;

		// the meridians
		if (_current_pos->overlaps[meridian_overlap])
			grade -= 2;

		_current_pos->grade = grade;
		_current_pos->is_perfect = (grade == 0);

		++_current_pos;		// use the next cache slot
	}

	_end_cache = _current_pos;
	_current_pos = _positions_cache;

	return _end_cache != _positions_cache;
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
	group_t::texts_container_t::const_iterator i = grp.texts.begin();
	while (i != grp.texts.end())
	{
		o << **i << "\n";
		++i;
	}

	return o;
}

// ****************************
// finding positions
// ****************************

typedef matrix<text_object::position_iterator>		matrix_t;

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
		<< "  size=" << m.size() << "(elem)  " << bytes_to_readable(sizeof(text_object::position_iterator) * m.size()) << "\n";

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
	std::vector<text_object>::iterator	txt_iter;		// the iterator to the text

	// ******************************************************
	// all the overlaps of this text with all the other texts

	struct overlaps_another_t
	{
		size_t								in_position;		// this text overlaps with ...
		text_object::position_iterator		with_position;		// ... a text in this position
	};

	std::vector<overlaps_another_t>			all_overlaps;

	void add_overlap(text_object::position_iterator& overlap_pos_iter)
	{
		overlaps_another_t o;
		o.in_position = txt_iter->get_current_position_index();
		o.with_position = overlap_pos_iter;

		all_overlaps.push_back(o);
	}

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
		total_pos_cnt = txt_iter->get_valid_pos_num();

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
			txt_iter->set_position_index(*free_pos_iter);

			if (txt_iter->is_position_perfect())
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
			text_object::position_iterator out_pos = out_text_iter->txt_iter->begin();
			text_object::position_iterator in_pos;
			while (out_pos != out_text_iter->txt_iter->end())
			{
				in_pos = in_text_iter->txt_iter->begin();
				while (in_pos != in_text_iter->txt_iter->end())
				{
					// do they overlap?
					if (overlap_text_text(*out_pos, *in_pos))
					{
						// add the info about overlaps to the all_overlaps
						// container of the two texts
						
						out_text_iter->add_overlap(in_pos);
						in_text_iter->add_overlap(out_pos);
					}

					++in_pos;
				}

				++out_pos;
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

struct get_position_grate_t
{
	typedef text_object::position_iterator		argument_type;
	typedef text_object::position_quality_t		result_type;

	result_type operator () (argument_type& arg) const
	{
		return (*arg).get_pos_grade();
	}
};

struct set_position_t
{
	typedef text_object::position_iterator		argument_type;
	typedef void								result_type;

	result_type operator () (argument_type& arg) const
	{
		*arg;
	}
};

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

void text_group_processor_t::operator () (group_t& grp)
{
	static size_t grpcnt = 0;
	grpcnt++;

	log_stream << "processing group " << grpcnt << " started\n";

	// copy all the texts into a vector of text_in_group_t
	std::vector<text_in_group_t> group;
	text_in_group_t txt;
	group_t::texts_container_t::iterator grp_txt_iter = grp.texts.begin();
	while (grp_txt_iter != grp.texts.end())
	{
		txt.txt_iter = *grp_txt_iter;
		group.push_back(txt);
		++grp_txt_iter;
	}

	// collect all the overlaps for the group
	fill_stats(group);

	// order for processing: by perfect then by free then by valid
	std::sort(group.begin(), group.end(), text_in_group_t::process_order_t());

	// !!! debug output begin
	// count the number of texts that have free and perfect positions
	const size_t total_non_free_cnt = std::count_if(group.begin(), group.end(), [](const auto& txt) { return !txt.has_free(); });
	const size_t total_non_perfect_cnt = std::count_if(group.begin(), group.end(), [](const auto& txt) { return !txt.has_perfect(); });

	log_stream << "*************************************************************\n";
	std::vector<text_in_group_t>::iterator txt_iter = group.begin();
	while (txt_iter != group.end())
	{
		log_stream << *txt_iter->txt_iter << "\t";
		log_stream << "total/overlap/free/perfect: " << txt_iter->total_pos_cnt << " " << txt_iter->overlap_pos_cnt << " " << txt_iter->free_pos_cnt << " " << txt_iter->perfect_pos_cnt << "\n";

		++txt_iter;
	}
	log_stream << "------------------------------------------------------------\n";
	log_stream << "group size: " << group.size() << "\n";
	log_stream << "group size after reduction (non free/non perfect): " << total_non_free_cnt << " " << total_non_perfect_cnt << "\n";
	// !!! debug output end

	// build a matrix of the texts that fall into the criteria
	matrix_t the_matrix;

	// the temp files used to store the combination data
	std::vector<std::string> temp_fnames;

	// iterate the texts
	std::vector<text_in_group_t>::iterator grp_iter = group.begin();
	while (grp_iter != group.end())
	{
		// break with the first text with perfect positions
		if (grp_iter->has_perfect())
			break;

		text_object& curr_text = *grp_iter->txt_iter;

		log_stream << curr_text << "\n";

		// collect all the valid positions of this text
		std::vector<text_object::position_iterator> text_valid_positions;
		text_object::position_iterator txt_pos_iter = curr_text.begin();
		while (txt_pos_iter != curr_text.end())
		{
			text_valid_positions.push_back(txt_pos_iter);
			++txt_pos_iter;
		}

		size_t predicted_dest_size;
		if (the_matrix.empty())
		   predicted_dest_size = sizeof(matrix_t::iterator) * text_valid_positions.size();
		else
		   predicted_dest_size = (the_matrix.size_columns() + 1) * the_matrix.size_rows() * text_valid_positions.size() * sizeof(matrix_t::iterator);

		// check if the matrix data is large enough to be swaped to HDD
		if (predicted_dest_size > max_matrix_mem_usage)
		// if ((the_matrix.size() * sizeof(text_object::position_iterator)) > max_matrix_mem_usage)
		{
			// save the matrix to HDD and clear the memory
			const std::string tmp_filename(get_temp_filename());
			temp_fnames.push_back(tmp_filename);
			save(the_matrix, tmp_filename);

			the_matrix.clear();
		}

		// the first text in the matrix?
		if (the_matrix.empty())
		{
			the_matrix.set_columns(1);

			// add all the valid positions for this new text
			std::vector<text_object::position_iterator>::iterator new_text_pos_iter = text_valid_positions.begin();
			while (new_text_pos_iter != text_valid_positions.end())
			{
				the_matrix.push_row(new_text_pos_iter, new_text_pos_iter + 1);
				++new_text_pos_iter;
			}

		} else {

			// the new matrix to copy the data to
			matrix_t dest_matrix;
			dest_matrix.set_columns(the_matrix.size_columns() + 1);

			// iterate the rows of the source matrix
			for (matrix_t::size_type row_num = 0; row_num < the_matrix.size_rows(); ++row_num)
			{
				// get a row from the source matrix
				std::pair<matrix_t::iterator, matrix_t::iterator> row_range = the_matrix.get_row(row_num);

				// iterate the valid positions of the new text
				std::vector<text_object::position_iterator>::iterator new_text_iter = text_valid_positions.begin();
				while (new_text_iter != text_valid_positions.end())
				{
					// find if anything overlaps
					matrix_t::iterator first = row_range.first;
					matrix_t::iterator last = row_range.second;

					while (first != last)
					{
						if (overlap_text_text(**first, **new_text_iter))
							break;

						++first;
					}

					// no overlaps?
					if (first == last)
					{
						// add the new valid position group to the dest_matrix

						// copy the source position
						dest_matrix.push_row(row_range.first, row_range.second);

						// add the new text in its current position
						dest_matrix.get(dest_matrix.size_rows() - 1, dest_matrix.size_columns() - 1) = *new_text_iter;
					}

					++new_text_iter;
				}

				if (dest_matrix.empty())
				{
					throw "internal error: dest_matrix is empty";
				}
			}

			// swap the matrices
			std::swap(the_matrix, dest_matrix);
		}

		size_t actual_dest_size = the_matrix.size() * sizeof(matrix_t::iterator);
		//log_stream << "predicted_dest_size=" << bytes_to_readable(predicted_dest_size) << '\t' << "actual_dest_size=" << bytes_to_readable(actual_dest_size) << '\n';
		//log_stream << the_matrix;

		++grp_iter;
	}

	// save the last matrix into a temp file
	if (!the_matrix.empty())
	{
		const std::string tmp_filename(get_temp_filename());
		temp_fnames.push_back(tmp_filename);
		save(the_matrix, tmp_filename);
		the_matrix.clear();
	}

	log_stream << "matrices created\n";

	// position the remaining texts into perfect positions
	while (grp_iter != group.end())
	{
		grp_iter->txt_iter->set_position_index(grp_iter->perfect_pos.front());
		++grp_iter;
	}

	// iterate the saved matrices to find position combinations
	if (!temp_fnames.empty())
	{
		// create all the matrix combinations 
		std::string result_temp_filename = get_temp_filename();
		std::ifstream source_file;
		std::ofstream result_file;
		std::string source_file_name = temp_fnames.front();
		std::vector<std::string>::iterator tfn_iter = temp_fnames.begin() + 1;
		size_t result_row_cnt;
		size_t result_col_cnt;

		bool is_final_check = false, is_first_valid_combination = true;
		std::vector<text_object::position_iterator> best_positions;
		std::vector<text_object::position_quality_t> curr_position_grades;
		size_t best_pos_grade = 0;

		while (tfn_iter != temp_fnames.end())
		{
			// create the result file
			result_file.open(result_temp_filename, std::ios_base::out || std::ios_base::trunc);

			// open the source file
			source_file.open(source_file_name);

			// load the source matrix
			load(the_matrix, *tfn_iter);

			// load the dimension of the source matrix
			size_t src_rows, src_cols;
			source_file.read((char*) &src_rows, sizeof(src_rows));
			source_file.read((char*) &src_cols, sizeof(src_cols));

			log_stream << "reading source file: " << source_file_name << " rows: " << src_rows << " cols: " << src_cols << "\n";

			// save 4 bytes for the row number that we'll rewrite later
			result_col_cnt = src_cols + the_matrix.size_columns();
			result_file.write((char*) &result_col_cnt, sizeof(result_col_cnt));
			// save 4 more bytes for the number of columns in the result file
			result_file.write((char*) &result_col_cnt, sizeof(result_col_cnt));

			// should we activate the final check
			if (temp_fnames.end() - tfn_iter == 1)
			{
				is_final_check = true;
				best_positions.insert(best_positions.end(), result_col_cnt, text_object::position_iterator());
				curr_position_grades.insert(curr_position_grades.end(), result_col_cnt, text_object::position_quality_t());
			}

			// load the rest from the source matrix in a loop and write the new combinations into result_file
			result_row_cnt = 0;
			std::vector<text_object::position_iterator> src_group(src_cols, text_object::position_iterator());

			for (size_t file_row_cnt = 0; file_row_cnt < src_rows; ++file_row_cnt)
			{
				// load the group from the source file
				const size_t bytesToRead = src_cols * sizeof(text_object::position_iterator);
				source_file.read((char*) &src_group.front(), bytesToRead);

				log_stream << "file row: " << src_rows - file_row_cnt << "    \r";

				if (bytesToRead != source_file.gcount())
				{
					throw "to read: " + std::to_string(bytesToRead) + " read: " + std::to_string(source_file.gcount()) + "\n";
				}

				const std::pair<std::vector<text_object::position_iterator>::iterator, std::vector<text_object::position_iterator>::iterator>
					file_row(src_group.begin(), src_group.end());

				std::pair<std::vector<text_object::position_iterator>::iterator, std::vector<text_object::position_iterator>::iterator> file_row_iter = file_row;

				for (size_t matrix_row_cnt = 0; matrix_row_cnt < the_matrix.size_rows(); ++matrix_row_cnt)
				{
					const std::pair<matrix_t::iterator, matrix_t::iterator> matrix_row(the_matrix.get_row(matrix_row_cnt));
					std::pair<matrix_t::iterator, matrix_t::iterator> matrix_row_iter(matrix_row);

					// iterate through the source matrix
					while (matrix_row_iter.first != matrix_row_iter.second)
					{
						// reset the group range
						file_row_iter.first = file_row.first;

						// iterate the group we read from source_file
						while (file_row_iter.first != file_row_iter.second)
						{
							if (overlap_text_text(**matrix_row_iter.first, **file_row_iter.first))
								break;

							++file_row_iter.first;
						}

						// overlaps found?
						if (file_row_iter.first != file_row_iter.second)
							break;

						++matrix_row_iter.first;
					}

					// no overlaps found?
					if (matrix_row_iter.first == matrix_row_iter.second)
					{
						if (is_final_check)
						{
							// get the grade of the combination from the part we have in the_matrix
							std::transform(matrix_row.first, matrix_row.second, curr_position_grades.begin(), get_position_grate_t());

							std::transform(src_group.begin(), src_group.end(), curr_position_grades.begin() + (matrix_row.second - matrix_row.first), get_position_grate_t());

							const size_t curr_pos_grade = std::accumulate(curr_position_grades.begin(), curr_position_grades.end(), 0);

							if (curr_pos_grade > best_pos_grade  ||  is_first_valid_combination)
							{
								best_pos_grade = curr_pos_grade;

								// remember this combination
								std::copy(src_group.begin(), src_group.end(), best_positions.begin());
								std::copy(matrix_row.first, matrix_row.second, best_positions.begin() + best_positions.size());

								is_first_valid_combination = false;
							}
						}
						else {
							// save the combination
							std::vector<text_object::position_quality_t> curr_position_grades(result_col_cnt);

							// the part from the source matrix
							result_file.write((const char*) &*matrix_row.first, the_matrix.size_columns() * sizeof(text_object::position_iterator));

							// the part from the source file
							result_file.write((const char*) &*src_group.begin(), src_group.size() * sizeof(text_object::position_iterator));
						}

						// inc the number of rows in the result file
						++result_row_cnt;
					}
				}
			}

			// write the number of rows into the result
			result_file.seekp(0);
			result_file.write((char*) &result_row_cnt, sizeof(result_row_cnt));

			source_file.close();
			result_file.close();

			// set the new source/result filenames
			result_temp_filename = get_temp_filename();
			source_file_name = result_temp_filename;

			++tfn_iter;
		}

		// are the combinations still in memory?
		if (temp_fnames.size() < 2)
		{
			// find the position in the matrix with the best grade
			matrix_t::iterator best_position_iter = 0;
			int best_pos_grade = 0;
			std::vector<text_object::position_quality_t> curr_position_grades(the_matrix.size_columns());
			matrix_t::iterator matrix_iter = the_matrix.begin();
			for (size_t rows_cnt = 0; rows_cnt < the_matrix.size_rows(); ++rows_cnt)
			{
				// get the grade of the combination
				std::transform(	matrix_iter, matrix_iter + the_matrix.size_columns(),
								curr_position_grades.begin(),
								get_position_grate_t());

				// is this one better?
				int curr_pos_grade = std::accumulate(curr_position_grades.begin(), curr_position_grades.end(), 0);
				if (curr_pos_grade > best_pos_grade  ||  best_position_iter == 0)
				{
					best_pos_grade = curr_pos_grade;
					best_position_iter = matrix_iter;
				}

				// advance to the next row
				matrix_iter += the_matrix.size_columns();
			}

			// set the best position
			if (best_position_iter != 0)
				std::for_each(best_position_iter, best_position_iter + the_matrix.size_columns(), set_position_t());

		} else {
			// set the best position
			std::for_each(best_positions.begin(), best_positions.end(), set_position_t());
		}
	}

	log_stream << "d";

	// delete all the files we've created
	std::vector<std::string>::iterator tfn_iter = temp_fnames.begin();
	while (tfn_iter != temp_fnames.end())
	{
		remove(tfn_iter->c_str());

		++tfn_iter;
	}

	log_stream << "processing group " << grpcnt << " ended\n";
}
