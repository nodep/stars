#include <gwin>
#pragma hdrstop

#include "resource.h"

class config_t
{
public:
	typedef enum {cb_full, cb_corner}					constellation_bound_e;
	typedef enum {ls_solid, ls_dots}					line_style_e;
	typedef enum {mws_dots, mws_solid, mws_fill}		milky_way_style_e;
	typedef enum {ngc_all, ngc_messier_only, ngc_none}	ngc_type_e;
	typedef enum {ngcname_none, ngcname_ngc, ngcname_messier, ngcname_both} ngc_names_type_e;
	typedef enum {vdt_none, vdt_all, vdt_not_v_num}		var_dsgn_type_e;
	typedef enum {dft_linear, dft_atan}					diam_function_type_e;
	typedef enum {dr_all, dr_orion, dr_orions_belt}		debug_region_e;

private:

	line_style_e _get_line_style(const char* key)
	{
		std::string par = gwin::read_config<std::string>("", key);

		if (par == "SOLID")		return ls_solid;
		if (par == "DOTS")		return ls_dots;

		return ls_solid;
	}

public:

	bool draw_text_frame()
	{
		return gwin::read_config<bool>("", "DBGTextFrame");
	}

	debug_region_e get_debug_region()
	{
		std::string par = gwin::read_config<std::string>("", "DBGRegion");

		if (par == "ALL")			return dr_all;
		if (par == "ORION")			return dr_orion;
		if (par == "ORIONS_BELT")	return dr_orions_belt;

		return dr_all;
	}

	std::string get_output_file_name()	{ return gwin::read_config<std::string>("", "OutputFileName"); }
	std::string get_map_radius()		{ return gwin::read_config<std::string>("", "MapRadius"); }
	std::string get_spare_space()		{ return gwin::read_config<std::string>("", "SpareSpace"); }
	std::string get_scale_factor()		{ return gwin::read_config<std::string>("", "ScaleFactor"); }

	bool is_north()						{ return gwin::read_config<bool>("", "IsNorth"); }
	bool draw_ecliptic()				{ return gwin::read_config<bool>("", "DrawEcliptic"); }
	bool draw_galactic_equator()		{ return gwin::read_config<bool>("", "DrawGalacticEquator"); }

	std::string get_circle_dots_frequency()	{ return gwin::read_config<std::string>("", "CirclesDotsFrequency"); }
	std::string get_max_pole_distance()		{ return gwin::read_config<std::string>("", "MaxPoleDistance"); }
	bool draw_milky_way()					{ return gwin::read_config<bool>("", "DrawMilkyWay"); }

	milky_way_style_e get_milky_way_line_style()
	{
		std::string par = gwin::read_config<std::string>("", "MilkyWayStyle");

		// DOTS		- Isophote with dots
		// SOLID	- Isophote with solid lines
		// FILL		- Filled grayscale surfaces

		if (par == "DOTS")			return mws_dots;
		if (par == "SOLID")			return mws_solid;
		if (par == "FILL")			return mws_fill;

		return mws_dots;
	}

	bool ignore_pleiades()					{ return gwin::read_config<bool>("", "IgnorePleiades"); }
	bool draw_radiants()					{ return gwin::read_config<bool>("", "DrawRadiants"); }
	std::string get_radiants_text_size()	{ return gwin::read_config<std::string>("", "RadiantsDesignationSize"); }

	std::string get_names_font()			{ return gwin::read_config<std::string>("", "NamesFont"); }
	bool print_fixed_text()					{ return gwin::read_config<bool>("", "PrintFixedText"); }
	std::string get_fixed_text_file_name()	{ return gwin::read_config<std::string>("", "FixedTextFile"); }
	std::string get_text_distance()			{ return gwin::read_config<std::string>("", "TextDistance"); }

	bool draw_net()							{ return gwin::read_config<bool>("", "DrawNet"); }

	std::string get_decl_text_height()		{ return gwin::read_config<std::string>("", "DeclTextHeight"); }
	std::string get_ra_text_height()		{ return gwin::read_config<std::string>("", "RATextHeight"); }

	bool draw_stars()						{ return gwin::read_config<bool>("", "DrawStars"); }
	std::string get_star_magnitude_limit()	{ return gwin::read_config<std::string>("", "StarMagnitudeLimit"); }
	std::string get_min_diam_star()			{ return gwin::read_config<std::string>("", "MinStarDiameter"); }
	std::string get_max_diam_star()			{ return gwin::read_config<std::string>("", "MaxStarDiameter"); }

	diam_function_type_e get_star_diam_func_type()
	{
		std::string par = gwin::read_config<std::string>("", "StarDiameterType");

		// LINEAR		- Linear
		// ATAN			- Arcus Tangens
		// EXPONENTIAL	- Expenential

		if (par == "LINEAR")			return dft_linear;
		if (par == "ATAN")				return dft_atan;
		// if (par == "EXPONENTIAL")		return dft_exponential;

		return dft_linear;
	}

	bool draw_variable_ring()				{ return gwin::read_config<bool>("", "DrawVariableStarRing"); }
	bool draw_multiple_sign()				{ return gwin::read_config<bool>("", "DrawMultipleStarSign"); }
	bool draw_star_frame()					{ return gwin::read_config<bool>("", "DrawStarFrame"); }
	std::string get_star_frame_width()		{ return gwin::read_config<std::string>("", "StarFrameWidth"); }
	bool print_star_names()					{ return gwin::read_config<bool>("", "PrintStarNames"); }
	std::string get_star_name_height()		{ return gwin::read_config<std::string>("", "StarNameHeight"); }
	std::string get_star_name_mag_limit()	{ return gwin::read_config<std::string>("", "MinStarNameMagnitude"); }
	bool print_bayer_dsgn()					{ return gwin::read_config<bool>("", "PrintBayerDesignations"); }
	bool optimize_bayer_dsgn()				{ return gwin::read_config<bool>("", "OptimizeBayerDesignations"); }
	bool print_flamsteed_dsgn()				{ return gwin::read_config<bool>("", "PrintFlamsteedDesignations"); }

	var_dsgn_type_e get_var_dsgn_type()
	{
		std::string par = gwin::read_config<std::string>("", "");

		// NONE			- Don't print variable designations
		// ALL			- Print all variable designations
		// NOT_V_NUM	- Print only the designations that are not in the Vxxx format

		if (par == "NONE")			return vdt_none;
		if (par == "ALL")			return vdt_all;
		if (par == "NOT_V_NUM")		return vdt_not_v_num;

		return vdt_none;
	}

	std::string get_star_dsgn_height()		{ return gwin::read_config<std::string>("", "StarDesignationHeight"); }
	std::string get_star_dsgn_mag_limit()	{ return gwin::read_config<std::string>("", "MinStarDesignationMagnitude"); }


	bool draw_const_bounds()						{ return gwin::read_config<bool>("", "DrawConstellationBounds"); }
	size_t get_const_bounds_style()
	{
		return _get_line_style("ConstellationBoundStyle");
	}

	constellation_bound_e get_const_bound_type()
	{
		std::string par = gwin::read_config<std::string>("", "ConstellationBoundType");

		if (par == "FULL")			return cb_full;
		if (par == "CORNER")		return cb_corner;

		return cb_full;
	}

	std::string get_min_angle_length_factor()		{ return gwin::read_config<std::string>("", "MinAngleLengthFactor"); }
	std::string get_max_angle_length_factor()		{ return gwin::read_config<std::string>("", "MaxAngleLengthFactor"); }
	std::string get_const_bound_angle()				{ return gwin::read_config<std::string>("", "ConstellationBoundAngle"); }
	bool draw_asterism()							{ return gwin::read_config<bool>("", "DrawAsterism"); }
	std::string get_asterism_file_name()			{ return gwin::read_config<std::string>("", "AsterismFileName"); }

	line_style_e get_asterism_style()
	{
		return _get_line_style("AsterismLineStyle");
	}

	ngc_type_e draw_ngc_type()
	{
		std::string par = gwin::read_config<std::string>("", "DrawNGCType");

		if (par == "ALL")				return ngc_all;
		if (par == "MESSIER_ONLY")		return ngc_messier_only;
		if (par == "NONE")				return ngc_none;

		return ngc_none;
	}

	std::string get_ngc_names_height()				{ return gwin::read_config<std::string>("", "NGCNamesHeight"); }
	
