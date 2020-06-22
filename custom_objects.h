#pragma once

struct custom_object
{
	std::string		sign_type;
	coordinate		coord;
	std::string		name;
	double			diameter;
	double			text_height;

	custom_object()
		: diameter(1.5)
	{}


	static custom_object import(const std::string& in);
};
