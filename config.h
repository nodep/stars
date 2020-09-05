#pragma once

#include "coordinate.h"

class config;
extern config* cfg;

// coordinate
// forwarding
struct galaxy_tag;
struct nebula_tag;
struct planetary_nebula_tag;
struct open_cluster_tag;
struct globular_cluster_tag;

struct star;

template <class ObjectTag, size_t object_id>
struct ngc_object;

typedef ngc_object<galaxy_tag, 2>				galaxy;
typedef ngc_object<nebula_tag, 3>				nebula;
typedef ngc_object<planetary_nebula_tag, 4>		planetary_nebula;
typedef ngc_object<open_cluster_tag, 5>			open_cluster;
typedef ngc_object<globular_cluster_tag, 6>		globular_cluster;

class config
{
public:
	typedef enum {full_bounds, corner_bounds}								constellation_bound_type;
	typedef enum {ls_solid, ls_dots}										line_style;
	typedef enum {mws_solid, mws_dots, mws_fill}							milky_way_style;
	typedef enum {ngc_all, ngc_messier_only, ngc_none}						ngc_type;
	typedef enum {ngcname_none, ngcname_ngc, ngcname_messier, ngcname_both} ngc_names_type;
	typedef enum {vdt_none, vdt_all, vdt_not_v_num}							var_dsgn_type;
	typedef enum {dft_linear, dft_atan, dft_const, dft_exp}					diam_function_type;
	typedef enum {dr_all, dr_orion, dr_orions_belt, dr_comma_berenices, dr_cygnus, dr_custom}		debug_region;
	typedef enum {vst_none, vst_ring, vst_var}								variant_symbol_type;
	typedef enum {bot_none, bot_concat, bot_reduce}							bayer_optimization_type;

private:
	// overall
	bool							_draw_text_frame;
	std::pair<double, double>		_RA_range;
	std::pair<double, double>		_Dec_range;

	std::string						_output_file_name;
	double							_circle_dots_frequency;
	coordinate						_projection_center;

	// text positioning algorithm
	bool							_ignore_pleiades;

	// the map
	bool							_is_north;
	double							_map_radius;
	double							_scale_factor;
	double							_spare_space;
	double							_ra_text_height;
	double							_decl_text_height;
	double							_max_pole_distance;
	std::string						_names_font;

	// the milky way
	bool							_draw_milky_way;
	std::string						_milky_way_file_name;
	milky_way_style					_milky_way_style;

	// the net
	bool							_draw_net;
	bool							_draw_ecliptic;
	bool							_draw_gal_equator;

	// the constellation boundaries
	bool							_draw_const_bounds;
	constellation_bound_type		_const_bound_type;
	line_style						_const_bound_style;
	double							_const_bound_angle;
	double							_min_angle_length_factor;
	double							_max_angle_length_factor;

	// the texts
	double							_text_distance;
	bool							_print_fixed_text;
	std::string						_fixed_text_file_name;

	// radiants
	bool							_draw_radiants;
	std::string						_radiants_file_name;
	double							_radiants_text_size;

	// custom objects
	bool							_draw_custom_objects;
	std::string						_custom_objects_file_name;

	// the stars
	bool							_draw_stars;
	bool							_print_star_names;
	diam_function_type				_star_diam_type;
	bool							_draw_star_frame;
	variant_symbol_type				_variable_symbol_type;
	double							_var_text_height;
	bool							_draw_multiple_sign;
	double							_star_frame_width;
	double							_min_star_diam;
	double							_max_star_diam;
	double							_star_mag_limit;
	double							_star_name_height;
	double							_star_name_mag_limit;
	bool							_bayer_dsgn;
	bayer_optimization_type			_bayer_optimization_type;
	var_dsgn_type					_var_dsgn_type;
	bool							_flamsteed_dsgn;
	double							_bayer_dsgn_height;
	double							_flamsteed_dsgn_height;
	double							_star_dsgn_height;
	double							_star_dsgn_mag_limit;

	// the constellation asterisms
	bool							_draw_asterism;
	std::string						_asterism_file_name;
	line_style						_asterism_style;