	ngc_names_type_e get_ngc_names_type()
	{
		std::string par = gwin::read_config<std::string>("", "NGCNamesType");

		// NONE:    Don't print NGC designations
		// NGC:     NGC number only (ie 6531)
		// MESSIER: Messier only (ie M31)
		// BOTH:    Messier (if it exists) then NGC

		if (par == "NONE")				return ngcname_none;
		if (par == "NGC")				return ngcname_ngc;
		if (par == "MESSIER")			return ngcname_messier;
		if (par == "BOTH")				return ngcname_both;

		return ngcname_messier;
	}

	bool draw_planetary_nebula()					{ return gwin::read_config<bool>("", "DrawPlanetaryNebula"); }
	std::string get_mag_limit_pn()					{ return gwin::read_config<std::string>("", "PlanetaryNebulaMagnitudeLimit"); }
	std::string get_min_diam_pn()					{ return gwin::read_config<std::string>("", "MinPlanetaryNebulaDiameter"); }   
	std::string get_max_diam_pn()					{ return gwin::read_config<std::string>("", "MaxPlanetaryNebulaDiameter"); }   

	bool draw_nebula()								{ return gwin::read_config<bool>("", "DrawNebula"); }
	std::string get_mag_limit_n()					{ return gwin::read_config<std::string>("", "NebulaMagnitudeLimit"); }
	std::string get_min_diam_n()					{ return gwin::read_config<std::string>("", "MinNebulaDiameter"); }
	std::string get_max_diam_n()					{ return gwin::read_config<std::string>("", "MaxNebulaDiameter"); }

	bool draw_galaxies()							{ return gwin::read_config<bool>("", "DrawGalaxies"); }                
	std::string get_mag_limit_g()					{ return gwin::read_config<std::string>("", "GalaxyMagnitudeLimit"); } 
	std::string get_min_diam_g()					{ return gwin::read_config<std::string>("", "MinGalaxyDiameter"); }    
	std::string get_max_diam_g()					{ return gwin::read_config<std::string>("", "MaxGalaxyDiameter"); }    

	bool draw_open_clusters()						{ return gwin::read_config<bool>("", "DrawOpenClusters"); }                
	std::string get_mag_limit_oc()					{ return gwin::read_config<std::string>("", "OpenClusterMagnitudeLimit"); }
	std::string get_min_diam_oc()					{ return gwin::read_config<std::string>("", "MinOpenClusterDiameter"); }   
	std::string get_max_diam_oc()					{ return gwin::read_config<std::string>("", "MaxOpenClusterDiameter"); }   

	bool draw_globular_clusters()					{ return gwin::read_config<bool>("", "DrawGlobularClusters"); }
	std::string get_mag_limit_gc()					{ return gwin::read_config<std::string>("", "GlobClusterMagnitudeLimit"); }
	std::string get_min_diam_gc()					{ return gwin::read_config<std::string>("", "MinGlobClusterDiameter"); }
	std::string get_max_diam_gc()					{ return gwin::read_config<std::string>("", "MaxGlobClusterDiameter"); }
};

config_t* conf = 0;

struct run_thread_t
{
	std::string		config_file;

	gwin::mutex		sync_pn;
	std::string		process_name;

	gwin::mutex		sync_log;
	std::string		log;

	std::string get_log()
	{
		gwin::lock l(sync_log);
		std::string ret_val = log;
		return ret_val;
	}

	std::string get_process_name()
	{
		gwin::lock l(sync_pn);
		std::string ret_val = process_name;
		return ret_val;
	}

	void operator () ()
	{
        // If NT do security stuff
        SECURITY_DESCRIPTOR sd;
        SECURITY_ATTRIBUTES sa;
        LPSECURITY_ATTRIBUTES lpsa = NULL;
		gwin::windows_version_t win_ver = gwin::get_win_version();
		if (win_ver == gwin::win_nt
				|| win_ver == gwin::win2000
				|| win_ver == gwin::win_xp)
		{
			::InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
			::SetSecurityDescriptorDacl(&sd, true, NULL, false);
			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			sa.bInheritHandle = true;
			sa.lpSecurityDescriptor = &sd;
			lpsa = &sa;
		}

        // Create the Pipe and get r/w handles
        HANDLE hReadPipe = 0;
        HANDLE hWritePipe = 0;
		size_t result = ::CreatePipe(&hReadPipe, &hWritePipe, lpsa, 2500000);
        assert(hWritePipe);

		// make a window title
		std::string window_title;
		{
			// lock the mutex
			gwin::lock l(sync_pn);

			// stringstream sstr;
			// sstr << "stars_gui child process [" + ... + "]";
			window_title = process_name = "stars_gui child process";
		}

        // initialize STARTUPINFO struct
        STARTUPINFO si;
        memset(&si, 0, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
		si.lpTitle = (char*) window_title.c_str();
        si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
        si.wShowWindow = SW_HIDE;
        si.hStdOutput = hWritePipe;
        si.hStdError = hWritePipe;

        PROCESS_INFORMATION pi;

        // run the stars.exe process
        result = ::CreateProcess(NULL, "stars stars_gui.cfg", NULL, NULL, TRUE, 0, 0, 0, &si, &pi);

        // read from the pipe and put in log
        assert(hReadPipe);
        DWORD BytesRead;
        char dest[4000];
		do {
        	memset(dest, 0, 4000);
            result = ::ReadFile(hReadPipe, &dest, sizeof(dest) - 1, &BytesRead, NULL);
			{
				gwin::lock l(sync_log);
				log += dest;
			}
        } while (result != 0);

        // Process cleanup
		::CloseHandle(hReadPipe);
    	::CloseHandle(hWritePipe);
    	::CloseHandle(pi.hProcess);
	}
};

// ******************************************
// log dialog

class log_dialog_controller: public gwin::window
{
public:
	struct init_t
	{
		std::string		project_name;
		std::string		eps_file_name;
	};

private:

	gwin::list_box<log_dialog_controller>	_lst_log;
	gwin::button<log_dialog_controller>		_btn_cancel;

	init_t&									_init;
	gwin::timer								_timer;
	std::string								_prev_log;
	gwin::thread<run_thread_t>				_thread;

	std::string								_process_name;
	gwin::stopwatch							_elapsed_time;

public:
	enum {resource_id = IDD_LOG};

	log_dialog_controller(gwin::window& w, init_t& i)
	:	gwin::window(w),
		_init(i),
		_lst_log(IDC_LST_LOG, *this),
		_btn_cancel(IDCANCEL, *this)
	{}

	std::string get_window_title()
	{
		return "Generating EPS file: " + _init.eps_file_name + "   Elapsed time: " + _elapsed_time.get_time();
	}

	bool operator () (gwin::wm_initdialog& msg)
	{
		// center the dialog window
		gwin::center_window(*this);

		// setup the event handlers
		_btn_cancel.on_click = gwin::closure(this, on_cancel);

		// start the pipe
		_thread.functor.config_file = _init.project_name;
		_thread.run();

		// set the dialog caption
		gwin::set_text(*this, get_window_title());

		// start the timer
		_timer.start(*this, 500);

		// start the clock
		_elapsed_time.start();

		return false;
	}

	void on_cancel()
	{
		// get the process name
		std::string process_name = _thread.functor.get_process_name();

		// try to stop the process
		gwin::window console(::FindWindow(0, process_name.c_str()));
		gwin::close_window(console);
		gwin::end_dialog(*this);
	}

	bool operator () (gwin::wm_timer& msg)
	{
		gwin::set_text(*this, get_window_title());

		// lock the handle
		std::string log = _thread.functor.get_log();

		if (_prev_log != log)
		{
			// finished?
			if (log.find("<!finished!>") != std::string::npos)
			{
				_timer.stop();

				gwin::message_box(*this, "Map generation complete", "Complete", MB_OK | MB_ICONINFORMATION);
				gwin::end_dialog(*this);
				return true;
			} else {
				const std::string command = "<!exception!>";
				size_t index = log.find(command);
				if (index != std::string::npos)
				{
					_timer.stop();
					gwin::message_box(*this, log.c_str() + index + command.length(), "Exception", MB_OK | MB_ICONERROR);
					gwin::end_dialog(*this);
					return true;
				}
			}

			// convert the string into lines
			std::vector<std::string> lines;
			std::string::iterator str_iter = log.begin();
			std::string current_line;
			std::string::iterator curr_line_iter = current_line.begin();
			while (str_iter != log.end())
			{
				switch (*str_iter)
				{
				case '\n':
					lines.push_back(current_line);
					current_line.clear();
					curr_line_iter = current_line.end();
					break;
				case '\r':
					curr_line_iter = current_line.begin();
					break;
				default:
					if (curr_line_iter == current_line.end())
					{
						current_line.push_back(*str_iter);
						curr_line_iter = current_line.end();
					} else {
						*curr_line_iter = *str_iter;
						++curr_line_iter;
					}

					break;
				}

				++str_iter;
			}

			if (!current_line.empty())
				lines.push_back(current_line);

			// use only the last 50 lines
			if (lines.size() > 50)
				lines.erase(lines.begin(), lines.end() - 50);

			_lst_log.clear();
			std::for_each(lines.begin(), lines.end(),
						std::bind1st(
							gwin::add_item_t<gwin::list_box<log_dialog_controller> >(),
							_lst_log));

			// scroll down to the last line
			_lst_log.select_item(lines.size() - 1);

			_prev_log = log;
		}

		return true;
	}

