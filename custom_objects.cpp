#include "stl.h"
#pragma hdrstop

#include "coordinate.h"
#include "custom_objects.h"

custom_object custom_object::import(const std::string& in)
{
	custom_object ret_val;

	ret_val.sign_type = alltrim(in.substr(0, 6));

	const char* p = in.c_str() + 6;
	ret_val.coord.alpha = atoi(p);
	ret_val.coord.alpha += atoi(p + 3) / 60.0;
	ret_val.coord.alpha += atof(p + 6) / 3600.0;

	p = in.c_str() + 17;
	ret_val.coord.delta = atoi(p);
	ret_val.coord.delta += atoi(p + 3) / 60.0;
	ret_val.coord.delta += atoi(p + 6) / 3600.0;
	if (in[16] == '-')
		ret_val.coord.delta *= -1;

	ret_val.text_height = atof(in.c_str() + 27);

	if (in.length() > 32)
		ret_val.name = alltrim(in.substr(32));

	return ret_val;
}
