#pragma once

struct asterism_line 
{
	struct is_valid
	{
		typedef asterism_line	argument_type;
		typedef bool			result_type;

		result_type operator () (const argument_type& arg) const
		{
			return arg.start_star != 0  &&  arg.end_star != 0;
		}
	};

	asterism_line()
		:	start_star(0), end_star(0)
	{}

	const star* start_star;
	const star* end_star;

	std::string	start_desgn;
	std::string	end_desgn;

};