	bool operator () (gwin::wm_close& msg)
	{
		_timer.stop();
		return true;
	}

	// the default message handler
	template <class DefaultMessage>
	bool operator () (const DefaultMessage&)
	{
		return false;
	}
};

typedef gwin::dialog<log_dialog_controller, log_dialog_controller::init_t> log_dialog;

// ****************************
// the main dialog

class output_page_controller: public gwin::window
{
private:

	gwin::check_box<output_page_controller>		_chk_plot_text_frame;
	gwin::combo_box<output_page_controller>		_cmb_debug_region;
	gwin::edit_box<output_page_controller>		_edt_output_file_name;
	gwin::edit_box<output_page_controller>		_edt_map_radius;
	gwin::edit_box<output_page_controller>		_edt_map_border;
	gwin::edit_box<output_page_controller>		_edt_scale_factor;

public:
	enum {resource_id = IDD_PAGE_OUTPUT};

	output_page_controller(gwin::window& w)
	:	gwin::window(w),
		_chk_plot_text_frame(IDC_CHK_PLOT_TEXT_FRAME, *this),
		_cmb_debug_region(IDC_CMB_DEBUG_REGION, *this),
		_edt_output_file_name(IDC_EDT_OUTPUT_FILE_NAME, *this),
		_edt_map_radius(IDC_EDT_MAP_RADIUS, *this),
		_edt_map_border(IDC_EDT_MAP_BORDER, *this),
		_edt_scale_factor(IDC_EDT_SCALE_FACTOR, *this)
	{}

	std::string get_page_title() const	{ return "Output"; }
	void on_page_changed()				{}

	bool operator () (gwin::wm_initdialog& msg)
	{
		_cmb_debug_region.add_item("All");
		_cmb_debug_region.add_item("Orion");
		_cmb_debug_region.add_item("Orion's belt");

		_chk_plot_text_frame.set_state(conf->draw_text_frame());

		_cmb_debug_region.select_item(conf->get_debug_region());

		gwin::set_text(_edt_output_file_name, conf->get_output_file_name());
		gwin::set_text(_edt_map_radius,	conf->get_map_radius());
		gwin::set_text(_edt_map_border, conf->get_spare_space());
		gwin::set_text(_edt_scale_factor, conf->get_scale_factor());

		return false;
	}

	std::string get_eps_file_name()
	{
		return gwin::get_text(_edt_output_file_name);
	}

	void save()
	{
		gwin::write_config("", "DBGTextFrame", _chk_plot_text_frame.is_checked());

		switch (_cmb_debug_region.get_selected())
		{
		case 0: gwin::write_config("", "DBGRegion", std::string("ALL"));			break;
		case 1:	gwin::write_config("", "DBGRegion", std::string("ORION"));			break;
		case 2:	gwin::write_config("", "DBGRegion", std::string("ORIONS_BELT"));	break;
		}

		gwin::write_config("", "OutputFileName", gwin::get_text(_edt_output_file_name));
		gwin::write_config("", "ScaleFactor", atof(gwin::get_text(_edt_scale_factor).c_str()));
		gwin::write_config("", "MapRadius", atof(gwin::get_text(_edt_map_radius).c_str()));
		gwin::write_config("", "SpareSpace", atof(gwin::get_text(_edt_map_border).c_str()));
	}																	  

	// the default message handler
	template <class DefaultMessage>
	bool operator () (const DefaultMessage&)
	{
		return false;
	}
};

typedef gwin::dialog<output_page_controller> output_page;


// ******************************************
// general

class general_page_controller: public gwin::window
{
private:

	gwin::radio_button<general_page_controller>		_rdb_north;
	gwin::radio_button<general_page_controller>		_rdb_south;
	gwin::check_box<general_page_controller>		_chk_plot_ecliptic;
	gwin::check_box<general_page_controller>		_chk_plot_gal_equator;
	gwin::edit_box<general_page_controller>			_edt_dots_frequency;
	gwin::edit_box<general_page_controller>			_edt_max_pole_distance;
	gwin::combo_box<general_page_controller>		_cmb_plot_milky_way;
	gwin::check_box<general_page_controller>		_chk_ignore_pleiade;
	gwin::check_box<general_page_controller>		_chk_plot_meteor_radiants;
	gwin::edit_box<general_page_controller>			_edt_radiants_text_size;
	gwin::combo_box<general_page_controller>		_cmb_titles_font;
	gwin::check_box<general_page_controller>		_chk_plot_fixed_text;
	gwin::edit_box<general_page_controller>			_edt_fixed_text_file;
	gwin::edit_box<general_page_controller>			_edt_text_distance;
	gwin::check_box<general_page_controller>		_chk_plot_net;
	gwin::edit_box<general_page_controller>			_edt_declination_text_height;
	gwin::edit_box<general_page_controller>			_edt_right_ascention_text_height;


public:
	enum {resource_id = IDD_PAGE_GENERAL};

	general_page_controller(gwin::window& w)
	:	gwin::window(w),
		_rdb_north(IDC_RDB_NORTH, *this),
		_rdb_south(IDC_RDB_SOUTH, *this),
		_chk_plot_ecliptic(IDC_CHK_PLOT_ECLIPTIC, *this),
		_chk_plot_gal_equator(IDC_CHK_PLOT_GAL_EQUATOR, *this),
		_edt_dots_frequency(IDC_EDT_DOTS_FREQUENCY, *this),
		_edt_max_pole_distance(IDC_EDT_MAX_POLE_DISTANCE, *this),
		_cmb_plot_milky_way(IDC_CMB_PLOT_MILKY_WAY, *this),
		_chk_ignore_pleiade(IDC_CHK_IGNORE_PLEIADE, *this),
		_chk_plot_meteor_radiants(IDC_CHK_PLOT_METEOR_RADIANTS, *this),
		_edt_radiants_text_size(IDC_EDT_RADIANTS_TEXT_SIZE, *this),
		_cmb_titles_font(IDC_CMB_TITLES_FONT, *this),
		_chk_plot_fixed_text(IDC_CHK_PLOT_FIXED_TEXT, *this),
		_edt_fixed_text_file(IDC_EDT_FIXED_TEXT_FILE, *this),
		_edt_text_distance(IDC_EDT_TEXT_DISTANCE, *this),
		_chk_plot_net(IDC_CHK_PLOT_NET, *this),
		_edt_declination_text_height(IDC_EDT_DECLINATION_TEXT_HEIGHT, *this),
		_edt_right_ascention_text_height(IDC_EDT_RIGHT_ASCENTION_TEXT_HEIGHT, *this)
	{}

	std::string get_page_title() const	{ return "General"; }
	void on_page_changed()				{}

