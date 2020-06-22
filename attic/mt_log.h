#pragma once

#include <windows.h>
#include <gwin>

struct log_t
{
	gwin::mutex			sync_mutex;
	std::stringstream	sstr;

	/*
	log_t& operator << (const std::string& str)
	{
		sstr << str;
		return *this;
	}
	*/

	std::string get_log()
	{
		gwin::lock lm(sync_mutex);
		return sstr.str();
	}
};

/*
template <class UnkownType>
inline log_t& operator << (log_t& l, UnkownType& obj)
{
	l.sstr << obj;
	return l;
}
*/

inline log_t& operator << (log_t& l, const char* cstr)
{
	gwin::lock lm(l.sync_mutex);
	l.sstr << cstr;
	return l;
}

inline log_t& operator << (log_t& l, const char chr)
{
	gwin::lock lm(l.sync_mutex);
	l.sstr << chr;
	return l;
}

inline log_t& operator << (log_t& l, const string& str)
{
	gwin::lock lm(l.sync_mutex);
	l.sstr << str;
	return l;
}

inline log_t& operator << (log_t& l, const double& dbl)
{
	gwin::lock lm(l.sync_mutex);
	l.sstr << dbl;
	return l;
}

inline log_t& operator << (log_t& l, const int i)
{
	gwin::lock lm(l.sync_mutex);
	l.sstr << i;
	return l;
}

inline log_t& operator << (log_t& l, const size_t ui)
{
	gwin::lock lm(l.sync_mutex);
	l.sstr << ui;
	return l;
}

extern log_t log_stream;