	// ngc
	std::string						_ngc_filename;
	ngc_type						_draw_ngc_type;
	double							_ngc_names_height;
	ngc_names_type					_ngc_names_type;
	bool							_draw_galaxies;
	double							_galaxy_mag_limit;
	double							_min_galaxy_diam;
	double							_max_galaxy_diam;
	bool							_draw_open_clusters;
	double							_open_cluster_mag_limit;
	double							_min_open_cluster_diam;
	double							_max_open_cluster_diam;
	bool							_draw_glob_clusters;
	double							_glob_cluster_mag_limit;
	double							_min_glob_cluster_diam;
	double							_max_glob_cluster_diam;
	bool							_draw_nebula;
	double							_nebula_mag_limit;
	double							_min_nebula_diam;
	double							_max_nebula_diam;
	bool							_draw_planetary_nebula;
	double							_planetary_nebula_mag_limit;
	double							_min_planetary_nebula_diam;
	double							_max_planetary_nebula_diam;
	bool							_draw_quasars;
	double							_quasar_mag_limit;
	double							_quasar_name_height;
	double							_quasar_diameter;
	bool							_draw_xray;
	double							_xray_diameter;
	double							_xray_name_height;

	std::string			_file_name;

	void _set_to_default();
	void _read_ar_file(double ar_array[256], const std::string& font_name);

public:

	// the character aspect ratios
	double char_AR[256];		// for the titles font
	double symbol_AR[256];	// for the symbol font

	config()
	{
		assert(cfg == 0);
		cfg = this;

		_set_to_default();
	}

	~config()
	{
		assert(cfg == this);
		cfg = 0;
	}

	void parse_file(const char* file_name);

	// overall stuff
	bool draw_text_frame() const							{ return _draw_text_frame; }
	const std::pair<double, double>& get_ra_range() const	{ return _RA_range; }
	const std::pair<double, double>& get_dec_range() const	{ return _Dec_range; }

	std::string read_str(const char* name) const;

	std::string get_output_file_name() const				{ return _output_file_name; }
	double get_circle_dots_frequency() const				{ return _circle_dots_frequency; }

	// the positioning algorithm
	bool ignore_pleiades() const							{ return _ignore_pleiades; }

	// the map
	bool is_north() const									{ return _is_north; }
	double get_map_radius() const							{ return _map_radius; }
	double get_scale_factor() const							{ return _scale_factor; }
	double get_spare_space() const							{ return _spare_space; }
	double get_ra_text_height() const						{ return _ra_text_height; }
	double get_decl_text_height() const						{ return _decl_text_height; }
	double get_max_pole_distance() const					{ return _max_pole_distance; }
	const std::string& get_names_font() const				{ return _names_font; }

	// the net
	bool draw_net() const									{ return _draw_net; }
	bool draw_ecliptic() const								{ return _draw_ecliptic; }
	bool draw_galactic_equator() const						{ return _draw_gal_equator; }

	// the milky way
	bool draw_milky_way() const								{ return _draw_milky_way; }
	const std::string& get_milky_way_isophotes_file() const	{ return _milky_way_file_name; }
	milky_way_style get_milky_way_line_style() const		{ return _milky_way_style; }

	// the texts
	double get_text_distance()								{ return _text_distance; }
	bool print_fixed_text() const							{ return _print_fixed_text; }
	const std::string& get_fixed_text_file_name() const		{ return _fixed_text_file_name; }

	diam_function_type get_diam_func_type(const star&) const	{ return _star_diam_type; }

	template <class Object>
	diam_function_type get_diam_func_type(const Object&) const	{ return dft_const; }

	// the meteor radiants
	bool draw_radiants() const								{ return _draw_radiants; }
	double get_radiants_text_size() const					{ return _radiants_text_size; }
	const std::string& get_radiants_file_name() const		{ return _radiants_file_name; }

	bool draw_custom_objects() const						{ return _draw_custom_objects; }
	std::string custom_objects_file_name() const			{ return _custom_objects_file_name; }

	// the stars
	bool draw_stars() const									{ return _draw_stars; }
	bool print_star_names() const							{ return _print_star_names; }