	void operator () (gwin::wm_initdialog& msg)
	{
		_chk_plot_meteor_radiants.on_click	= gwin::closure(this, on_update_states);
		_chk_plot_fixed_text.on_click		= gwin::closure(this, on_update_states);
		_chk_plot_net.on_click				= gwin::closure(this, on_update_states);

		// fill the list boxes
		_cmb_titles_font.add_item("Times");
		_cmb_titles_font.add_item("Helvetica");

		_cmb_plot_milky_way.add_item("Don't plot");
		_cmb_plot_milky_way.add_item("Fill");
		_cmb_plot_milky_way.add_item("Lines");
		_cmb_plot_milky_way.add_item("Dots");

		// load the controls
		if (conf->is_north())
			_rdb_north.check();
		else
			_rdb_south.check();

		_chk_plot_ecliptic.set_state(conf->draw_ecliptic());
		_chk_plot_gal_equator.set_state(conf->draw_galactic_equator());
		gwin::set_text(_edt_dots_frequency, conf->get_circle_dots_frequency());
		gwin::set_text(_edt_max_pole_distance, conf->get_max_pole_distance());
		if (!conf->draw_milky_way())
		{
			_cmb_plot_milky_way.select_item(0);
		} else {
			switch (conf->get_milky_way_line_style())
			{
			case config_t::mws_fill:
				_cmb_plot_milky_way.select_item(1);
				break;
			case config_t::mws_solid:
				_cmb_plot_milky_way.select_item(2);
				break;
			case config_t::mws_dots:
				_cmb_plot_milky_way.select_item(3);
				break;
			}
		}

		_chk_ignore_pleiade.set_state(conf->ignore_pleiades());
		_chk_plot_meteor_radiants.set_state(conf->draw_radiants());
		gwin::set_text(_edt_radiants_text_size, conf->get_radiants_text_size());
		if (conf->get_names_font() == "Times")
			_cmb_titles_font.select_item(0);
		else
			_cmb_titles_font.select_item(1);
		_chk_plot_fixed_text.set_state(conf->print_fixed_text());
		gwin::set_text(_edt_fixed_text_file, conf->get_fixed_text_file_name());
		gwin::set_text(_edt_text_distance, conf->get_text_distance());
		_chk_plot_net.set_state(conf->draw_net());
		gwin::set_text(_edt_declination_text_height, conf->get_decl_text_height());
		gwin::set_text(_edt_right_ascention_text_height, conf->get_ra_text_height());

		on_update_states();
	}

	void on_update_states()
	{
		if (_chk_plot_fixed_text.is_checked())
			gwin::enable(_edt_fixed_text_file);
		else
			gwin::disable(_edt_fixed_text_file);

		if (_chk_plot_net.is_checked())
		{
			gwin::enable(_edt_declination_text_height);
			gwin::enable(_edt_right_ascention_text_height);
		} else {
			gwin::disable(_edt_declination_text_height);
			gwin::disable(_edt_right_ascention_text_height);
		}

		if (_chk_plot_meteor_radiants.is_checked())
			gwin::enable(_edt_radiants_text_size);
		else
			gwin::disable(_edt_radiants_text_size);
	}

	void save()
	{
		gwin::write_config("", "IsNorth", _rdb_north.is_checked());
		gwin::write_config("", "DrawEcliptic", _chk_plot_ecliptic.is_checked());
		gwin::write_config("", "DrawGalacticEquator", _chk_plot_gal_equator.is_checked());
		gwin::write_config("", "CirclesDotsFrequency", atof(gwin::get_text(_edt_dots_frequency).c_str()));
		gwin::write_config("", "MaxPoleDistance", atof(gwin::get_text(_edt_max_pole_distance).c_str()));
		std::string cmb_text = gwin::get_text(_cmb_plot_milky_way);
		if (cmb_text == "Don't plot")
		{
			gwin::write_config("", "DrawMilkyWay", false);
		} else {
			gwin::write_config("", "DrawMilkyWay", true);

			if (cmb_text == "Fill")
			{
				gwin::write_config("", "MilkyWayStyle", std::string("FILL"));
			} else if (cmb_text == "Lines") {
				gwin::write_config("", "MilkyWayStyle", std::string("SOLID"));
			} else if (cmb_text == "Dots") {
				gwin::write_config("", "MilkyWayStyle", std::string("DOTS"));
			}
		}

		gwin::write_config("", "IgnorePleiades", _chk_ignore_pleiade.is_checked());
		gwin::write_config("", "DrawRadiants", _chk_plot_meteor_radiants.is_checked());
		gwin::write_config("", "RadiantsDesignationSize", atof(gwin::get_text(_edt_radiants_text_size).c_str()));
		gwin::write_config("", "NamesFont", gwin::get_text(_cmb_titles_font));
		gwin::write_config("", "PrintFixedText", _chk_plot_fixed_text.is_checked());
		gwin::write_config("", "FixedTextFile", gwin::get_text(_edt_fixed_text_file));
		gwin::write_config("", "TextDistance", atof(gwin::get_text(_edt_text_distance).c_str()));
		gwin::write_config("", "DrawNet", _chk_plot_net.is_checked());
		gwin::write_config("", "DeclTextHeight", atof(gwin::get_text(_edt_declination_text_height).c_str()));
		gwin::write_config("", "RATextHeight", atof(gwin::get_text(_edt_right_ascention_text_height).c_str()));
	}

	// the default message handler
	template <class DefaultMessage>
	bool operator () (const DefaultMessage&)
	{
		return false;
	}
};

typedef gwin::dialog<general_page_controller> general_page;


// ******************************************
// stars

class stars_page_controller: public gwin::window
{
private:

	gwin::check_box<stars_page_controller>		_chk_plot_stars;
	gwin::edit_box<stars_page_controller>		_edt_magnitude_limit;
	gwin::edit_box<stars_page_controller>		_edt_min_star_diameter;
	gwin::edit_box<stars_page_controller>		_edt_max_star_diameter;
	gwin::combo_box<stars_page_controller>		_cmb_diameter_function;
	gwin::check_box<stars_page_controller>		_chk_draw_variable_ring;
	gwin::check_box<stars_page_controller>		_chk_draw_multiple_sign;
	gwin::check_box<stars_page_controller>		_chk_draw_star_frame;
	gwin::edit_box<stars_page_controller>		_edt_star_frame_width;
	gwin::check_box<stars_page_controller>		_chk_plot_star_names;
	gwin::edit_box<stars_page_controller>		_edt_star_name_height;
	gwin::edit_box<stars_page_controller>		_edt_min_name_magnitude;
	gwin::check_box<stars_page_controller>		_chk_plot_bayer_designations;
	gwin::check_box<stars_page_controller>		_chk_optimize_bayer_designations;
	gwin::check_box<stars_page_controller>		_chk_plot_flamsteed_designations;
	gwin::combo_box<stars_page_controller>		_cmb_variable_designations;
	gwin::edit_box<stars_page_controller>		_edt_designation_text_height;
	gwin::edit_box<stars_page_controller>		_edt_min_magnitude_for_designation;

public:
	enum {resource_id = IDD_PAGE_STARS};

	stars_page_controller(gwin::window& w)
	:	gwin::window(w),
		_chk_plot_stars(IDC_CHK_PLOT_STARS, *this), 
		_edt_magnitude_limit(IDC_EDT_MAGNITUDE_LIMIT, *this),
		_edt_min_star_diameter(IDC_EDT_MIN_STAR_DIAMETER, *this),
		_edt_max_star_diameter(IDC_EDT_MAX_STAR_DIAMETER, *this),
		_cmb_diameter_function(IDC_CMB_DIAMETER_FUNCTION, *this),
		_chk_draw_variable_ring(IDC_CHK_DRAW_VARIABLE_RING, *this),
		_chk_draw_multiple_sign(IDC_CHK_DRAW_MULTIPLE_SIGN, *this),
		_chk_draw_star_frame(IDC_CHK_DRAW_STAR_FRAME, *this),
		_edt_star_frame_width(IDC_EDT_STAR_FRAME_WIDTH, *this),
		_chk_plot_star_names(IDC_CHK_PLOT_STAR_NAMES, *this),
		_edt_star_name_height(IDC_EDT_STAR_NAME_HEIGHT, *this),
		_edt_min_name_magnitude(IDC_EDT_MIN_NAME_MAGNITUDE, *this),
		_chk_plot_bayer_designations(IDC_CHK_PLOT_BAYER_DESIGNATIONS, *this),
		_chk_optimize_bayer_designations(IDC_CHK_OPTIMIZE_BAYER_DESIGNATIONS, *this),
		_chk_plot_flamsteed_designations(IDC_CHK_PLOT_FLAMSTEED_DESIGNATIONS, *this),
		_cmb_variable_designations(IDC_CMB_VARIABLE_DESIGNATIONS, *this),
		_edt_designation_text_height(IDC_EDT_DESIGNATION_TEXT_HEIGHT, *this),
		_edt_min_magnitude_for_designation(IDC_EDT_MIN_MAGNITUDE_FOR_DESIGNATION, *this)
	{}

	std::string get_page_title() const	{ return "Stars"; }
	void on_page_changed()				{}

