#pragma once

typedef enum
{
	black,
	gray90, 
	gray80, 
	grey70, 
	grey60, 
	grey50, 
	grey40, 
	grey30, 
	grey20, 
	grey10, 
	white, 

	red,		// not used ATM
	green,
	blue,
} color_t;

typedef enum
{
	thin_solid,
	thin_dots,
	thin_dashed,
	thick_solid,
	other
} pen_t;
