#include "stl.h"
#pragma hdrstop

#include "point.h"
#include "ps_canvas.h"
#include "config.h"
#include "calcs.h"

point coordinate::conv2point() const
{
	return point(	calc_pole_distance(*this) * canvas->get_extent() / cfg->get_max_pole_distance(),
					hour2rad(alpha));
}