	bool operator () (gwin::wm_initdialog& msg)
	{
		_cmb_diameter_function.add_item("f(x)=x");
		_cmb_diameter_function.add_item("f(x)=atan(x)");
		//_cmb_diameter_function.add_item("f(x)=exp(x)");

		_cmb_variable_designations.add_item("None");
		_cmb_variable_designations.add_item("All");
		_cmb_variable_designations.add_item("Not Vxxx");

		_chk_plot_stars.on_click = gwin::closure(this, on_update_states);
		_chk_draw_star_frame.on_click = gwin::closure(this, on_update_states);
		_chk_plot_star_names.on_click = gwin::closure(this, on_update_states);
		_chk_plot_bayer_designations.on_click = gwin::closure(this, on_update_states);

		_chk_plot_stars.set_state(conf->draw_stars());
		gwin::set_text(_edt_magnitude_limit, conf->get_star_magnitude_limit());
		gwin::set_text(_edt_min_star_diameter, conf->get_min_diam_star());
		gwin::set_text(_edt_max_star_diameter, conf->get_max_diam_star());
		_cmb_diameter_function.select_item(conf->get_star_diam_func_type());
		_chk_draw_variable_ring.set_state(conf->draw_variable_ring());
		_chk_draw_multiple_sign.set_state(conf->draw_multiple_sign());
		_chk_draw_star_frame.set_state(conf->draw_star_frame());
		gwin::set_text(_edt_star_frame_width, conf->get_star_frame_width());
		_chk_plot_star_names.set_state(conf->print_star_names());
		gwin::set_text(_edt_star_name_height, conf->get_star_name_height());
		gwin::set_text(_edt_min_name_magnitude, conf->get_star_name_mag_limit());
		_chk_plot_bayer_designations.set_state(conf->print_bayer_dsgn());
		_chk_optimize_bayer_designations.set_state(conf->optimize_bayer_dsgn());
		_chk_plot_flamsteed_designations.set_state(conf->print_flamsteed_dsgn());

		switch (conf->get_var_dsgn_type())
		{
		case config_t::vdt_none:
			_cmb_variable_designations.select_item(0);
			break;
		case config_t::vdt_all:
			_cmb_variable_designations.select_item(1);
			break;
		case config_t::vdt_not_v_num:
			_cmb_variable_designations.select_item(2);
			break;
		}

		gwin::set_text(_edt_designation_text_height, conf->get_star_dsgn_height());
		gwin::set_text(_edt_min_magnitude_for_designation, conf->get_star_dsgn_mag_limit());

		on_update_states();

		return false;
	}

	void on_update_states()
	{
		if (_chk_plot_stars.is_checked())
		{
			gwin::enable(_edt_magnitude_limit);
			gwin::enable(_edt_min_star_diameter);
			gwin::enable(_edt_max_star_diameter);
			gwin::enable(_cmb_diameter_function);
			gwin::enable(_chk_draw_variable_ring);
			gwin::enable(_chk_draw_multiple_sign);
			gwin::enable(_chk_draw_star_frame);
			gwin::enable(_chk_plot_star_names);
			gwin::enable(_chk_plot_bayer_designations);
			gwin::enable(_chk_optimize_bayer_designations);
			gwin::enable(_chk_plot_flamsteed_designations);
			gwin::enable(_cmb_variable_designations);
			gwin::enable(_edt_designation_text_height);
			gwin::enable(_edt_min_magnitude_for_designation);

			if (_chk_draw_star_frame.is_checked())
				gwin::enable(_edt_star_frame_width);
			else
				gwin::disable(_edt_star_frame_width);

			if (_chk_plot_star_names.is_checked())
			{
				gwin::enable(_edt_star_name_height);
				gwin::enable(_edt_min_name_magnitude);
			} else {
				gwin::disable(_edt_star_name_height);
				gwin::disable(_edt_min_name_magnitude);
			}

			if (_chk_plot_bayer_designations.is_checked())
				gwin::enable(_chk_optimize_bayer_designations);
			else
				gwin::disable(_chk_optimize_bayer_designations);

		} else {
			gwin::disable(_edt_magnitude_limit);
			gwin::disable(_edt_min_star_diameter);
			gwin::disable(_edt_max_star_diameter);
			gwin::disable(_cmb_diameter_function);
			gwin::disable(_chk_draw_variable_ring);
			gwin::disable(_chk_draw_multiple_sign);
			gwin::disable(_chk_draw_star_frame);
			gwin::disable(_edt_star_frame_width);
			gwin::disable(_chk_plot_star_names);
			gwin::disable(_edt_star_name_height);
			gwin::disable(_edt_min_name_magnitude);
			gwin::disable(_chk_plot_bayer_designations);
			gwin::disable(_chk_optimize_bayer_designations);
			gwin::disable(_chk_plot_flamsteed_designations);
			gwin::disable(_cmb_variable_designations);
			gwin::disable(_edt_designation_text_height);
			gwin::disable(_edt_min_magnitude_for_designation);
		}
	}

	void save()
	{
		gwin::write_config("", "DrawStars", _chk_plot_stars.is_checked());
		gwin::write_config("", "StarMagnitudeLimit", atof(gwin::get_text(_edt_magnitude_limit).c_str()));
		gwin::write_config("", "MaxStarDiameter", atof(gwin::get_text(_edt_max_star_diameter).c_str()));
		gwin::write_config("", "MinStarDiameter", atof(gwin::get_text(_edt_min_star_diameter).c_str()));
		switch (_cmb_diameter_function.get_selected())
		{
		case 0:	gwin::write_config("", "StarDiameterType", std::string("LINEAR"));		break;
		case 1:	gwin::write_config("", "StarDiameterType", std::string("ATAN"));		break;
		}
		gwin::write_config("", "DrawVariableStarRing", _chk_draw_variable_ring.is_checked());
		gwin::write_config("", "DrawMultipleStarSign", _chk_draw_multiple_sign.is_checked());
		gwin::write_config("", "DrawStarFrame", _chk_draw_star_frame.is_checked());
		gwin::write_config("", "StarFrameWidth", atof(gwin::get_text(_edt_star_frame_width).c_str()));
		gwin::write_config("", "PrintStarNames", _chk_plot_star_names.is_checked());
		gwin::write_config("", "StarNameHeight", atof(gwin::get_text(_edt_star_name_height).c_str()));
		gwin::write_config("", "MinStarNameMagnitude", atof(gwin::get_text(_edt_min_name_magnitude).c_str()));
		gwin::write_config("", "PrintBayerDesignations", _chk_plot_bayer_designations.is_checked());
		gwin::write_config("", "OptimizeBayerDesignations", _chk_optimize_bayer_designations.is_checked());
		gwin::write_config("", "PrintFlamsteedDesignations", _chk_plot_flamsteed_designations.is_checked());
		switch (_cmb_variable_designations.get_selected())
		{
		case 0:	gwin::write_config("", "PrintVariableDesignations", std::string("NONE"));		break;
		case 1:	gwin::write_config("", "PrintVariableDesignations", std::string("ALL"));		break;
		case 2:	gwin::write_config("", "PrintVariableDesignations", std::string("NOT_V_NUM"));	break;
		}
		gwin::write_config("", "StarDesignationHeight", atof(gwin::get_text(_edt_designation_text_height).c_str()));
		gwin::write_config("", "MinStarDesignationMagnitude", atof(gwin::get_text(_edt_min_magnitude_for_designation).c_str()));
	}

	// the default message handler
	template <class DefaultMessage>
	bool operator () (const DefaultMessage&)
	{
		return false;
	}
};

typedef gwin::dialog<stars_page_controller> stars_page;

// ******************************************
// constellations

class constellations_page_controller: public gwin::window
{
private:

	gwin::check_box<constellations_page_controller>		_chk_plot_bounds;
	gwin::radio_button<constellations_page_controller>	_rdb_plot_line;
	gwin::radio_button<constellations_page_controller>	_rdb_plot_dots;
	gwin::radio_button<constellations_page_controller>	_rdb_corner;
	gwin::radio_button<constellations_page_controller>	_rdb_full_line;
	gwin::edit_box<constellations_page_controller>		_edt_factor_min;
	gwin::edit_box<constellations_page_controller>		_edt_factor_max;
	gwin::edit_box<constellations_page_controller>		_edt_angle;
	gwin::check_box<constellations_page_controller>		_chk_connection_plot;
	gwin::edit_box<constellations_page_controller>		_edt_connection_from_file;
	gwin::radio_button<constellations_page_controller>	_rdb_connection_solid;
	gwin::radio_button<constellations_page_controller>	_rdb_connection_dots;

public:
	enum {resource_id = IDD_PAGE_CONSTELLATIONS};