	bool draw_star_frame() const							{ return _draw_star_frame; }
	variant_symbol_type get_variable_symbol_type() const	{ return _variable_symbol_type; }
	double get_var_text_height() const						{ return _var_text_height; }
	bool draw_multiple_sign() const							{ return _draw_multiple_sign; }

	double get_star_frame_width() const						{ return _star_frame_width; }
	double get_min_diam(const star&) const					{ return _min_star_diam; }
	double get_max_diam(const star&) const					{ return _max_star_diam; }
	double get_star_magnitude_limit() const					{ return _star_mag_limit; }
	double get_star_name_mag_limit() const					{ return _star_name_mag_limit; }
	double get_star_name_height() const						{ return _star_name_height; }
	bool print_bayer_dsgn() const							{ return _bayer_dsgn; }
	bool print_flamsteed_dsgn() const						{ return _flamsteed_dsgn; }
	bayer_optimization_type get_bayer_optimization_type() const		{ return _bayer_optimization_type; }
	var_dsgn_type get_var_dsgn_type() const					{ return _var_dsgn_type; }
	double get_flamsteed_dsgn_height() const				{ return _flamsteed_dsgn_height; }
	double get_bayer_dsgn_height() const					{ return _bayer_dsgn_height; }
	double get_star_dsgn_mag_limit() const					{ return _star_dsgn_mag_limit; }

	// the constellation asterisms
	bool draw_asterism() const								{ return _draw_asterism; }
	const std::string& get_asterism_file_name() const		{ return _asterism_file_name; }
	line_style get_asterism_style() const					{ return _asterism_style; }

	// constellation bounds
	bool draw_const_bounds() const							{ return _draw_const_bounds; }
	line_style get_const_bounds_style() const				{ return _const_bound_style; }
	constellation_bound_type get_const_bound_type() const	{ return _const_bound_type; }
	double get_const_bound_angle() const					{ return _const_bound_angle; }
	double get_min_angle_length_factor() const				{ return _min_angle_length_factor; }
	double get_max_angle_length_factor() const				{ return _max_angle_length_factor; }

	// ngc
	const std::string get_ngc_filename() const				{ return _ngc_filename; }
	ngc_type draw_ngc_type() const							{ return _draw_ngc_type; }
	double get_ngc_names_height() const						{ return _ngc_names_height; }
	ngc_names_type get_ngc_names_type() const				{ return _ngc_names_type; }

	bool draw_galaxies() const								{ return _draw_galaxies; }
	double get_mag_limit(const galaxy&) const				{ return _galaxy_mag_limit; }
	double get_min_diam(const galaxy&) const				{ return _min_galaxy_diam; }
	double get_max_diam(const galaxy&) const				{ return _max_galaxy_diam; }

	bool draw_open_clusters() const							{ return _draw_open_clusters; }
	double get_mag_limit(const open_cluster&) const			{ return _open_cluster_mag_limit; }
	double get_min_diam(const open_cluster&) const			{ return _min_open_cluster_diam; }
	double get_max_diam(const open_cluster&) const			{ return _max_open_cluster_diam; }

	bool draw_globular_clusters() const						{ return _draw_glob_clusters; }
	double get_mag_limit(const globular_cluster&) const		{ return _glob_cluster_mag_limit; }
	double get_min_diam(const globular_cluster&) const		{ return _min_glob_cluster_diam; }
	double get_max_diam(const globular_cluster&) const		{ return _max_glob_cluster_diam; }

	bool draw_nebula() const								{ return _draw_nebula; }
	double get_mag_limit(const nebula&) const				{ return _nebula_mag_limit; }
	double get_min_diam(const nebula&) const				{ return _min_nebula_diam; }
	double get_max_diam(const nebula&) const				{ return _max_nebula_diam; }

	bool draw_planetary_nebula() const						{ return _draw_planetary_nebula; }
	double get_mag_limit(const planetary_nebula&) const		{ return _planetary_nebula_mag_limit; }
	double get_min_diam(const planetary_nebula&) const		{ return _min_planetary_nebula_diam; }
	double get_max_diam(const planetary_nebula&) const		{ return _max_planetary_nebula_diam; }
};
