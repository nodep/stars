#include "stl.h"
#pragma hdrstop

#include "config.h"

config* cfg = 0;

void config::parse_file(const char* file_name)
{
	std::ifstream cfg_file(file_name);

	if (cfg_file.is_open() == false)
	{
		throw "unable to open config file <" + std::string(file_name) + ">";
	} else {
		log_stream << "reading config file <" << file_name << '>' << "\n";

		std::string line, var_name, value;
		size_t index;
		char p[150];

		while (true)
		{
			cfg_file.getline(p, 150);

			if (cfg_file.eof())
				break;

			line = p;

			// kill the comments
			index = line.find("//");
			if (index != std::string::npos)
				line = line.substr(0, index);

			/*
			// kill everything after the first white space
			index = line.find_first_of(" \t");
			if (index != std::string::npos)
				line = line.substr(0, index);
			*/

			index = line.find('=');
			if (index != std::string::npos)
			{
				var_name = line.substr(0, index);

				if (index + 1 == line.length())
					throw std::string("Syntax error in config file ") + file_name + " in line:\n" + p;

				value = line.substr(index + 1);
			} else
				var_name = value = "";

			// assign the value
			if (var_name == "OutputFileName")
			{
				_output_file_name = value;
            } else if (var_name == "ScaleFactor") {
                _scale_factor = atof(value.c_str());
			} else if (var_name == "DBGTextFrame") {
				_draw_text_frame = (value == "1");
			} else if (var_name == "DBGRegion") {
				if (value == "ALL")
				{
					_debug_region = dr_all;
				} else if (value == "ORION") {
					_debug_region = dr_orion;
				} else if (value == "ORIONS_BELT") {
					_debug_region = dr_orions_belt;
				} else if (value == "COMMA_BERENICES") {
					_debug_region = dr_comma_berenices;
				} else if (value == "CYGNUS") {
					_debug_region = dr_cygnus;
				} else {
					std::stringstream ss(value);
					_debug_region = dr_custom;
					ss >> _RA_range.first;
					ss >> _RA_range.second;
					ss >> _Dec_range.first;
					ss >> _Dec_range.second;
				}
			} else if (var_name == "MapRadius") {
				_map_radius = atof(value.c_str());
			} else if (var_name == "SpareSpace") {
				_spare_space = atof(value.c_str());
			} else if (var_name == "RATextHeight") {
				_ra_text_height = atof(value.c_str());
			} else if (var_name == "DeclTextHeight") {
				_decl_text_height = atof(value.c_str());
			} else if (var_name == "IsNorth") {
				_is_north = (value == "1");
			} else if (var_name == "DrawConstellationBounds") {
				_draw_const_bounds = (value == "1");
			} else if (var_name == "DrawEcliptic") {
    			_draw_ecliptic = (value == "1");
			} else if (var_name == "DrawGalacticEquator") {
    			_draw_gal_equator = (value == "1");
			} else if (var_name == "CirclesDotsFrequency") {
    			_circle_dots_frequency = atof(value.c_str());
			} else if (var_name == "ConstellationBoundType") {

				if (value == "FULL")
					_const_bound_type = full_bounds;
				else if (value == "CORNER")
					_const_bound_type = corner_bounds;
				else
					throw "unknown constellation boundary type: " + value;

			} else if (var_name == "ConstellationBoundStyle") {

				if (value == "SOLID")
					_const_bound_style = ls_solid;
				else if (value == "DOTS")
					_const_bound_style = ls_dots;
				else
					throw "unknown constellation boundary style.";

			} else if (var_name == "IgnorePleiades") {
				_ignore_pleiades = (value == "1");
			} else if (var_name == "DrawNet") {
				_draw_net = (value == "1");
			} else if (var_name == "ConstellationBoundAngle") {
				_const_bound_angle = atof(value.c_str());
			} else if (var_name == "MaxAngleLengthFactor") {
				_max_angle_length_factor = atof(value.c_str());
			} else if (var_name == "MinAngleLengthFactor") {
				_min_angle_length_factor = atof(value.c_str());
			} else if (var_name == "NamesFont") {
				_names_font = value;
			} else if (var_name == "TextDistance") {
				_text_distance = atof(value.c_str());
			} else if (var_name == "PrintFixedText") {
				_print_fixed_text = (value == "1");
			} else if (var_name == "FixedTextFile") {
				_fixed_text_file_name = value;
			} else if (var_name == "DrawStars") {
				_draw_stars = (value == "1");
			} else if (var_name == "DrawStarFrame") {
				_draw_star_frame = (value == "1");
			} else if (var_name == "VariableStarSymbolType") {

				if (value == "NONE")
					_variable_symbol_type = vst_none;
				else if (value == "RING")
					_variable_symbol_type = vst_ring;
				else if (value == "VAR")
					_variable_symbol_type = vst_var;
				else
					throw "unknown variable start symbol type";

			} else if (var_name == "VarTextHeight") {
				_var_text_height = atof(value.c_str());
			} else if (var_name == "DrawMultipleStarSign") {
				_draw_multiple_sign = (value == "1");
			} else if (var_name == "StarFrameWidth") {
				_star_frame_width = atof(value.c_str());
			} else if (var_name == "MaxPoleDistance") {
				_max_pole_distance = atof(value.c_str());
			} else if (var_name == "StarMagnitudeLimit") {
				_star_mag_limit = atof(value.c_str());
			} else if (var_name == "MaxStarDiameter") {
				_max_star_diam = atof(value.c_str());
			} else if (var_name == "MinStarDiameter") {
				_min_star_diam = atof(value.c_str());
			} else if (var_name == "StarDiameterType") {

				if (value == "LINEAR")
					_star_diam_type = dft_linear;
				else if (value == "ATAN")
					_star_diam_type = dft_atan;
				else if (value == "CONST")
					_star_diam_type = dft_const;
				else if (value == "EXP")
					_star_diam_type = dft_exp;
				else
					throw "unknown star diameter type";

			} else if (var_name == "PrintStarNames") {
				_print_star_names = (value == "1");
			} else if (var_name == "StarNameHeight") {
				_star_name_height = atof(value.c_str());
			} else if (var_name == "MinStarNameMagnitude") {
				_star_name_mag_limit = atof(value.c_str());
			} else if (var_name == "PrintBayerDesignations") {
				_bayer_dsgn = (value == "1");
			} else if (var_name == "BayerOptimizationsType") {
				if (value == "NONE") 
					_bayer_optimization_type = /*bayer_optimization_type::*/ bot_none;
				else if (value == "REDUCE")
					_bayer_optimization_type = /*bayer_optimization_type::*/ bot_reduce;
				else if (value == "CONCAT")
					_bayer_optimization_type = /*bayer_optimization_type::*/ bot_concat;
				else
					throw "unknown BayerOptimizationType type";

			} else if (var_name == "PrintVariableDesignations") {

				if (value == "ALL")
					_var_dsgn_type = vdt_all;
				else if (value == "NONE")
					_var_dsgn_type = vdt_none;
				else if (value == "NOT_V_NUM")
					_var_dsgn_type = vdt_not_v_num;
				else
					throw "unknown PrintVariableDesignations type";

			} else if (var_name == "PrintFlamsteedDesignations") {
				_flamsteed_dsgn = (value == "1");
			} else if (var_name == "BayerDesignationHeight") {
				_bayer_dsgn_height = atof(value.c_str());
			} else if (var_name == "FlamsteedDesignationHeight") {
				_flamsteed_dsgn_height = atof(value.c_str());
			} else if (var_name == "MinStarDesignationMagnitude") {
				_star_dsgn_mag_limit = atof(value.c_str());
			} else if (var_name == "DrawAsterism") {
				_draw_asterism = (value == "1");
			} else if (var_name == "AsterismFileName") {
				_asterism_file_name = value;
			} else if (var_name == "AsterismLineStyle") {

				if (value == "SOLID")
					_asterism_style = ls_solid;
				else if (value == "DOTS")
					_asterism_style = ls_dots;
				else
					throw "unknown asterism style.";

			} else if (var_name == "NGCFilename") {
				_ngc_filename = value;
			} else if (var_name == "DrawNGCType") {
				if (value == "ALL")
					_draw_ngc_type = ngc_all;
				else if (value == "MESSIER_ONLY")
					_draw_ngc_type = ngc_messier_only;
				else if (value == "NONE")
					_draw_ngc_type = ngc_none;
				else
					throw "unknown DrawNGCType.";
			} else if (var_name == "NGCNamesHeight") {
				_ngc_names_height = atof(value.c_str());
			} else if (var_name == "NGCNamesType") {
				if (value == "NONE")
					_ngc_names_type = ngcname_none;
				else if (value == "NGC")
					_ngc_names_type = ngcname_ngc;
				else if (value == "MESSIER")
					_ngc_names_type = ngcname_messier;
				else if (value == "BOTH")
					_ngc_names_type = ngcname_both;
				else
					throw "unknown NGCNamesType.";
			} else if (var_name == "DrawGalaxies") {
				_draw_galaxies = (value == "1");
			} else if (var_name == "GalaxyMagnitudeLimit") {
				_galaxy_mag_limit = atof(value.c_str());
			} else if (var_name == "MinGalaxyDiameter") {
				_min_galaxy_diam = atof(value.c_str());
			} else if (var_name == "MaxGalaxyDiameter") {
				_max_galaxy_diam = atof(value.c_str());
			} else if (var_name == "DrawOpenClusters") {
				_draw_open_clusters = (value == "1");
			} else if (var_name == "OpenClusterMagnitudeLimit") {
				_open_cluster_mag_limit = atof(value.c_str());
			} else if (var_name == "MinOpenClusterDiameter") {
				_min_open_cluster_diam = atof(value.c_str());
			} else if (var_name == "MaxOpenClusterDiameter") {
				_max_open_cluster_diam = atof(value.c_str());
			} else if (var_name == "DrawGlobularClusters") {
				_draw_glob_clusters = (value == "1");
			} else if (var_name == "GlobClusterMagnitudeLimit") {
				_glob_cluster_mag_limit = atof(value.c_str());
			} else if (var_name == "MinGlobClusterDiameter") {
				_min_glob_cluster_diam = atof(value.c_str());
			} else if (var_name == "MaxGlobClusterDiameter") {
				_max_glob_cluster_diam = atof(value.c_str());
			} else if (var_name == "DrawNebula") {
				_draw_nebula = (value == "1");
			} else if (var_name == "NebulaMagnitudeLimit") {
				_nebula_mag_limit = atof(value.c_str());
			} else if (var_name == "MinNebulaDiameter") {
				_min_nebula_diam = atof(value.c_str());
			} else if (var_name == "MaxNebulaDiameter") {
				_max_nebula_diam = atof(value.c_str());
			} else if (var_name == "DrawPlanetaryNebula") {
				_draw_planetary_nebula = (value == "1");
			} else if (var_name == "PlanetaryNebulaMagnitudeLimit") {
				_planetary_nebula_mag_limit = atof(value.c_str());
			} else if (var_name == "MinPlanetaryNebulaDiameter") {
				_min_planetary_nebula_diam = atof(value.c_str());
			} else if (var_name == "MaxPlanetaryNebulaDiameter") {
				_max_planetary_nebula_diam = atof(value.c_str());
			} else if (var_name == "DrawQuasars") {
				_draw_quasars = (value == "1");
			} else if (var_name == "QuasarMagnitudeLimit") {
				_quasar_mag_limit = atof(value.c_str());
			} else if (var_name == "QuasarNameHeight") {
				_quasar_name_height = atof(value.c_str());
			} else if (var_name == "QuasarDiameter") {
				_quasar_diameter = atof(value.c_str());
			} else if (var_name == "DrawX-RaySources") {
				_draw_xray = (value == "1");
			} else if (var_name == "X-RaySymbolDiameter") {
				_xray_diameter = atof(value.c_str());
			} else if (var_name == "X-RayNameHeight") {
				_xray_name_height = atof(value.c_str());
			} else if (var_name == "DrawMilkyWay") {
				_draw_milky_way = (value == "1");
			} else if (var_name == "MilkyWayFile") {
				_milky_way_file_name = value;
			} else if (var_name == "MilkyWayStyle") {

				if (value == "SOLID")
					_milky_way_style = mws_solid;
				else if (value == "DOTS")
					_milky_way_style = mws_dots;
				else if (value == "FILL")
					_milky_way_style = mws_fill;
				else
					throw "unknown milky way style.";

			} else if (var_name == "DrawRadiants") {
				_draw_radiants = (value == "1");
			} else if (var_name == "RadiantsFileName") {
				_radiants_file_name = value;
			} else if (var_name == "RadiantsDesignationSize") {
				_radiants_text_size = atof(value.c_str());
			} else if (var_name == "CustomObjects") {

				_draw_custom_objects = true;

			} else if (!var_name.empty()) {
				log_stream << "<!warning!>Unknown config parameter " + var_name + "\n";
			}
		}
	}

	if (print_bayer_dsgn())
		_read_ar_file(symbol_AR, "Symbol");

	_read_ar_file(char_AR, _names_font);
}