	constellations_page_controller(gwin::window& w)
	:	gwin::window(w),
		_chk_plot_bounds(IDC_CHK_BOUNDS_PLOT, *this),
		_rdb_plot_line(IDC_RDB_BOUNDS_PLOT_LINES, *this),
		_rdb_plot_dots(IDC_RDB_BOUNDS_PLOT_DOTS, *this),
		_rdb_corner(IDC_RDB_BOUNDS_CORNER, *this),
		_rdb_full_line(IDC_RDB_BOUNDS_FULL_LINE, *this),
		_edt_factor_min(IDC_EDT_BOUNDS_FACTOR_MIN, *this),
		_edt_factor_max(IDC_EDT_BOUNDS_FACTOR_MAX, *this),
		_edt_angle(IDC_EDT_BOUNDS_ANGLE, *this),
		_chk_connection_plot(IDC_CHK_CONNECTION_PLOT, *this),
		_edt_connection_from_file(IDC_EDT_CONNECTION_FROM_FILE, *this),
		_rdb_connection_solid(IDC_RDB_CONNECTION_SOLID, *this),
		_rdb_connection_dots(IDC_RDB_CONNECTION_DOTS, *this)
	{}

	std::string get_page_title() const	{ return "Constellations"; }
	void on_page_changed()				{}

	void operator () (gwin::wm_initdialog& msg)
	{
		_chk_plot_bounds.on_click		= gwin::closure(this, on_update_states);
		_rdb_plot_line.on_click			= gwin::closure(this, on_update_states);
		_rdb_plot_dots.on_click			= gwin::closure(this, on_update_states);
		_rdb_corner.on_click			= gwin::closure(this, on_update_states);
		_rdb_full_line.on_click			= gwin::closure(this, on_update_states);
		_chk_connection_plot.on_click	= gwin::closure(this, on_update_states);

		_chk_plot_bounds.set_state(conf->draw_const_bounds());
		_rdb_plot_line.set_state(conf->get_const_bounds_style() == config_t::ls_solid);
		_rdb_plot_dots.set_state(conf->get_const_bounds_style() == config_t::ls_dots);
		_rdb_corner.set_state(conf->get_const_bound_type() == config_t::cb_corner);
		_rdb_full_line.set_state(conf->get_const_bound_type() == config_t::cb_full);
		gwin::set_text(_edt_factor_min, conf->get_min_angle_length_factor());
		gwin::set_text(_edt_factor_max, conf->get_max_angle_length_factor());
		gwin::set_text(_edt_angle, conf->get_const_bound_angle());
		_chk_connection_plot.set_state(conf->draw_asterism());
		gwin::set_text(_edt_connection_from_file, conf->get_asterism_file_name());
		_rdb_connection_solid.set_state(conf->get_asterism_style() == config_t::ls_solid);
		_rdb_connection_dots.set_state(conf->get_asterism_style() == config_t::ls_dots);

		on_update_states();
	}

	void on_update_states()
	{
		if (_chk_plot_bounds.is_checked())
		{
			gwin::enable(_rdb_plot_line);
			gwin::enable(_rdb_plot_dots);
			gwin::enable(_rdb_corner);
			gwin::enable(_rdb_full_line);

			if (_rdb_corner.is_checked())
			{
				gwin::enable(_edt_factor_min);
				gwin::enable(_edt_factor_max);
				gwin::enable(_edt_angle);
			} else {
				gwin::disable(_edt_factor_min);
				gwin::disable(_edt_factor_max);
				gwin::disable(_edt_angle);
			}
		} else {
			gwin::disable(_rdb_plot_line);
			gwin::disable(_rdb_plot_dots);
			gwin::disable(_rdb_corner);
			gwin::disable(_rdb_full_line);
			gwin::disable(_edt_factor_min);
			gwin::disable(_edt_factor_max);
			gwin::disable(_edt_angle);
		}

		if (_chk_connection_plot.is_checked())
		{
			gwin::enable(_edt_connection_from_file);
			gwin::enable(_rdb_connection_solid);
			gwin::enable(_rdb_connection_dots);
		} else {
			gwin::disable(_edt_connection_from_file);
			gwin::disable(_rdb_connection_solid);
			gwin::disable(_rdb_connection_dots);
		}
	}

	void save()
	{
		gwin::write_config("", "DrawConstellationBounds", _chk_plot_bounds.is_checked());
		if (_rdb_plot_line.is_checked())
			gwin::write_config("", "ConstellationBoundStyle", std::string("SOLID"));
		else
			gwin::write_config("", "ConstellationBoundStyle", std::string("DOTS"));

		if (_rdb_corner.is_checked())
			gwin::write_config("", "ConstellationBoundType", std::string("CORNER"));
		else
			gwin::write_config("", "ConstellationBoundType", std::string("FULL"));

		gwin::write_config("", "MinAngleLengthFactor", atof(gwin::get_text(_edt_factor_min).c_str()));
		gwin::write_config("", "MaxAngleLengthFactor", atof(gwin::get_text(_edt_factor_max).c_str()));
		gwin::write_config("", "ConstellationBoundAngle", atof(gwin::get_text(_edt_angle).c_str()));
		gwin::write_config("", "DrawAsterism", _chk_connection_plot.is_checked());
		gwin::write_config("", "AsterismFileName", gwin::get_text(_edt_connection_from_file));
		if (_rdb_connection_solid.is_checked())
			gwin::write_config("", "AsterismLineStyle", std::string("SOLID"));
		else
			gwin::write_config("", "AsterismLineStyle", std::string("DOTS"));
	}

	// the default message handler
	template <class DefaultMessage>
	bool operator () (const DefaultMessage&)
	{
		return false;
	}
};

typedef gwin::dialog<constellations_page_controller> constellations_page;


// ******************************************
// ngc page

class ngc_page_controller: public gwin::window
{
private:

	gwin::check_box<ngc_page_controller> _chk_plot_ngc_objects;
	gwin::check_box<ngc_page_controller> _chk_messier_only;
	gwin::edit_box<ngc_page_controller> _edt_name_text_height;
	gwin::combo_box<ngc_page_controller> _cmb_designations_type;

	gwin::check_box<ngc_page_controller> _chk_plot_planetary_nebulae;
	gwin::edit_box<ngc_page_controller> _edt_min_planetary_nebulae_magnitude;
	gwin::edit_box<ngc_page_controller> _edt_min_planetary_nebulae_diameter;
	gwin::edit_box<ngc_page_controller> _edt_max_planetary_nebulae_diameter;

	gwin::check_box<ngc_page_controller> _chk_plot_nebulae;
	gwin::edit_box<ngc_page_controller> _edt_min_nebulae_magnitude;
	gwin::edit_box<ngc_page_controller> _edt_min_nebulae_diameter;
	gwin::edit_box<ngc_page_controller> _edt_max_nebulae_diameter;

	gwin::check_box<ngc_page_controller> _chk_plot_galaxies;
	gwin::edit_box<ngc_page_controller> _edt_min_galaxy_magnitude;
	gwin::edit_box<ngc_page_controller> _edt_min_galaxy_diameter;
	gwin::edit_box<ngc_page_controller> _edt_max_galaxy_diameter;

	gwin::check_box<ngc_page_controller> _chk_plot_open_clusters;
	gwin::edit_box<ngc_page_controller> _edt_min_open_cluster_magnitude;
	gwin::edit_box<ngc_page_controller> _edt_min_open_cluster_diameter;
	gwin::edit_box<ngc_page_controller> _edt_max_open_cluster_diameter;

	gwin::check_box<ngc_page_controller> _chk_plot_globular_clusters;
	gwin::edit_box<ngc_page_controller> _edt_min_globular_cluster_magnitude;
	gwin::edit_box<ngc_page_controller> _edt_min_globular_cluster_diameter;
	gwin::edit_box<ngc_page_controller> _edt_max_globular_cluster_diameter;

public:
	enum {resource_id = IDD_PAGE_NGC};

	ngc_page_controller(gwin::window& w)
	:	gwin::window(w),
		_chk_plot_ngc_objects(IDC_CHK_PLOT_NGC, *this),
		_chk_messier_only(IDC_CHK_MESSIER_ONLY, *this),
		_edt_name_text_height(IDC_EDT_NAME_TEXT_HEIGHT, *this),
		_cmb_designations_type(IDC_CMB_DESIGNATIONS_TYPE, *this),

