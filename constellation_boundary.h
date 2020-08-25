#pragma once

#include "coordinate.h"

class constellation_boundary_line
{
private:
	double get_arc_angle(const point&) const;
	double get_line_length(const point&) const;

	void draw_line() const;
	void draw_arc() const;

public:
	coordinate	start;
	coordinate	end;
	bool		is_line;	// is this a line or an arc?

	void draw() const
	{
		if (is_line)
			draw_line();
		else
			draw_arc();
	}

	bool operator == (const constellation_boundary_line& rhs) const
	{
		return	(start == rhs.start  &&  end == rhs.end)  ||
				(start == rhs.end  &&  end == rhs.start);
	}

	bool operator != (const constellation_boundary_line& rhs) const
	{
		return !operator == (rhs);
	}
};

class constellation_boundary
{
private:
	std::vector<constellation_boundary_line>	_lines;
	std::string									_code;	// constellation code

	void add(const std::string&, const std::string&);
	void decode(const std::string&, coordinate&) const;

public:
	// reads the data from the stream (file)
	std::string read(std::istream&, const std::string&);

	void paint() const;

	// operators
	bool operator == (const constellation_boundary& rhs) const
	{
		return _code == rhs._code;
	}

	bool operator != (const constellation_boundary& rhs) const
	{
		return !operator == (rhs);
	}

	void clear()
	{
		_code.clear();
		_lines.clear();
	}

	// returns true if the constellation has line int the _lines list
	bool have_line(const constellation_boundary_line& line) const
	{
		return std::find(_lines.begin(), _lines.end(), line) != _lines.end();
	}

	// returns true if the constellation has any lines
	bool has_lines()		{ return !_lines.empty(); }
};