void config::_read_ar_file(double ar_array[256], const std::string& font_name)
{
	log_stream << "reading aspect ratios for font " << font_name << "...\n";

	std::string file_name = font_name + ".dat";
	std::ifstream ar_file(file_name.c_str());

	if (!ar_file.is_open())
		throw "unable to open aspect ratios definitions in file " + file_name;

	char p[100];
	std::string line;
	size_t srchPos, index;
	while (true)
	{
		ar_file.getline(p, 100);

		if (ar_file.eof())
			break;

		line = p;

		// kill the comment
		srchPos = line.find("//");
		if (srchPos != std::string::npos)
			line[srchPos] = '\0';

		index = atoi(line.c_str());
		if (index < 256)
		{
			// find the space
			srchPos = line.find_first_of(" \t");
			if (srchPos != std::string::npos)
				ar_array[index] = atof(line.c_str() + srchPos);
		}
	}
}

void config::_set_to_default()
{
	_draw_text_frame = false;
	_output_file_name = "sky.eps";
	_circle_dots_frequency = 0.01;
	_ignore_pleiades = true;

	_is_north = true;
	_map_radius = 300;
	_scale_factor = 1.0;
	_spare_space = 5;
	_ra_text_height = 4;
	_decl_text_height = 2;
	_max_pole_distance = 120;
	_names_font = "Helvetica";

	_draw_net = true;

	_draw_ecliptic = true;
	_draw_gal_equator = true;

	_projection_center = coordinate(90, 0);

	// the milky way
	_draw_milky_way = true;
	_milky_way_file_name = "milkyway.dat";
	_milky_way_style = mws_fill;

	// the constellation boundaries
	_draw_const_bounds = true;
	_const_bound_type = full_bounds;
	_const_bound_style = ls_dots;
	_const_bound_angle = 1.5;
	_min_angle_length_factor = 0.0018;
	_max_angle_length_factor = 0.0012;

	_text_distance = 0.7;
	_print_fixed_text = false;
	_fixed_text_file_name = "";

	_draw_radiants = true;
	_radiants_text_size = 2;
	_radiants_file_name = "radiants.dat";

	_draw_custom_objects = true;
	_custom_objects_file_name = "custom_objects.dat";

	// the stars
	_draw_stars = true;
	_print_star_names = true;
	_star_diam_type = dft_atan;
	_draw_star_frame = true;
	_star_frame_width = 0.3;
	_variable_symbol_type = vst_var;
	_var_text_height = 1.2;
	_draw_multiple_sign = true;
	_min_star_diam = 1.5;
	_max_star_diam = 0.23;
	_star_mag_limit = 5.5;
	_star_name_height = 2.5;
	_star_name_mag_limit = 5.5;
	_bayer_dsgn = true;
	_bayer_optimization_type = /*bayer_optimization_type::*/ bot_reduce;
	_var_dsgn_type = vdt_not_v_num;
	_flamsteed_dsgn = true;
	_bayer_dsgn_height = 0;
	_flamsteed_dsgn_height = 0;
	_star_dsgn_mag_limit = 6;

	// the constellation asterisms
	_draw_asterism = true;
	_asterism_file_name = "clinks.dat";
	_asterism_style = ls_solid;

	// ngc
	_draw_ngc_type = ngc_messier_only;
	_ngc_names_height = 1.8;
	_ngc_names_type = ngcname_messier;

	_draw_galaxies = true;
	_galaxy_mag_limit = 10;
	_min_galaxy_diam = 0.7;
	_max_galaxy_diam = 2.0;

	_draw_open_clusters = true;
	_open_cluster_mag_limit = 5;
	_min_open_cluster_diam = 0.6;
	_max_open_cluster_diam = 0.7;

	_draw_glob_clusters = true;
	_glob_cluster_mag_limit = 9;
	_min_glob_cluster_diam = 0.6;
	_max_glob_cluster_diam = 0.7;

	_draw_nebula = true;
	_nebula_mag_limit = 10;
	_min_nebula_diam = 0.6;
	_max_nebula_diam = 0.7;

	_draw_planetary_nebula = true;
	_planetary_nebula_mag_limit = 9;
	_min_planetary_nebula_diam = 0.6;
	_max_planetary_nebula_diam = 0.7;

	_draw_quasars = true;
	_quasar_mag_limit = 16;
	_quasar_name_height = 1.7;
	_quasar_diameter = 1;

	_draw_xray = true;
	_xray_diameter = 1.7;
	_xray_name_height = 0.8;
}