		_chk_plot_planetary_nebulae(IDC_CHK_PLOT_PLANETARY_NEBULAE, *this),
		_edt_min_planetary_nebulae_magnitude(IDC_EDT_MIN_PLANETARY_NEBULAE_MAGNITUDE, *this),
		_edt_min_planetary_nebulae_diameter(IDC_EDT_MIN_PLANETARY_NEBULAE_DIAMETER, *this),
		_edt_max_planetary_nebulae_diameter(IDC_EDT_MAX_PLANETARY_NEBULAE_DIAMETER, *this),

		_chk_plot_nebulae(IDC_CHK_PLOT_NEBULAE, *this),
		_edt_min_nebulae_magnitude(IDC_EDT_MIN_NEBULAE_MAGNITUDE, *this),
		_edt_min_nebulae_diameter(IDC_EDT_MIN_NEBULAE_DIAMETER, *this),
		_edt_max_nebulae_diameter(IDC_EDT_MAX_NEBULAE_DIAMETER, *this),

		_chk_plot_galaxies(IDC_CHK_PLOT_GALAXIES, *this),
		_edt_min_galaxy_magnitude(IDC_EDT_MIN_GALAXY_MAGNITUDE, *this),
		_edt_min_galaxy_diameter(IDC_EDT_MIN_GALAXY_DIAMETER, *this),
		_edt_max_galaxy_diameter(IDC_EDT_MAX_GALAXY_DIAMETER, *this),

		_chk_plot_open_clusters(IDC_CHK_PLOT_OPEN_CLUSTERS, *this),
		_edt_min_open_cluster_magnitude(IDC_EDT_MIN_OPEN_CLUSTER_MAGNITUDE, *this),
		_edt_min_open_cluster_diameter(IDC_EDT_MIN_OPEN_CLUSTER_DIAMETER, *this),
		_edt_max_open_cluster_diameter(IDC_EDT_MAX_OPEN_CLUSTER_DIAMETER, *this),

		_chk_plot_globular_clusters(IDC_CHK_PLOT_GLOBULAR_CLUSTERS, *this),
		_edt_min_globular_cluster_magnitude(IDC_EDT_MIN_GLOBULAR_CLUSTER_MAGNITUDE, *this),
		_edt_min_globular_cluster_diameter(IDC_EDT_MIN_GLOBULAR_CLUSTER_DIAMETER, *this),
		_edt_max_globular_cluster_diameter(IDC_EDT_MAX_GLOBULAR_CLUSTER_DIAMETER, *this)

	{}

	std::string get_page_title() const	{ return "NGC"; }
	void on_page_changed()				{}

	bool operator () (gwin::wm_initdialog& msg)
	{
		// event handlers
		_chk_plot_ngc_objects.on_click = gwin::closure(this, on_update_state);
		_cmb_designations_type.on_change = gwin::closure(this, on_update_state);
		_chk_plot_planetary_nebulae.on_click = gwin::closure(this, on_update_state);
		_chk_plot_nebulae.on_click = gwin::closure(this, on_update_state);
		_chk_plot_galaxies.on_click = gwin::closure(this, on_update_state);
		_chk_plot_open_clusters.on_click = gwin::closure(this, on_update_state);
		_chk_plot_globular_clusters.on_click = gwin::closure(this, on_update_state);

		// init controls
		_cmb_designations_type.add_item("None");
		_cmb_designations_type.add_item("NGC");
		_cmb_designations_type.add_item("Messier");
		_cmb_designations_type.add_item("NGC && Messier");

		// load values
		switch (conf->draw_ngc_type())
		{
		case config_t::ngc_all:
			_chk_plot_ngc_objects.set_state(true);
			_chk_messier_only.set_state(false);
			break;
		case config_t::ngc_messier_only:
			_chk_plot_ngc_objects.set_state(true);
			_chk_messier_only.set_state(true);
			break;
		case config_t::ngc_none:
			_chk_plot_ngc_objects.set_state(false);
			_chk_messier_only.set_state(false);
			break;
		}

		gwin::set_text(_edt_name_text_height, conf->get_ngc_names_height());
		_cmb_designations_type.select_item(conf->get_ngc_names_type());

		_chk_plot_planetary_nebulae.set_state(conf->draw_planetary_nebula());
		gwin::set_text(_edt_min_planetary_nebulae_magnitude, conf->get_mag_limit_pn());
		gwin::set_text(_edt_min_planetary_nebulae_diameter, conf->get_min_diam_pn());
		gwin::set_text(_edt_max_planetary_nebulae_diameter, conf->get_max_diam_pn());

		_chk_plot_nebulae.set_state(conf->draw_nebula());
		gwin::set_text(_edt_min_nebulae_magnitude, conf->get_mag_limit_n());
		gwin::set_text(_edt_min_nebulae_diameter, conf->get_min_diam_n());
		gwin::set_text(_edt_max_nebulae_diameter, conf->get_max_diam_n());

		_chk_plot_galaxies.set_state(conf->draw_galaxies());
		gwin::set_text(_edt_min_galaxy_magnitude, conf->get_mag_limit_g());
		gwin::set_text(_edt_min_galaxy_diameter, conf->get_min_diam_g());
		gwin::set_text(_edt_max_galaxy_diameter, conf->get_max_diam_g());

		_chk_plot_open_clusters.set_state(conf->draw_open_clusters());
		gwin::set_text(_edt_min_open_cluster_magnitude, conf->get_mag_limit_oc());
		gwin::set_text(_edt_min_open_cluster_diameter, conf->get_min_diam_oc());
		gwin::set_text(_edt_max_open_cluster_diameter, conf->get_max_diam_oc());

		_chk_plot_globular_clusters.set_state(conf->draw_globular_clusters());
		gwin::set_text(_edt_min_globular_cluster_magnitude, conf->get_mag_limit_gc());
		gwin::set_text(_edt_min_globular_cluster_diameter, conf->get_min_diam_gc());
		gwin::set_text(_edt_max_globular_cluster_diameter, conf->get_max_diam_gc());

		on_update_state();

		return false;
	}

	void on_update_state()
	{
		if (_chk_plot_ngc_objects.is_checked())
		{
			gwin::enable(_chk_messier_only);
			gwin::enable(_cmb_designations_type);

			if (_cmb_designations_type.get_selected() == 0)
				gwin::disable(_edt_name_text_height);
			else
				gwin::enable(_edt_name_text_height);

			gwin::enable(_chk_plot_planetary_nebulae);
			if (_chk_plot_planetary_nebulae.is_checked())
			{
				gwin::enable(_edt_min_planetary_nebulae_magnitude);
				gwin::enable(_edt_min_planetary_nebulae_diameter);
				gwin::enable(_edt_max_planetary_nebulae_diameter);
			} else {
				gwin::disable(_edt_min_planetary_nebulae_magnitude);
				gwin::disable(_edt_min_planetary_nebulae_diameter);
				gwin::disable(_edt_max_planetary_nebulae_diameter);
			}

			gwin::enable(_chk_plot_nebulae);
			if (_chk_plot_nebulae.is_checked())
			{
				gwin::enable(_edt_min_nebulae_magnitude);
				gwin::enable(_edt_min_nebulae_diameter);
				gwin::enable(_edt_max_nebulae_diameter);
			} else {
				gwin::disable(_edt_min_nebulae_magnitude);
				gwin::disable(_edt_min_nebulae_diameter);
				gwin::disable(_edt_max_nebulae_diameter);
			}

			gwin::enable(_chk_plot_galaxies);
			if (_chk_plot_galaxies.is_checked())
			{
				gwin::enable(_edt_min_galaxy_magnitude);
				gwin::enable(_edt_min_galaxy_diameter);
				gwin::enable(_edt_max_galaxy_diameter);
			} else {
				gwin::disable(_edt_min_galaxy_magnitude);
				gwin::disable(_edt_min_galaxy_diameter);
				gwin::disable(_edt_max_galaxy_diameter);
			}

			gwin::enable(_chk_plot_open_clusters);
			if (_chk_plot_open_clusters.is_checked())
			{
				gwin::enable(_edt_min_open_cluster_magnitude);
				gwin::enable(_edt_min_open_cluster_diameter);
				gwin::enable(_edt_max_open_cluster_diameter);
			} else {
				gwin::disable(_edt_min_open_cluster_magnitude);
				gwin::disable(_edt_min_open_cluster_diameter);
				gwin::disable(_edt_max_open_cluster_diameter);
			}

			gwin::enable(_chk_plot_globular_clusters);
			if (_chk_plot_globular_clusters.is_checked())
			{
				gwin::enable(_edt_min_globular_cluster_magnitude);
				gwin::enable(_edt_min_globular_cluster_diameter);
				gwin::enable(_edt_max_globular_cluster_diameter);
			} else {
				gwin::disable(_edt_min_globular_cluster_magnitude);
				gwin::disable(_edt_min_globular_cluster_diameter);
				gwin::disable(_edt_max_globular_cluster_diameter);
			}


		} else {

			gwin::disable(_chk_messier_only);
			gwin::disable(_cmb_designations_type);
			gwin::disable(_edt_name_text_height);

			gwin::disable(_chk_plot_planetary_nebulae);
			gwin::disable(_edt_min_planetary_nebulae_magnitude);
			gwin::disable(_edt_min_planetary_nebulae_diameter);
			gwin::disable(_edt_max_planetary_nebulae_diameter);

			gwin::disable(_chk_plot_nebulae);
			gwin::disable(_edt_min_nebulae_magnitude);
			gwin::disable(_edt_min_nebulae_diameter);
			gwin::disable(_edt_max_nebulae_diameter);

			gwin::disable(_chk_plot_galaxies);
			gwin::disable(_edt_min_galaxy_magnitude);
			gwin::disable(_edt_min_galaxy_diameter);
			gwin::disable(_edt_max_galaxy_diameter);

			gwin::disable(_chk_plot_open_clusters);
			gwin::disable(_edt_min_open_cluster_magnitude);
			gwin::disable(_edt_min_open_cluster_diameter);
			gwin::disable(_edt_max_open_cluster_diameter);

			gwin::disable(_chk_plot_globular_clusters);
			gwin::disable(_edt_min_globular_cluster_magnitude);
			gwin::disable(_edt_min_globular_cluster_diameter);
			gwin::disable(_edt_max_globular_cluster_diameter);
		}
	}

