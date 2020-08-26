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

	bool decode(const char* line);
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
