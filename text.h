#pragma once

#include "point.h"

class text_object
{
public:
	enum class position_code_e {right, left, up, down, right_up, right_down, left_up, left_down, other, invalid};

	typedef int		position_quality_t;

private:
	// the values used for grade caching
	enum {not_overlapping, overlapping, not_set};
	typedef enum {alinman_overlap, meridian_overlap, declination_overlap, out_of_bounds, object_overlap} overlap_type;
	enum {max_positions = 8, cacheable_overlap_types = 4};

	struct cached_position
	{
		position_code_e		position_code;		// enum: right, left...
		point				position;			// the text's upper left corner
		point				left_down_bound;	// the boundaries of the text
		point				right_up_bound;
		bool				overlaps[cacheable_overlap_types];
		double				angle;				// the text angle in this position
		position_quality_t	grade;				// the grade of the position (overlaps with map elements)
		bool				is_perfect;			// no overlaps with any element of the map
	};

	cached_position		_positions_cache[max_positions];
	cached_position*	_end_cache;
	cached_position*	_current_pos;

	void update_angle(cached_position* on_pos);
	void update_bounds(cached_position* on_pos);

	bool overlaps_alinman(const cached_position* on_pos) const;
	bool overlaps_meridian(const cached_position* on_pos) const;
	bool overlaps_declination(const cached_position* on_pos) const;

	void copy_cache(const text_object& c)
	{
		// the cache
		memcpy(_positions_cache, c._positions_cache, sizeof(_positions_cache));

		// the pointers
		_current_pos = _positions_cache + (c._current_pos - c._positions_cache);
		_end_cache = _positions_cache + (c._end_cache - c._positions_cache);
	}

public:

	class position_iterator
	{
		friend class text_object;
	public:
		text_object*		parent;
		cached_position*	curr_pos;

		position_iterator(text_object* p, cached_position* cp)
			: parent(p), curr_pos(cp)
		{}

	public:

		position_iterator()
			: parent(0), curr_pos(0)
		{}

		position_iterator& operator ++ ()
		{
			++curr_pos;
			return *this;
		}

		bool operator == (const position_iterator& rhs) const
		{
			return parent == rhs.parent  &&  curr_pos == rhs.curr_pos;
		}

		bool operator != (const position_iterator& rhs) const
		{
			return !(operator == (rhs));
		}

		std::vector<text_object>::iterator overlaps_another() const;

		text_object* get_parent()
		{
			return parent;
		}

		text_object& operator * ()
		{
			parent->_current_pos = curr_pos;
			return *parent;
		}

		text_object* operator -> ()
		{
			parent->_current_pos = curr_pos;
			return parent;
		}
	};

	position_iterator begin()
	{
		return position_iterator(this, _positions_cache);
	}

	position_iterator end()
	{
		return position_iterator(this, _end_cache);
	}

	std::string				text;					// the text
	bool					is_greek;				// is the text in greek alphabet (Symbol font)
	double					height;					// the size (height) of the text (descent + upcent)

	point					bound_to;				// the position that the text is bound to
	double					bound_diameter;			// the diameter of the object (in mm)
	bool					is_removed;				// is the text removed from the map?
	bool					has_overlaps;			// if the text overlaps et least one other

	text_object()
	:	is_greek(false), is_removed(false),
		_current_pos(_positions_cache), _end_cache(_positions_cache)
	{}

	// copy ctor
	text_object(const text_object& c)
	:	text(c.text), is_greek(c.is_greek), height(c.height),
		bound_to(c.bound_to), bound_diameter(c.bound_diameter),
		is_removed(c.is_removed), has_overlaps(false)
	{
		copy_cache(c);
	}

	const text_object& operator = (const text_object& c)
	{
		text			= c.text;
		is_greek		= c.is_greek;
		height			= c.height;

		bound_to		= c.bound_to;
		bound_diameter	= c.bound_diameter;
		is_removed		= c.is_removed;

		copy_cache(c);

		return *this;
	}

	template <class BoundingObject>
	void set_bounding(const BoundingObject& bound_to_obj)
	{
		bound_to = bound_to_obj.coord.conv2point();
		bound_diameter = (double) bound_to_obj.diameter;
	}

	bool init_valid_positions();

	cached_position& curr_pos()
	{
		return *_current_pos;
	}

	size_t get_current_position_index() const
	{
		return _current_pos - _positions_cache;
	}

	const cached_position& curr_pos() const
	{
		return *_current_pos;
	}

	position_quality_t get_pos_grade() const
	{
		return _current_pos->grade;
	}

	bool is_position_perfect() const
	{
		return _current_pos->is_perfect;
	}

	void init_position(cached_position* p_position);
	void set_position_direct(const point&);
	void set_position_index(const size_t pos_idx)
	{
		_current_pos = _positions_cache + pos_idx;
	}

	// get the number of valid positions of the text
	size_t get_valid_pos_num() const
	{
		return _end_cache - _positions_cache;
	}

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

		return true;	// we won't bother
	}
};

log_t& operator << (log_t& o, const text_object& t);

struct group_t
{
	typedef std::vector<std::vector<text_object>::iterator>		texts_container_t;
	texts_container_t		texts;

	bool operator < (const group_t& rhs) const	{ return texts.size() < rhs.texts.size(); }
	bool operator > (const group_t& rhs) const	{ return texts.size() > rhs.texts.size(); }
};

log_t& operator << (log_t& o, const group_t& t);

struct text_group_processor_t
{
	// process a group -- find it's best position
	void operator () (group_t& grp);
};
