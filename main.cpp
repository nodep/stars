#include "stl.h"
#pragma hdrstop

#include "point.h"
#include "config.h"
#include "coordinate.h"
#include "ps_canvas.h"
#include "process.h"
#include "storage.h"

log_t log_stream;

int main(int argc, char* argv[])
{
	log_stream << "Sky Map Generator v1.0 alpha\n\n";

	// the arguments
	try {
		char* cfg_file_name = 0;
		if (argc >= 2)
		{
			cfg_file_name = argv[1];
		} else {
			throw "Config file not specified";
		}

		// instantiate the objects (subsystems)
		config				c;			// configuration
		post_script_canvas	psc;		// the post script canvas
		process				proc;		// does all the processing
		storage				s;			// the storage where all the map objects are stored

		cfg->parse_file(cfg_file_name);

		proc.run();

	} catch (const char* e) {
		log_stream << "<!exception!>" << e << "\n";
		return 1;
	} catch (std::string e) {
		log_stream << "<!exception!>" << e << "\n";
		return 1;
		/*
	} catch (std::bad_alloc e) {
		log_stream << "<!exception!>Out of memory!\n";
		return 1;
	} catch (...) {
		log_stream << "<!exception!>Unexpected exception\n";
		return 1;
		*/
	}

	log_stream << "<!finished!>\n";

	/*
	char buff[10];
	// wait for response
	cin.getline(buff, sizeof(buff));
	*/

	return 0;
}

std::string alltrim(const std::string in)
{
	size_t ndx1 = in.find_first_not_of(" \t\r\n");
	if (ndx1 == std::string::npos)
		return "";

	size_t ndx2 = in.find_last_not_of(" \t\r\n");
	std::string::const_iterator f = in.begin() + (ndx1 == std::string::npos ? 0 : ndx1);
	std::string::const_iterator l = (ndx2 == std::string::npos ? in.end() : in.begin() + ndx2 + 1);
	std::string ret_val;
	std::copy(f, l, std::back_inserter(ret_val));
	return ret_val;
}
