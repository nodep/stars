#pragma once

#include "storage.h"
#include "config.h"

template <class Object>
struct update_diameter
{
	typedef Object&		result_type;
	typedef Object&		argument_type;

	// linear diameter of an ibject
	void linear_df (Object& obj) const
	{
		//double mag_delta = store->get_faintest_mag<Object>() - store->get_brightest_mag<Object>();
		double mag_delta = store->get_faintest_mag(obj) - store->get_brightest_mag(obj);
		if (mag_delta == 0)
		{
			obj.diameter = (cfg->get_max_diam(obj) + cfg->get_min_diam(obj)) / 2;
		} else {
			obj.diameter = (store->get_faintest_mag(obj) - obj.magnitude) / mag_delta;
			obj.diameter *= cfg->get_max_diam(obj) - cfg->get_min_diam(obj);
			obj.diameter += cfg->get_min_diam(obj);
		}
	}

	// calc the atan diameter of an object
	void atan_df (Object& obj) const
	{
		double mag_delta = store->get_faintest_mag(obj) - store->get_brightest_mag(obj);
		if (mag_delta == 0)
		{
			obj.diameter = (cfg->get_max_diam(obj) + cfg->get_min_diam(obj)) / 2;
		} else {
			double in_mag = (store->get_faintest_mag(obj) - obj.magnitude) / mag_delta;
    		in_mag = in_mag * 9 - 4.5;
    		double angle = atan(in_mag);
			double factor = angle / pi * 1.166 + 0.5;

			obj.diameter = cfg->get_min_diam(obj) + factor * (cfg->get_max_diam(obj) - cfg->get_min_diam(obj));
		}
	}

	// calc the exp diameter of an object (Sky & Telescope, march 1990)
	void exp_df(Object& obj) const
	{
		obj.diameter = cfg->get_min_diam(obj) * pow(10, .11 * (store->get_faintest_mag(obj) - obj.magnitude));
	}

	void const_df(Object& obj) const
	{
		obj.diameter = (cfg->get_max_diam(obj) + cfg->get_min_diam(obj)) / 2;
	}

	Object& operator() (Object& obj) const
	{
		switch (cfg->get_diam_func_type(obj))
		{
		case config::dft_linear:
			linear_df(obj);
			break;
		case config::dft_atan:
			atan_df(obj);
			break;
		case config::dft_const:
			const_df(obj);
			break;
		case config::dft_exp:
			exp_df(obj);
			break;
		}

		return obj;
	}
};
