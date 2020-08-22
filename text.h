#pragma once

#include "point.h"

class text_object
{
public:
	enum class position_code_e { right, left, up, down, right_up, right_down, left_up, left_down, other, invalid };

	enum { not_overlapping, overlapping, not_set };
	typedef enum { asterism_overlap, meridian_overlap, declination_overlap, out_of_bounds, overlap_count } overlap_type;

	struct position_t
	{
		position_code_e		position_code;		// enum: right, left...
		point				position;			// the text's upper left corner
		point				left_down_bound;	// the boundaries of the text
		point				right_up_bound;
		bool				overlaps[overlap_count];
		double				angle;				// the text angle in this position
		size_t				grade;				// the grade of the position (overlaps with map elements)

		void set_direct(point& p, text_object& txt)
		{
			position_code = position_code_e::other;
			position = p;

			update_angle(txt);
			update_bounds(txt);
		}

		void update_angle(text_object& text);
		void update_bounds(text_object& text);
	};

private:

	bool overlaps_line(const position_t& on_pos) const;
	bool overlaps_asterism(const position_t& on_pos) const;
	bool overlaps_meridian(const position_t& on_pos) const;
	bool overlaps_declination(const position_t& on_pos) const;

public:

	struct position_selector
	{
		text_object*							parent;
		std::vector<position_t>::iterator		curr_pos;

		explicit position_selector(text_object* p)
			: parent(p), curr_pos(p->valid_positions.begin())
		{}

		position_selector(text_object* p, std::vector<position_t>::iterator& cp)
			: parent(p), curr_pos(cp)
		{}

		bool operator == (const position_selector& rhs) const
		{
			return parent == rhs.parent && curr_pos == rhs.curr_pos;
		}

		bool operator != (const position_selector& rhs) const
		{
			return !(operator == (rhs));
		}

		position_selector begin()
		{
			return position_selector(parent);
		}

		position_selector end()
		{
			return position_selector(parent, parent->valid_positions.end());
		}

		const position_selector& operator* ()
		{
			return *this;
		}

		position_selector& operator ++ ()
		{
			++curr_pos;
			return *this;
		}
	};

	std::string				text;					// the text
	bool					is_greek;				// is the text in greek alphabet (Symbol font)
	double					height;					// the size (height) of the text (descent + upcent)

	point					bound_to;				// the position that the text is bound to
	double					bound_diameter;			// the diameter of the object (in mm)
	bool					is_removed;				// is the text removed from the map?
	bool					has_overlaps;			// if the text overlaps at least one other text

	std::vector<position_t>	valid_positions;

	text_object()
		: is_greek(false), is_removed(false)
	{}

	// copy ctor
	text_object(const text_object& c)
		: text(c.text), is_greek(c.is_greek), height(c.height),
		bound_to(c.bound_to), bound_diameter(c.bound_diameter),
		is_removed(c.is_removed), has_overlaps(false),
		valid_positions(c.valid_positions)
	{}

	const position_t& curr_pos() const
	{
		return valid_positions.front();
	}

	const text_object& operator = (const text_object& c)
	{
		text			= c.text;
		is_greek		= c.is_greek;
		height			= c.height;

		bound_to		= c.bound_to;
		bound_diameter	= c.bound_diameter;
		is_removed		= c.is_removed;

		valid_positions = c.valid_positions;

		return *this;
	}

	template <class BoundingObject>
	void set_bounding(const BoundingObject& bound_to_obj)
	{
		bound_to = bound_to_obj.coord.conv2point();
		bound_diameter = (double) bound_to_obj.diameter;
	}

	//void set_position_direct(const point& p);
	bool find_valid_positions();

	void init_position(position_t& pos);

	void print() const;			// prints the text
	void connect() const;		// connects the text with the object (not used really)

	bool has_descent() const
	{
		if (is_greek)
			return text.find_first_of("bgzhmxrjcy") != std::string::npos;

		return text.find_first_of("qypjg") != std::string::npos;
	}

	bool has_upcent() const
	{
		if (is_greek)
			return text.find_first_of("bdJzlx0123456789") != std::string::npos;

		return true;	// we won't bother with one-off cases
	}
};

typedef std::vector<std::vector<text_object>::iterator>	group_t;

struct overlap_t;

log_t& operator << (log_t& o, const text_object& t);
log_t& operator << (log_t& o, const group_t& t);
log_t& operator << (log_t& o, const overlap_t& ovp);

struct overlap_t
{
	size_t	txt1;
	size_t	pos1;

	size_t	txt2;
	size_t	pos2;

	overlap_t(const text_object& t1, const text_object::position_t& p1, const text_object& t2, const text_object::position_t& p2);
	overlap_t(const size_t t1, const size_t p1, const size_t t2, const size_t p2);

	bool operator == (const overlap_t& rhs) const
	{
		return		txt1 == rhs.txt1
			&& pos1 == rhs.pos1
			&& txt2 == rhs.txt2
			&& pos2 == rhs.pos2;
	}
};

