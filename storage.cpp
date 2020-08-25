#include "stl.h"
#pragma hdrstop

#include "storage.h"

storage* store = 0;

bool storage::text_overlaps_object(text_object& text, const text_object::position_t& vp)
{
	if (std::find_if(stars.begin(), stars.end(), [&](const auto& o) { return overlap_text_object(text, vp, o); }) != stars.end())
		return true;

	if (std::find_if(galaxies.begin(), galaxies.end(), [&](const auto& o) { return overlap_text_object(text, vp, o); }) != galaxies.end())
		return true;

	if (std::find_if(nebulas.begin(), nebulas.end(), [&](const auto& o) { return overlap_text_object(text, vp, o); }) != nebulas.end())
		return true;

	if (std::find_if(planetary_nebulas.begin(), planetary_nebulas.end(), [&](const auto& o) { return overlap_text_object(text, vp, o); }) != planetary_nebulas.end())
		return true;

	if (std::find_if(open_clusters.begin(), open_clusters.end(), [&](const auto& o) { return overlap_text_object(text, vp, o); }) != open_clusters.end())
		return true;

	if (std::find_if(globular_clusters.begin(), globular_clusters.end(), [&](const auto& o) { return overlap_text_object(text, vp, o); }) != globular_clusters.end())
		return true;

	// also the fixed texts
	if (std::find_if(fixed_texts.begin(), fixed_texts.end(), [&](auto& o) { return overlap_text_text(vp, o.valid_positions.front()); }) != fixed_texts.end())
		return true;

	// custom objects
	if (std::find_if(custom_objects.begin(), custom_objects.end(), [&](const auto& o) { return overlap_text_object(text, vp, o); }) != custom_objects.end())
		return true;

	return false;
}
