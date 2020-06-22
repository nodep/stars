#pragma once

// simple, rectangle, dense, row major, generic matrix
template <class T>
class matrix
{
	template <typename T>
	friend void save(const matrix<T>& m, const std::string& fname);

	template <typename T>
	friend void load(matrix<T>& m, const std::string& fname);

public:
	typedef T			value_type;
	typedef T*			pointer;
	typedef const T&	const_reference;
	typedef T&			reference;
	typedef size_t		size_type;
	typedef ptrdiff_t	distance_type;

private:
	std::vector<T>		_data;

	size_type			_rows;
	size_type			_columns;

	void _repack(size_type new_cols)
	{
		// any data to repack?
		if (_columns == 0  ||  _rows == 0)
			return;

		// reserve the memory in _data
		_data.reserve(_rows * new_cols);

		// copy the existing data
		if (new_cols > _columns)
		{
			// insert new elements into the matrix so that the vector's size() equals matrix's size()
			_data.insert(_data.end(), (new_cols - _columns) * _rows, T());

			for (size_type row_cnt = _rows - 1; row_cnt > 0; --row_cnt)
			{
				// SGI STL uses memmove() in copy() -- this would not work with memcpy()
				std::copy(	_data.begin() + row_cnt * _columns,
							_data.begin() + (row_cnt + 1) * _columns,
							_data.begin() + row_cnt * new_cols);
			}
		} else {
			for (size_type row_cnt = 1; row_cnt < _rows; ++row_cnt)
			{
				std::copy(	_data.begin() + row_cnt * _columns,
							_data.begin() + (row_cnt + 1) * _columns,
							_data.begin() + row_cnt * new_cols);
			}
		}
	}

public:
	typedef const value_type*	const_iterator;
	typedef value_type*			iterator;

	explicit matrix(size_type c = 0)
		: _rows(0), _columns(c)
	{
		// reserve all the memory at once
		_data.reserve(max_matrix_mem_usage / sizeof(T));
	}

	void set_columns(size_type c)
	{
		if (c == _columns)
			return;

		_repack(c);
		_columns = c;
	}

	void inc_columns(size_type add_col = 1)
	{
		set_columns(_columns + 1);
	}

	iterator begin()				{ return &*_data.begin(); }
	const_iterator begin() const	{ return _data.begin(); }

	iterator end()					{ return _data.end(); }
	const_iterator end() const		{ return _data.end(); }

	// added
	iterator get_pos(size_type row, size_type col)
	{ return iterator(this, _data.begin() + row * _columns + col); }

	const_iterator get_pos(size_type row, size_type col) const
	{ return const_iterator(this, _data.begin() + row * _columns + col); }


	void reserve(size_type rows)
	{
		_data.reserve(rows * _columns);
	}

	template <class ForwardIterator>
	void push_row(ForwardIterator first, ForwardIterator last)
	{
		// add the available elements
		size_type col_cnt = 0;
		while (first != last  &&  col_cnt < _columns)
		{
			_data.push_back(*first);
			++col_cnt;
			++first;
		}

		if (_columns > col_cnt)
			_data.insert(_data.end(), _columns - col_cnt, T());

		++_rows;
	}

	size_type size() const				{ return _rows * _columns; }
	size_type size_rows() const			{ return _rows; }
	size_type size_columns() const		{ return _columns; }

	bool empty() const					{ return _rows == 0; }

	void clear()
	{
		_data.clear();
		_columns = _rows = 0;
	}

	const_reference get(size_type row, size_type col) const
	{
		assert(row < _rows  &&  col < _columns);
		return *(_data.begin() + row * _columns + col);
	}

	std::pair<iterator, iterator> get_row(const size_type row)
	{
		assert(row < _rows);
		iterator start = &*(_data.begin() + _columns * row);
		iterator end   = start + _columns;
		return std::pair<iterator, iterator>(start, end);
	}

	std::pair<const_iterator, const_iterator> get_row(const size_type row) const
	{
		assert(row < _rows);
		const iterator start = &*(_data.begin() + _columns * row);
		const iterator end = start + _columns;
		return std::pair<const_iterator, const_iterator>(start, end);
	}

	reference get(size_type row, size_type col)
	{
		assert(row < _rows  &&  col < _columns);
		return *(_data.begin() + row * _columns + col);
	}

	void swap(matrix& c)
	{
		std::swap(_data, c._data);

		std::swap(_rows, c._rows);
		std::swap(_columns, c._columns);
	}
};

template <class T>
inline std::ostream& operator << (std::ostream& o, const matrix<T>& m)
{
	o << "matrix rows=" << m.size_rows() << " cols=" << m.size_columns()
		<< "  size=" << m.size() << "(elem)  " << sizeof(text_object::position_iterator) * m.size() / 1024.0 / 1024 << "(MB)\n";

	/*
	for (size_t row_cnt = 0; row_cnt < m.size_rows(); ++row_cnt)
	{
		for (size_t col_cnt = 0; col_cnt < m.size_columns(); ++col_cnt)
		{
			o << m.get(row_cnt, col_cnt);
			if (col_cnt == m.size_columns() - 1)
				o << std::endl;
			else
				o << "\t";
		}
	}

	return o;
	*/
}
