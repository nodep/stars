#include "stl.h"
#pragma hdrstop

#include "star.h"
#include "storage.h"
#include "config.h"
#include "pi.h"
#include "calcs.h"

/*
0000000001111111111222222222233333333334444444444555555555566666666668
1234567890123456789012345678901234567890123456789012345678901234567890 
17 33 36.551  -37  6 13.61   1.7 V       14482 35 LAM SCO Shaula
Right ascent  declination-   mag V VDsgn MDsgN FlmBay Con Common Name
*/

void star::decode(const char* row)
{
	// the coordinates of the star
	coord.alpha = atoi(row);
	coord.alpha += atoi(row + 3) / 60.0;
	coord.alpha += atof(row + 6) / 3600.0;

	coord.delta = atoi(row + 14);

	double fract;
	fract  = atoi(row + 18) / 60.0;
	fract += atoi(row + 21) / 3600.0;

	if (coord.delta > 0)
		coord.delta += fract;
	else
		coord.delta -= fract;

	// the star's magnitude
	magnitude = atof(row + 28);

	size_t row_length = strlen(row);

	// if the star is variable
	if (row_length > 32)
	{
		is_variable = (row[33] == 'V');
		if (row_length > 36  &&  row[35] != ' ')
		{
			var_name = row + 35;
			var_name = var_name.substr(0, 5);

			size_t srchRes = var_name.find_last_not_of(" ");
			if (srchRes != std::string::npos)
				var_name = var_name.substr(0, srchRes + 1);
		} else {
			var_name = "";
		}
	} else {
		is_variable = false;
		var_name = "";
	}

	// the multiple system code (if any)
	if (row_length >= 46  &&  row[45] != ' ')
		multiple_system = atoi(row + 41);
	else
		multiple_system = 0;

	// the Flamsteed or Bayer designation and the common name
	fb_code = name = "";
	if (row_length > 46)
	{
		fb_code = row + 47;
		if (fb_code[7] == ' ')
			fb_code = "";
		else
			fb_code = fb_code.substr(0, 10);

		if (cfg->get_star_name_mag_limit() > magnitude  &&  row_length > 57)
			name = row + 58;
	}

	// the pleiades  (3h43' < alpha < 3h50'  &&  23deg 50' < delta < 25deg)
	is_pleiade = (	coord.alpha > 3.7  &&  coord.alpha < 3.83  &&
					coord.delta > 23.83  &&  coord.delta < 25);

	/*
	// convert the coordinates
	polar_transform()(coord, coordinate(5.5, 0));
	*/
}
