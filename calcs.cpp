#include "stl.h"
#pragma hdrstop

#include "calcs.h"
#include "point.h"
#include "text.h"
#include "storage.h"
#include "config.h"

bool _overlap_text_object(const text_object& text, const text_object::position_t& vp, const point& obj_pos, const double diameter)
{
	// get the boundaries of the text
	const point& textLD = vp.left_down_bound;
	const point& textRU = vp.right_up_bound;

	// if the circle's center (R) is inside the text
	bool isInR = is_in_radius(textRU.r, textLD.r, obj_pos.r);

	// if the circle's center (Alpha) is inside the text
	bool isInAlpha = is_in_angle(textLD.alpha, textRU.alpha, obj_pos.alpha);

	// is the center of the circle inside the text?
	if (isInR  &&  isInAlpha)
		return true;

	// if the circle is under the text and overlaps it
	bool isInUnder = is_in_radius(obj_pos.r, obj_pos.r + diameter, textRU.r);

	// if the circle overlaps the text -- under
	if (isInAlpha && isInUnder)
		return true;

	// if the circle is over the text and overlaps it
	bool isInOver = is_in_radius(obj_pos.r - diameter, obj_pos.r, textLD.r);

	// if the circle overlaps the text -- over
	if (isInAlpha  &&  isInOver)
		return true;

	// if the circle overlaps the text at the text's start
	bool isInStart = is_in_angle(obj_pos.alpha - cut_angle(diameter, obj_pos.r), obj_pos.alpha, textRU.alpha);

	// if the circle overlaps the text -- start of text
	if (isInR && isInStart)
		return true;

	bool isInEnd = is_in_angle(obj_pos.alpha, obj_pos.alpha + cut_angle(diameter, obj_pos.r), textLD.alpha);

	// if the circle overlaps the text -- end of text
	if (isInR   &&   isInEnd)
		return true;

	// calc the distance of the center of the circle to the left-down corner of the text
	double center2corner = line_length(obj_pos, textLD);
	if (center2corner < diameter)
		return true;

	// calc the distance of the center of the circle to the left-up corner of the text
	point textLU;
	textLU.alpha = textLD.alpha;
	textLU.r = textRU.r;
	center2corner = line_length(obj_pos, textLU);
	if (center2corner < diameter)
		return true;

	// calc the distance of the center of the circle to the right-up corner of the text
	center2corner = line_length(obj_pos, textRU);
	if (center2corner < diameter)
		return true;

	// calc the distance of the center of the circle to the right-down corner of the text
	point textRD;
	textRD.alpha = textRU.alpha;
	textRD.r = textLD.r;
	center2corner = line_length(obj_pos, textRD);
	if (center2corner < diameter)
		return true;

	return false;
}

bool overlap_1st_text_2nd(const text_object::position_t& vp1, const text_object::position_t& vp2)
{
	// two texts overlap if any of their corners overlap
	const point& textLD1 = vp1.left_down_bound;
	const point& textRU1 = vp1.right_up_bound;
	const point& textLD2 = vp2.left_down_bound;
	const point& textRU2 = vp2.right_up_bound;

	bool isLeftAlpha = is_in_angle(textLD1.alpha, textRU1.alpha, textLD2.alpha);
	bool isRightAlpha = is_in_angle(textLD1.alpha, textRU1.alpha, textRU2.alpha);
	bool isBottomRadius = is_in_angle(textRU1.r, textLD1.r, textLD2.r);
	bool isTopRadius = is_in_angle(textRU1.r, textLD1.r, textRU2.r);

	// the bottom left corner
	if (isLeftAlpha  &&  isBottomRadius)
		return true;

	// the bottom right corner
	if (isRightAlpha  &&  isBottomRadius)
		return true;

	// the top left corner
	if (isLeftAlpha  &&  isTopRadius)
		return true;

	// the top right corner
	if (isRightAlpha  &&  isTopRadius)
		return true;

	// the "cross" position -- no corner is in the other text
	if (isLeftAlpha  &&  isRightAlpha  &&  textLD1.r < textLD2.r  &&  textRU1.r > textRU2.r)
		return true;

	return false;
}

// Herget precession, see p. 9 of publ. Cincinnati obs. no. 24
void precession(const double inEpoch, const coordinate& inCoord, const double outEpoch, coordinate& outCoord)
{
	// na'rani pse!  nista ne diraj!

	const double CSR = 0.017453292519943 / 3600.0;
	double x1[3], x2[3], r[3][3];
	double t, st, a, b, c, sina, sinb, sinc, cosa, cosb, cosc;
	double inDec, inRA, outDec, outRA;

	// convert to radians
	inDec = inCoord.delta / 180.0 * pi;
	inRA  = inCoord.alpha / 12 * pi;

	// Compute input direction cosines
	a = cos(inDec);
 	x1[0] = a * cos(inRA);
	x1[1] = a * sin(inRA);
	x1[2] = sin(inDec);
	t  = 0.001 * (outEpoch - inEpoch);
	st = 0.001 * (inEpoch - 1900.0);
	a = CSR*t*(23042.53+st*(139.75+0.06*st)+t*(30.23-0.27*st+18.0*t));
	b = CSR*t*t*(79.27+0.66*st+0.32*t)+a;
	c = CSR*t*(20046.85-st*(85.33+0.37*st)+t*(-42.67-0.37*st-41.8*t));
	sina = sin(a);
	sinb = sin(b);
	sinc = sin(c);
	cosa = cos(a);
	cosb = cos(b);
	cosc = cos(c);
	r[0][0] = cosa*cosb*cosc-sina*sinb;
	r[0][1] = -cosa*sinb-sina*cosb*cosc;
	r[0][2] = -cosb*sinc;
	r[1][0] = sina*cosb+cosa*sinb*cosc;
	r[1][1] = cosa*cosb-sina*sinb*cosc;
	r[1][2] = -sinb*sinc;
	r[2][0] = cosa*sinc;
	r[2][1] = -sina*sinc;
	r[2][2] = cosc;

	// Perform the rotation to get the direction cosines at outEpoch
	for (size_t i = 0; i < 3; ++i)
	{
		x2[i] = 0.0;
		for (size_t j = 0; j < 3; ++j)
			x2[i] += r[i][j] * x1[j];
    }

    outRA = atan2(x2[1], x2[0]);
    if (outRA < 0.0)
		outRA += pi * 2;

    outDec = asin(x2[2]);

	// convert back to hours & degrees
	outCoord.delta = outDec / pi * 180;
	outCoord.alpha = outRA / pi * 12;
}

bool is_in_custom::operator () (argument_type& c) const
{
	return c.alpha > cfg->get_ra_range().first   &&   c.alpha < cfg->get_ra_range().second
			&&  c.delta > cfg->get_dec_range().first   &&   c.delta < cfg->get_dec_range().second;
}
