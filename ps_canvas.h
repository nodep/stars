#pragma once

#include "point.h"
#include "text.h"
#include "star.h"
#include "colors.h"
#include "irregular_surface.h"
#include "ngc.h"
#include "radiant.h"
#include "custom_objects.h"

class post_script_canvas;

extern post_script_canvas* canvas;

class post_script_canvas
{
private:
	double			_extent;
	std::ofstream	_ps_file;
	std::string		_curr_font_name;
	double			_curr_font_height;

	// the post script graphic state
	color_t			_color;
	pen_t			_pen;
	double			_curr_pen_width;	// used only if _pen == other

public:
	post_script_canvas()
	:	_color(black),
		_pen(other),
		_curr_pen_width(0.0)
	{
		assert(canvas == 0);
		canvas = this;
	}

	~post_script_canvas()
	{
		assert(canvas == this);
		canvas = 0;
	}

	double get_extent() const		{ return _extent; }

	void set_color(const color_t col);
	void set_pen_direct(const pen_t);
	void set_pen_width(const double&);
	void set_font(const std::string&, const double&);

	void line(const point&, const point&);
	void arc(const point&, const double&, const double&, const double&);

	void stroke_circle(const point& center, const double& r)
	{
		_ps_file << get_circle_path(center, r) << "stroke" << std::endl;
	}

	static std::string get_circle_path(const dekart& center, const double& r);
	static std::string get_circle_path(const point& center, const double& r)
	{
		return get_circle_path(center.conv2dekart(), r);
	}

	void draw(const star&);
	void draw(const irregular_surface&);
	void draw(const galaxy&);
	void draw(const nebula&);
	void draw(const planetary_nebula&);
	void draw(const open_cluster&);
	void draw(const globular_cluster&);
	void draw(const radiant&);
	void draw(const custom_object&);

	void draw(const text_object&);

	void clip_extent();
	void clip_restore();

    void comment(const std::string&);

	void start_drawing();
	void end_drawing();
};
