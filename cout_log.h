#pragma once

struct log_t
{};

inline log_t& operator << (log_t& l, const char* cstr)
{
	std::cout << cstr << std::flush;
	return l;
}

inline log_t& operator << (log_t& l, const char chr)
{
	std::cout << chr << std::flush;
	return l;
}

inline log_t& operator << (log_t& l, const std::string& str)
{
	std::cout << str << std::flush;
	return l;
}

inline log_t& operator << (log_t& l, const double& dbl)
{
	std::cout << dbl << std::flush;
	return l;
}

inline log_t& operator << (log_t& l, const int i)
{
	std::cout << i << std::flush;
	return l;
}

inline log_t& operator << (log_t& l, const __int64 i64)
{
	std::cout << i64 << std::flush;
	return l;
}

inline log_t& operator << (log_t& l, const size_t ui)
{
	std::cout << ui << std::flush;
	return l;
}

extern log_t log_stream;
