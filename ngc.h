#pragma once

#include "coordinate.h"

template <class ObjectTag, size_t object_id>
struct ngc_object
{
	enum {objectID = object_id};

	coordinate		coord;
	double			magnitude;
	double			diameter;
	std::string		name;

	bool			is_messier;
	std::string		messier_id;

	bool decode(const char* line)
	{
		// the magnitude
		magnitude = atof(line + 40);

		// we don't want do draw anything who's magnitude we don't know
		if (magnitude == 0)
			return true;

		// the ID
		name = line;
		name = name.substr(0, 5);
		size_t index = name.find_last_of(" ");

		if (index != std::string::npos)
			name = name.substr(index + 1);

		// the position
		coord.alpha  = atof(line + 10);
		coord.alpha += atof(line + 13) / 60.0;

		coord.delta  = atof(line + 20);
		coord.delta += atof(line + 23) / 60.0;
		if (line[19] == '-')
			coord.delta = -coord.delta;

		// if the object is in the Messier catalog
		std::map<std::string, std::string>::const_iterator i = store->ngc2messier.find(name);
		if (i != store->ngc2messier.end())
		{
			is_messier = true;
			messier_id = i->second;
		} else {
			is_messier = false;
			messier_id = "";
		}

		return false;
	}
};

struct galaxy_tag				{};
struct nebula_tag				{};
struct planetary_nebula_tag		{};
struct open_cluster_tag			{};
struct globular_cluster_tag		{};

typedef ngc_object<galaxy_tag, 2>				galaxy;
typedef ngc_object<nebula_tag, 3>				nebula;
typedef ngc_object<planetary_nebula_tag, 4>		planetary_nebula;
typedef ngc_object<open_cluster_tag, 5>			open_cluster;
typedef ngc_object<globular_cluster_tag, 6>		globular_cluster;