	void save()
	{
		if (_chk_plot_ngc_objects.is_checked())
		{
			if (_chk_messier_only.is_checked())
				gwin::write_config("", "DrawNGCType", std::string("MESSIER_ONLY"));
			else
				gwin::write_config("", "DrawNGCType", std::string("ALL"));
		} else {
			gwin::write_config("", "DrawNGCType", std::string("NONE"));
		}

		switch (_cmb_designations_type.get_selected())
		{
		case 0:	gwin::write_config("", "NGCNamesType", std::string("NONE"));		break;
		case 1:	gwin::write_config("", "NGCNamesType", std::string("NGC"));			break;
		case 2:	gwin::write_config("", "NGCNamesType", std::string("MESSIER"));		break;
		case 3:	gwin::write_config("", "NGCNamesType", std::string("BOTH"));		break;
		}

		gwin::write_config("", "NGCNamesHeight", atof(gwin::get_text(_edt_name_text_height).c_str()));

		gwin::write_config("", "DrawNebula", _chk_plot_nebulae.is_checked());
		gwin::write_config("", "NebulaMagnitudeLimit", atof(gwin::get_text(_edt_min_nebulae_magnitude).c_str()));
		gwin::write_config("", "MinNebulaDiameter", atof(gwin::get_text(_edt_min_nebulae_diameter).c_str()));
		gwin::write_config("", "MaxNebulaDiameter", atof(gwin::get_text(_edt_max_nebulae_diameter).c_str()));

		gwin::write_config("", "DrawPlanetaryNebula", _chk_plot_planetary_nebulae.is_checked());
		gwin::write_config("", "PlanetaryNebulaMagnitudeLimit", atof(gwin::get_text(_edt_min_planetary_nebulae_magnitude).c_str()));
		gwin::write_config("", "MinPlanetaryNebulaDiameter", atof(gwin::get_text(_edt_min_planetary_nebulae_diameter).c_str()));
		gwin::write_config("", "MaxPlanetaryNebulaDiameter", atof(gwin::get_text(_edt_max_planetary_nebulae_diameter).c_str()));

		gwin::write_config("", "DrawGalaxies", _chk_plot_galaxies.is_checked());
		gwin::write_config("", "GalaxyMagnitudeLimit", atof(gwin::get_text(_edt_min_galaxy_magnitude).c_str()));
		gwin::write_config("", "MinGalaxyDiameter", atof(gwin::get_text(_edt_min_galaxy_diameter).c_str()));
		gwin::write_config("", "MaxGalaxyDiameter", atof(gwin::get_text(_edt_max_galaxy_diameter).c_str()));

		gwin::write_config("", "DrawOpenClusters", _chk_plot_open_clusters.is_checked());
		gwin::write_config("", "OpenClusterMagnitudeLimit", atof(gwin::get_text(_edt_min_open_cluster_magnitude).c_str()));
		gwin::write_config("", "MinOpenClusterDiameter", atof(gwin::get_text(_edt_min_open_cluster_diameter).c_str()));
		gwin::write_config("", "MaxOpenClusterDiameter", atof(gwin::get_text(_edt_max_open_cluster_diameter).c_str()));

		gwin::write_config("", "DrawGlobularClusters", _chk_plot_globular_clusters.is_checked());
		gwin::write_config("", "GlobClusterMagnitudeLimit", atof(gwin::get_text(_edt_min_globular_cluster_magnitude).c_str()));
		gwin::write_config("", "MinGlobClusterDiameter", atof(gwin::get_text(_edt_min_globular_cluster_diameter).c_str()));
		gwin::write_config("", "MaxGlobClusterDiameter", atof(gwin::get_text(_edt_max_globular_cluster_diameter).c_str()));
	}

	// the default message handler
	template <class DefaultMessage>
	bool operator () (const DefaultMessage&)
	{
		return false;
	}
};

typedef gwin::dialog<ngc_page_controller> ngc_page;


// ******************************************
// main dialog

class main_dialog_controller: public gwin::window
{
private:

	gwin::tab_control<main_dialog_controller,
						TYPELIST_5(	output_page,
									general_page,
									stars_page,
									constellations_page,
									ngc_page)>		_tab;

	gwin::button<main_dialog_controller>			_btn_start;

	config_t		_conf;

public:
	enum {resource_id = IDD_MAIN};

	main_dialog_controller(gwin::window& w)
	:	gwin::window(w),
		_tab(IDC_TAB, *this),
		_btn_start(IDOK, *this)
	{
		gwin::set_config(gwin::e_ini_config, ".\\stars_gui.cfg");
	}


	bool operator () (gwin::wm_initdialog& msg)
	{
		// center the dialog window
		gwin::center_window(*this);

		/*
		_tab.add_page("General");
		_tab.add_page("Stars");
		_tab.add_page("Constellations");
		_tab.add_page("NGC");
		*/

		_tab.init();

		// setup the event handlers
		_btn_start.on_click = gwin::closure(this, on_start);

		return false;
	}

	void save_data()
	{
		// save the data
		_tab.get_page<output_page>().get_controller().save();
		_tab.get_page<general_page>().get_controller().save();
		_tab.get_page<constellations_page>().get_controller().save();
		_tab.get_page<stars_page>().get_controller().save();
		_tab.get_page<ngc_page>().get_controller().save();
	}

	void on_start()
	{
		save_data();

		log_dialog_controller::init_t log_init;

		log_init.project_name = "stars_gui.cfg";
		log_init.eps_file_name = _tab.get_page<output_page>().get_controller().get_eps_file_name();

		log_dialog log_dlg(log_init);
		log_dlg.modal(*this);
	}

	bool operator () (gwin::wm_close& msg)
	{
		save_data();

		// destroy the tab window
		gwin::destroy_window(_tab);

		::PostQuitMessage(0);

		return true;
	}

	// the default message handler
	template <class DefaultMessage>
	bool operator () (const DefaultMessage&)
	{
		return false;
	}
};

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, char* commandLine, int cmdShow)
{
	// load the custom controls dll
	::InitCommonControls();

	gwin::dialog<main_dialog_controller> main_dlg;
	main_dlg.modeless(gwin::window());

    // the main message loop
    MSG msg;
    int status;
    while ((status = ::GetMessage(&msg, 0, 0, 0)) != 0)
    {
        if (status == -1)
            return -1;

		if (::IsDialogMessage(main_dlg.get_hwnd(), &msg) == 0)
		{
			::TranslateMessage(&msg);
		    ::DispatchMessage(&msg);
		}
    }

    return msg.wParam;
}

