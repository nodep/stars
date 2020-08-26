#pragma once

#include "star.h"
#include "text.h"
#include "asterism_line.h"
#include "constellation_boundary.h"
#include "irregular_surface.h"
#include "ngc.h"
#include "calcs.h"
#include "radiant.h"
#include "custom_objects.h"

struct storage;

extern storage* store;

// all the objects on the map are embedded in this object
struct storage
{
	std::vector<star>					stars;
	std::vector<asterism_line>			asterism_lines;
	std::vector<constellation_boundary>	constellation_boundaries;
	std::vector<irregular_surface>		isophotes;
	std::vector<text_object>			texts;
	std::vector<text_object>			fixed_texts;
	std::vector<galaxy>					galaxies;
	std::vector<nebula>					nebulas;
	std::vector<planetary_nebula>		planetary_nebulas;
	std::vector<open_cluster>			open_clusters;
	std::vector<globular_cluster>		globular_clusters;
	std::vector<radiant>				radiants;
	std::vector<custom_object>			custom_objects;

	std::map<std::string, std::string>	ngc2messier;

    typedef std::map<size_t, std::pair<double, double> >    mag_range_storage_t;
    mag_range_storage_t  mag_storage;

    std::pair<double, double>& get_object_mag_range(const size_t& oid)
    {
        mag_range_storage_t::iterator srch_res(mag_storage.find(oid));
        if (srch_res == mag_storage.end())
		{
			std::pair<mag_range_storage_t::iterator, bool> res = 
							mag_storage.insert(mag_range_storage_t::value_type(oid, std::make_pair(-1e9, 1e9)));
			return res.first->second;
		}

        return srch_res->second;
    }

    template <typename T>
    double get_faintest_mag(const T&)
    {
        return get_object_mag_range(T::objectID).first;
    }

    template <typename T>
    double get_brightest_mag(const T&)
    {
        return get_object_mag_range(T::objectID).second;
    }

	template <typename T>
    void store_mag(T& o)
    {
		std::pair<double, double>& mag_pair(get_object_mag_range(T::objectID));

		if (mag_pair.first < o.magnitude)
			mag_pair.first = o.magnitude;

		if (mag_pair.second > o.magnitude)
			mag_pair.second = o.magnitude;
    }

	storage()
	{
		assert(store == 0);
		store = this;
	}

	~storage()
	{
		assert(store == this);
		store = 0;
	}

	void store_star(const star& s)
	{
		stars.push_back(s);
		store_mag(s);
	}

	void store_ngc(const galaxy& g)
	{
		galaxies.push_back(g);
		store_mag(g);
	}

	void store_ngc(const nebula& n)
	{
		nebulas.push_back(n);
		store_mag(n);
	}

	void store_ngc(const planetary_nebula& pn)
	{
		planetary_nebulas.push_back(pn);
		store_mag(pn);
	}

	void store_ngc(const open_cluster& oc)
	{
		open_clusters.push_back(oc);
		store_mag(oc);
	}

	void store_ngc(const globular_cluster& gc)
	{
		globular_clusters.push_back(gc);
		store_mag(gc);
	}

	bool text_overlaps_object(text_object& text, const text_object::position_t& vp);

	bool text_overlaps_text(const text_object::position_t& vp)
	{
		for (const auto& mov_text : texts)
		{
			if (overlap_text_text(vp, mov_text.curr_pos()))
				return true;
		}

		return false;
	}
};
