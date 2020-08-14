#include "stl.h"
#pragma hdrstop

#include "storage.h"

storage* store = 0;

bool storage::text_overlaps_object(text_object& text, const text_object::position_t& vp)
{
	if (std::find_if(stars.begin(), stars.end(), [&](const auto& o) { return overlap_text_object(text, vp, o); }) != stars.end())
		return true;

	if (std::find_if(galaxies.begin(), galaxies.end(), [&](const auto& o) { return overlap_text_object(text, vp, o); }) != galaxies.end())
	// {
	// 	log_stream << "galaxy" << endl;
		return true;
	// }

	if (std::find_if(nebulas.begin(), nebulas.end(), [&](const auto& o) { return overlap_text_object(text, vp, o); }) != nebulas.end())
	// {
	// 	log_stream << "nebula" << endl;
		return true;
	// }

	if (std::find_if(planetary_nebulas.begin(), planetary_nebulas.end(), [&](const auto& o) { return overlap_text_object(text, vp, o); }) != planetary_nebulas.end())
	// {
	// 	log_stream << "p. nebula" << endl;
		return true;
	// }

	if (std::find_if(open_clusters.begin(), open_clusters.end(), [&](const auto& o) { return overlap_text_object(text, vp, o); }) != open_clusters.end())
	// {
	// 	log_stream << "open cluster" << endl;
		return true;
	// }

	if (std::find_if(globular_clusters.begin(), globular_clusters.end(), [&](const auto& o) { return overlap_text_object(text, vp, o); }) != globular_clusters.end())
	// {
	// 	log_stream << "globular cluster" << endl;
		return true;
	// }

	// also the fixed texts
	if (std::find_if(fixed_texts.begin(), fixed_texts.end(), [&](auto& o) { return overlap_text_text(vp, o.valid_positions.front()); }) != fixed_texts.end())
	// {
	// 	log_stream << "fixed text" << endl;
		return true;
	// }

	// custom objects
	if (std::find_if(custom_objects.begin(), custom_objects.end(), [&](const auto& o) { return overlap_text_object(text, vp, o); }) != custom_objects.end())
	// {
	// 	log_stream << "globular cluster" << endl;
		return true;
	// }

	/*
	// quasars
	vector<Quasar>::const_iterator quasarIter = _quasars.begin();
	while (quasarIter != _quasars.end())
	{
		if (overlap_text_object(text, *quasarIter))
			return true;

		++quasarIter;
	}

	// x-ray sources
	vector<XRaySource>::const_iterator xrayIter = _XRaySources.begin();
	while (xrayIter != _XRaySources.end())
	{
		if (overlap_text_object(text, *xrayIter))
			return true;

		++xrayIter;
	}

	// check the fixed texts also
	ConstTextIter fti = _fixedTexts.begin();
	while (fti != _fixedTexts.end())
	{
		if (overlap_text_text(text, *fti))
			return true;

		++fti;
	}
	*/

	return false;
}
