#include "stl.h"
#pragma hdrstop

#include "storage.h"

storage* store = 0;

struct overlaps_object
{
	const text_object& txt;

	overlaps_object(const text_object& t)
		: txt(t)
	{}

	// special case for texts
	bool operator () (const text_object& obj) const
	{
		// a text does not overlap itself
		if (&txt == &obj)
			return false;

		return overlap_text_text(txt, obj);
	}

	// more generic instance for other, circular objects
	template <class Object>
	bool operator () (const Object& obj) const
	{
		return overlap_text_object(txt, obj);
	}
};

bool storage::text_overlaps_object(const text_object& text)
{
	if (std::find_if(stars.begin(), stars.end(), overlaps_object(text)) != stars.end())
		return true;

	if (std::find_if(galaxies.begin(), galaxies.end(), overlaps_object(text)) != galaxies.end())
	// {
	// 	log_stream << "galaxy" << endl;
		return true;
	// }

	if (std::find_if(nebulas.begin(), nebulas.end(), overlaps_object(text)) != nebulas.end())
	// {
	// 	log_stream << "nebula" << endl;
		return true;
	// }

	if (std::find_if(planetary_nebulas.begin(), planetary_nebulas.end(), overlaps_object(text)) != planetary_nebulas.end())
	// {
	// 	log_stream << "p. nebula" << endl;
		return true;
	// }

	if (std::find_if(open_clusters.begin(), open_clusters.end(), overlaps_object(text)) != open_clusters.end())
	// {
	// 	log_stream << "open cluster" << endl;
		return true;
	// }

	if (std::find_if(globular_clusters.begin(), globular_clusters.end(), overlaps_object(text)) != globular_clusters.end())
	// {
	// 	log_stream << "globular cluster" << endl;
		return true;
	// }

	// also the fixed texts
	if (std::find_if(fixed_texts.begin(), fixed_texts.end(), overlaps_object(text)) != fixed_texts.end())
	// {
	// 	log_stream << "fixed text" << endl;
		return true;
	// }

	// custom objects
	if (std::find_if(custom_objects.begin(), custom_objects.end(), overlaps_object(text)) != custom_objects.end())
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
