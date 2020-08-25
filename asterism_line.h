#pragma once

struct asterism_line 
{
	asterism_line()
	:	start_star(0), end_star(0)
	{}

	bool is_valid() const
	{
		return start_star != 0 && end_star != 0;
	}

	const star* start_star;
	const star* end_star;

	std::string	start_desgn;
	std::string	end_desgn;
};
