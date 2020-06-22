#include <string>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>

#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include <cctype>
#include <cmath>

// using namespace std;

#include "cout_log.h"
#include "matrix.h"

template <class T>	typename T::const_iterator begin(const T& v)	{ return v.begin(); }
template <class T>	typename T::iterator begin(T& v)				{ return v.begin(); }

template <class T>	typename T::const_iterator end(const T& v)		{ return v.end(); }
template <class T>	typename T::iterator end(T& v)					{ return v.end(); }

class stopwatch_t
{
private:
	clock_t	_clock;

public:
	void start()
	{
		_clock = ::clock();
	}

	size_t get_sec()
	{
		return (::clock() - _clock) / CLOCKS_PER_SEC;
	}

	size_t get_msec()
	{
		return ::clock() - _clock;
	}
};

std::string alltrim(const std::string in);
