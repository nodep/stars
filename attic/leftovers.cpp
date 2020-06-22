template <class Vector>
struct resize_vector
{
	typedef Vector&				first_argument_type;
	typedef Vector::size_type	second_argument_type;
	typedef void				result_type;

	result_type operator () (first_argument_type v, second_argument_type new_size) const
	{
		Vector::size_type current_size = v.size();
		if (current_size < new_size)
			v.insert(v.end(), new_size - current_size, Vector::value_type());
		else
			v.erase(v.end() - current_size + new_size, v.end());

		assert(v.size() == new_size);
	}
};


template <class MoveableObject>
struct moveable_objects_group
{
	permutator<MoveableObject::position_iterator>	permutate;

	vector<MoveableObject*>							all_overlaps;

	vector<MoveableObject::position_iterator>		best_positions;
	vector<MoveableObject::position_quality_type>	best_positions_quality;
	group_grader<MoveableObject>::result_type		best_position_grade;

	void clear()
	{
		permutate.clear();

		all_overlaps.clear();

		best_positions.clear();
		best_positions_quality.clear();

		best_position_grade = 0;
	}

	void insert_object(MoveableObject& obj)
	{
		permutate.add_range(obj.begin(), obj.end());
		best_positions.push_back(MoveableObject::position_iterator());
	}

	// adds all the overlapping texts that overlap eachother in any position to the permutator
	void build_group()
	{
		all_overlaps.push_back(permutate.iterators.front().get_parent());

		vector<text_object>::iterator found_text;
		for (size_t iter_index = 0; iter_index < permutate.iterators.size(); ++iter_index)
		{
			while (permutate.iterators[iter_index] != permutate.ranges[iter_index].second)
			{
				// dereference the iterator to set the text's initial position
				text_object* p = &*permutate.iterators[iter_index];

				// find a text that overlaps this one
				found_text = permutate.iterators[iter_index].overlaps_another();
				if (found_text != store->texts.end()  &&  !found_text->is_removed)
				{
					text_object* txt_ptr = &*found_text;

					// do we have this text already?
					if (find(all_overlaps.begin(), all_overlaps.end(), txt_ptr) == all_overlaps.end())
					{
						// insert the object into the group if it has not been processed
						//if (!found_text->been_processed)
							insert_object(*found_text);

						// add to all overlaps for faster text overlap detection
						all_overlaps.push_back(txt_ptr);
					}
				}

				++permutate.iterators[iter_index];
			}
		}

		// set been_processed
		for_each(permutate.iterators.begin(), permutate.iterators.end(),
							compose1(	mark_as_processed<text_object>(),
										dereference<MoveableObject::position_iterator>()));


		// re-init the permutator iterators
		permutate.reset();
	}

	bool find_best_position()
	{
		// used for debug
		size_t cnt;
		size_t total = 1;
		for (cnt = 0; cnt < permutate.iterators.size(); cnt++)
			total *= permutate.iterators[cnt]->get_valid_pos_num();

		cnt = 0;

		best_position_grade = 0;

		vector<MoveableObject::position_quality_type> curr_qtys(permutate.iterators.size());
		vector<MoveableObject*> group_ptrs(permutate.iterators.size(), 0);

		do {
			if ((total - ++cnt) % 50000 == 0)
				log_stream << total - cnt << "     \r";

			// copy all the quality grades into curr_qtys
			transform(	permutate.iterators.begin(), permutate.iterators.end(),
						curr_qtys.begin(),
						compose1(	get_quality<MoveableObject> (all_overlaps.begin(), all_overlaps.end()),
									dereference<MoveableObject::position_iterator> ()));

			// calc the group grade
			group_grader<MoveableObject>::result_type group_grade = group_grader<MoveableObject>()(curr_qtys.begin(), curr_qtys.end());

			// is this grade better?
			if (group_grade > best_position_grade)
			{
				best_position_grade = group_grade;
				best_positions_quality = curr_qtys;
				best_positions = permutate.iterators;
			}

			++permutate;
		} while (!permutate.is_end);

		return best_position_grade != 0;
	}

	void set_best_position()
	{
		assert(best_position_grade != 0);
		for_each(best_positions.begin(), best_positions.end(), dereference<MoveableObject::position_iterator>());
	}
};


template <class Iterator>
struct permutator
{
	typedef pair<Iterator, Iterator>	range_t;

	bool								is_end;

	vector<Iterator>					iterators;
	vector<range_t>						ranges;

	void add_range(Iterator begin, Iterator end)
	{
		iterators.push_back(begin);
		ranges.push_back(range_t(begin, end));
	}

	void remove_range(const size_t index)
	{
		iterators.erase(iterators.begin() + index);
		ranges.erase(ranges.begin() + index);
	}

	void reset()
	{
		// all iterators to begining positions
		transform(ranges.begin(), ranges.end(), iterators.begin(), select1st<range_t>());
	}

	void clear()
	{
		iterators.clear();
		ranges.clear();
	}

	void operator ++ ()
	{
		vector<Iterator>::iterator ii = iterators.begin();
		vector<range_t>::iterator ri = ranges.begin();
		is_end = true;	// pesimistic
		while (ii != iterators.end())
		{
			++(*ii);					// increase the iterator in the vector
			if (*ii == ri->second)		// check the range
			{
				*ii = ri->first;
			} else {
				is_end = false;
				break;
			}

			++ri;
			++ii;
		}
	}
};

template <class Iterator>
struct dereference
{
	typedef Iterator								argument_type;
	typedef iterator_traits<Iterator>::value_type&	result_type;

	result_type operator () (argument_type i) const
	{
		return *i;
	}
};

template <class Object>
struct address_of
{
	typedef Object		argument_type;
	typedef Object*		result_type;

	result_type operator () (argument_type& obj) const
	{
		return &obj;
	}
};

template <class MoveableObject>
struct get_quality
{
	typedef MoveableObject&								argument_type;
	typedef MoveableObject::position_quality_type 		result_type;

	vector<MoveableObject*>::iterator	begin_group;
	vector<MoveableObject*>::iterator	end_group;

	get_quality(vector<MoveableObject*>::iterator b, vector<MoveableObject*>::iterator e)
		: begin_group(b), end_group(e)
	{}

	result_type operator () (argument_type arg) const
	{
		return arg.get_quality(begin_group, end_group);
	}
};

template <class MoveableObject>
struct group_grader
{
	typedef size_t													result_type;
	typedef vector<MoveableObject::position_quality_type>::iterator	first_argument_type;
	typedef vector<MoveableObject::position_quality_type>::iterator	second_argument_type;

	result_type operator () (vector<MoveableObject::position_quality_type>::iterator begin_iter, vector<MoveableObject::position_quality_type>::iterator end_iter) const
	{
		size_t sum_grades = 0;

		while (begin_iter != end_iter)
		{
			const size_t curr_grade = *begin_iter;

			if (curr_grade == 0)
				return 0;

			sum_grades += curr_grade;

			++begin_iter;
		}

		return sum_grades;
	}
};


						/*
						size_t out_index = get_text_idx_t()(*out_text_iter->txt_iter);
						size_t in_index = get_text_idx_t()(*in_text_iter->txt_iter);

						//
						// increase the text pair overlap counter
						//

						group_stats_t::text_pair_t text_pair;
						text_pair.first = out_index;
						text_pair.second = in_index;

						// smaller comes first
						if (text_pair.first > text_pair.second)
							swap(text_pair.first, text_pair.second);

						// find the pair
						group_stats_t::text_pair_overlap_count_t::iterator
										srch_res = grp_stats.text_pair_overlap_count.find(text_pair);

						// found?
						if (srch_res == grp_stats.text_pair_overlap_count.end())
							grp_stats.text_pair_overlap_count[text_pair] = 1;
						else
							++srch_res->second;

						//
						// increase the individual text overlap counter
						//

						// out_text_iter
						group_stats_t::text_overlap_count_t::iterator
										srch_res2 = grp_stats.text_overlap_count.find(out_index);

						if (srch_res2 == grp_stats.text_overlap_count.end())
							grp_stats.text_overlap_count[out_index] = 1;
						else
							++srch_res2->second;

						// in_text_iter
						srch_res2 = grp_stats.text_overlap_count.find(in_index);
						if (srch_res2 == grp_stats.text_overlap_count.end())
							grp_stats.text_overlap_count[in_index] = 1;
						else
							++srch_res2->second;
						*/

/*
log_t& operator << (log_t& o, const text_group_processor_t::group_stats_t& grp_stats)
{
	// print the stats
	log_stream << "##################################\n";
	log_stream << "stats for group of: " << grp_stats.text_overlap_count.size() << "\n";
	text_group_processor_t::group_stats_t::text_overlap_count_t::const_iterator oc_iter = grp_stats.text_overlap_count.begin();
	while (oc_iter != grp_stats.text_overlap_count.end())
	{
		log_stream << "text: " << store->texts[oc_iter->first] << "\n";
		log_stream << "total overlaps in group: " << oc_iter->second << "\n";

		// find the text in the overlap pairs
		text_group_processor_t::group_stats_t::text_pair_overlap_count_t::const_iterator srch_res = grp_stats.text_pair_overlap_count.begin();
		while (srch_res != grp_stats.text_pair_overlap_count.end())
		{
			size_t found_text_index = oc_iter->first;	// :)
			if (srch_res->first.first == oc_iter->first)
				found_text_index = srch_res->first.second;
			else if (srch_res->first.second == oc_iter->first)
				found_text_index = srch_res->first.first;

			if (found_text_index != oc_iter->first)
				log_stream << "\toverlap count: " << srch_res->second << " with text: " << store->texts[found_text_index] << "\n";

			++srch_res;
		}

		++oc_iter;
	}

	log_stream << "##################################\n";

	return o;
};


struct greater_number_overlaps_t
{
	typedef const size_t	first_argument_type;
	typedef const size_t	second_argument_type;
	typedef bool			result_type;

	text_group_processor_t::group_stats_t& grp_stats;

	greater_number_overlaps_t(text_group_processor_t::group_stats_t& gs)
		: grp_stats(gs)
	{}

	result_type operator () (first_argument_type lhs, second_argument_type rhs) const
	{
		return grp_stats.text_overlap_count[lhs] < grp_stats.text_overlap_count[rhs];
	}
};
*/

/*
void text_group_processor_t::order_texts(vector<size_t>& grp_texts_idx, group_stats_t& grp_stats)
{
	// find the text with the most overlaps
	size_t max_overlaps = grp_stats.text_overlap_count.find(grp_texts_idx.front())->second;
	vector<size_t>::iterator max_overlap_iter = grp_texts_idx.begin();
	vector<size_t>::iterator txt_idx_iter = grp_texts_idx.begin() + 1;
	while (txt_idx_iter != grp_texts_idx.end())
	{
		size_t curr_overlaps = grp_stats.text_overlap_count.find(*txt_idx_iter)->second;
		if (max_overlaps < curr_overlaps)
		{
			swap(*max_overlap_iter, *txt_idx_iter);
			max_overlaps = curr_overlaps;
		}

		++txt_idx_iter;
	}

	// sort the rest of the elemets by the number of ovarlaps with previous texts
	txt_idx_iter = grp_texts_idx.begin() + 1;
	max_overlap_iter = grp_texts_idx.begin();
	while (txt_idx_iter != grp_texts_idx.end())
	{
		// find the text with maximum overlaps
		++txt_idx_iter;
	}
}
*/

	/*
	vector<size_t>::iterator text_idx_iter = grp_texts_idx.begin();
	while (text_idx_iter != grp_texts_idx.end())
	{

		matrix_t& src_matrix = matrices.back();

		log_stream << "text object: " << curr_text << "\n";

		// collect all the valid positions of this text
		vector<text_object::position_iterator> text_valid_positions;
		text_object::position_iterator txt_pos_iter = curr_text.begin();
		while (txt_pos_iter != curr_text.end())
		{
			text_valid_positions.push_back(txt_pos_iter);
			++txt_pos_iter;
		}

		bool group_too_big = false;
		if (!src_matrix.empty())
		{
			// the new matrix to copy the data to
			matrix_t dest_matrix;
			dest_matrix.set_columns(src_matrix.size_columns() + 1);

			// iterate the rows of the source matrix
			for (matrix_t::size_type row_num = 0; row_num < src_matrix.size_rows()  &&  !group_too_big; ++row_num)
			{
				// get a row from the source matrix
				pair<matrix_t::iterator, matrix_t::iterator> row_range = src_matrix.get_row(row_num);

				// iterate the valid positions of the new text
				vector<text_object::position_iterator>::iterator new_text_iter = text_valid_positions.begin();
				while (new_text_iter != text_valid_positions.end())
				{
					// find if anything overlaps
					matrix_t::iterator first = row_range.first;
					matrix_t::iterator last = row_range.second;

					while (first != last)
					{
						if (overlap_text_text()(**first, **new_text_iter))
							break;

						++first;
					}

					// no overlaps?
					if (first == last)
					{
						// add the new valid position group to the dest_matrix

						// copy the source position
						dest_matrix.push_row(row_range.first, row_range.second);

						// add the new text in it's current position
						dest_matrix.get(dest_matrix.size_rows() - 1, dest_matrix.size_columns() - 1) = *new_text_iter;

						if (dest_matrix.size() > MAX_MATRIX_SIZE)
						{
							group_too_big = true;
							break;
						}
					}

					++new_text_iter;
				}

				if (dest_matrix.empty())
					throw "<!aaaaaaaa!> no text found!!!";
			}

			// swap the src and dest matrices
			if (!group_too_big)
			{
				// swap src_matrix and dest_matrix
				src_matrix.swap(dest_matrix);
			}
		}

		if (group_too_big  ||  src_matrix.empty())
		{
			// add a default empty matrix to operate on
			if (group_too_big)
			{
				log_stream << "--- new matrix ---\n";
				matrices.push_back(matrix_t());
			}

			matrix_t& new_matrix = matrices.back();
			new_matrix.set_columns(1);

			// add all the valid positions for this new text
			vector<text_object::position_iterator>::iterator new_text_pos_iter = text_valid_positions.begin();
			while (new_text_pos_iter != text_valid_positions.end())
			{
				new_matrix.push_row(new_text_pos_iter, new_text_pos_iter + 1);
				++new_text_pos_iter;
			}
		}

		++text_idx_iter;
	}


	// get all the group text's indecies
	vector<size_t> grp_texts_idx;
	grp_txt_iter = grp.texts.begin();
	while (grp_txt_iter != grp.texts.end())
	{
		grp_texts_idx.push_back(get_text_idx_t()(**grp_txt_iter));
		++grp_txt_iter;
	}

	// sort the texts in the vector in such a way that as many 
	// invalid group positions are rejeceted as early as posible
	sort(grp_texts_idx.begin(), grp_texts_idx.end(), greater_number_overlaps_t(grp_stats));

	// init the text positions matrices
	vector<matrix_t> matrices;

	// reserve enough room for empty matrices 
	matrices.reserve(grp.texts.size());

	// add a default empty matrix
	matrices.push_back(matrix_t());

	log_stream << "------------------------------------------------------------" << "\n";
	log_stream << "number of texts in group: " << grp.texts.size() << "\n";

	if (grp.texts.size() == 7)
		log_stream << "";

	// iterate the texts
	vector<size_t>::iterator text_idx_iter = grp_texts_idx.begin();
	while (text_idx_iter != grp_texts_idx.end())
	{
		text_object& curr_text = store->texts[*text_idx_iter];

		matrix_t& src_matrix = matrices.back();

		log_stream << "text object: " << curr_text << "\n";

		// collect all the valid positions of this text
		vector<text_object::position_iterator> text_valid_positions;
		text_object::position_iterator txt_pos_iter = curr_text.begin();
		while (txt_pos_iter != curr_text.end())
		{
			text_valid_positions.push_back(txt_pos_iter);
			++txt_pos_iter;
		}

		bool group_too_big = false;
		if (!src_matrix.empty())
		{
			// the new matrix to copy the data to
			matrix_t dest_matrix;
			dest_matrix.set_columns(src_matrix.size_columns() + 1);

			// iterate the rows of the source matrix
			for (matrix_t::size_type row_num = 0; row_num < src_matrix.size_rows()  &&  !group_too_big; ++row_num)
			{
				// get a row from the source matrix
				pair<matrix_t::iterator, matrix_t::iterator> row_range = src_matrix.get_row(row_num);

				// iterate the valid positions of the new text
				vector<text_object::position_iterator>::iterator new_text_iter = text_valid_positions.begin();
				while (new_text_iter != text_valid_positions.end())
				{
					// find if anything overlaps
					matrix_t::iterator first = row_range.first;
					matrix_t::iterator last = row_range.second;

					while (first != last)
					{
						if (overlap_text_text()(**first, **new_text_iter))
							break;

						++first;
					}

					// no overlaps?
					if (first == last)
					{
						// add the new valid position group to the dest_matrix

						// copy the source position
						dest_matrix.push_row(row_range.first, row_range.second);

						// add the new text in it's current position
						dest_matrix.get(dest_matrix.size_rows() - 1, dest_matrix.size_columns() - 1) = *new_text_iter;

						if (dest_matrix.size() > MAX_MATRIX_SIZE)
						{
							group_too_big = true;
							break;
						}
					}

					++new_text_iter;
				}

				if (dest_matrix.empty())
					throw "<!aaaaaaaa!> no text found!!!";
			}

			// swap the src and dest matrices
			if (!group_too_big)
			{
				// swap src_matrix and dest_matrix
				src_matrix.swap(dest_matrix);
			}
		}

		if (group_too_big  ||  src_matrix.empty())
		{
			// add a default empty matrix to operate on
			if (group_too_big)
			{
				log_stream << "--- new matrix ---\n";
				matrices.push_back(matrix_t());
			}

			matrix_t& new_matrix = matrices.back();
			new_matrix.set_columns(1);

			// add all the valid positions for this new text
			vector<text_object::position_iterator>::iterator new_text_pos_iter = text_valid_positions.begin();
			while (new_text_pos_iter != text_valid_positions.end())
			{
				new_matrix.push_row(new_text_pos_iter, new_text_pos_iter + 1);
				++new_text_pos_iter;
			}
		}

		++text_idx_iter;
	}

struct less_by_bound_to
{
	typedef vector<text_object>::iterator	first_argument_type;
	typedef vector<text_object>::iterator	second_argument_type;
	typedef bool							result_type;

	result_type operator () (first_argument_type lhs, second_argument_type rhs) const
	{
		return lhs->bound_to < rhs->bound_to;
	}
};
	*/

	/*
	// see which members of the group we'll use
	typedef enum {e_all, e_no_perfect, e_no_free} group_inclusion_criteria_t;
	group_inclusion_criteria_t group_criteria;

	const size_t MAX_GROUP_MEMBERS = 8;

	if (group.size() <= MAX_GROUP_MEMBERS)
		group_criteria = e_all;
	else if (total_non_perfect_cnt <= MAX_GROUP_MEMBERS)
		group_criteria = e_no_perfect;
	else
		group_criteria = e_no_free;

	// !!! debug output
	log_stream << "inclusion criteria: ";
	switch (group_criteria)
	{
	case e_all:			log_stream << "e_all\n";			break;
	case e_no_perfect:	log_stream << "e_no_perfect\n";		break;
	case e_no_free:		log_stream << "e_no_free\n";		break;
	}
	// !!!
	*/


/*
void process::find_text_positions()
{
	stopwatch_t swatch;
	swatch.start();

	log_stream << "finding text positions...\n";

	// the group object we'll use
	moveable_objects_group<text_object> group;

 	vector<text_object>::iterator text_iter = store->texts.begin();
	while (text_iter != store->texts.end())
	{
		if (!text_iter->been_processed  &&  !text_iter->is_removed)
		{
			group.insert_object(*text_iter);

			// add all the texts that this text overlaps
			group.build_group();

			while (group.permutate.iterators.size() != 0)
			{
				if (group.find_best_position())
				{
					group.set_best_position();
					break;
				}

				// we have to remove a text from the group

				// !!! for now, just remove the longest (by strlen()) text
				size_t max_text_len = 0;
				size_t curr_text_len = 0;
				text_object* curr_text;
				text_object* max_text = 0;
				vector<text_object::position_iterator>::iterator pos_texts_iter = group.permutate.iterators.begin();
				vector<text_object::position_iterator>::iterator max_text_iter;
				while (pos_texts_iter != group.permutate.iterators.end())
				{
					curr_text = pos_texts_iter->get_parent();
					curr_text_len = curr_text->text.length();
					if (curr_text_len > max_text_len)
					{
						max_text_len = curr_text_len;
						max_text_iter = pos_texts_iter;
						max_text = curr_text;
					}

					++pos_texts_iter;
				}

				assert(max_text_len != 0);

				// remove max_text_iter from the group
				group.permutate.remove_range(max_text_iter - group.permutate.iterators.begin());

				// also remove from all_overlaps
				vector<text_object*>::iterator erase_iter = find(group.all_overlaps.begin(), group.all_overlaps.end(), max_text);
				assert(erase_iter != group.all_overlaps.end());

				// warning
				log_stream << "WARNING: Text <" << (*erase_iter)->text << "> had to be removed from the map during positions searching.\n";

				(*erase_iter)->is_removed = true;
				group.all_overlaps.erase(erase_iter);

				// remove from and best_positions (any iter, since they will be re-inited in the next cycle anyway)
				group.best_positions.pop_back();
			}


			group.clear();
		}

		++text_iter;
	}

	log_stream << "time taken (ms): " << swatch.get_msec() << "\n";
}
*/

	/*
	{

	// find valid positions for groups
	typedef vector<text_object::position_iterator>		group_positions_t;
	typedef vector<group_positions_t>					valid_positions_t;

	valid_positions_t valid_positions;
	valid_positions_t valid_positions_temp;

	// iterate the groups
	vector<group_t>::iterator grp_iter = groups.begin();
	while (grp_iter != groups.end())
	{
		log_stream << "--------------------------------\n";
		log_stream << "text in group: " << grp_iter->texts.size() << "\n";

		valid_positions.clear();
		valid_positions_temp.clear();

		// iterate texts in *grp_iter
		set<vector<text_object>::iterator>::iterator txt_iter = grp_iter->texts.begin();

		// add all the valid positions for the first text in the group
		group_positions_t grp_pos;
		grp_pos.push_back(text_object::position_iterator());
		text_object::position_iterator first_pos_iter = (*txt_iter)->begin();
		while (first_pos_iter != (*txt_iter)->end())
		{
			grp_pos.front() = first_pos_iter;
			valid_positions.push_back(grp_pos);
			++first_pos_iter;
		}

		log_stream << **txt_iter << "\n";

		// continue with the next text
		++txt_iter;

		while (txt_iter != grp_iter->texts.end())
		{
			log_stream << **txt_iter << "\n";

			// iterate the positions of *txt_iter
			text_object::position_iterator txt_pos_iter = (*txt_iter)->begin();
			while (txt_pos_iter != (*txt_iter)->end())
			{
				// put the text in it's position
				*txt_pos_iter;

				// iterate groups of valid positions of this group
				valid_positions_t::iterator src_pos_iter = valid_positions.begin();
				while (src_pos_iter != valid_positions.end())
				{
					// iterate through texts in this valid position
					group_positions_t::iterator valid_grp_iter = src_pos_iter->begin();
					while (valid_grp_iter != src_pos_iter->end())
					{
						// put the text in it's position
						**valid_grp_iter;

						// check for overlap of *txt_iter with all the positions in *src_pos_iter
						if (overlap_text_text()(**txt_iter, *valid_grp_iter->get_parent()))
							break;

						++valid_grp_iter;
					}

					// group valid with **txt_iter in current position?
					if (valid_grp_iter == src_pos_iter->end())
					{
						// copy the complete new position group to valid_positions_temp
						valid_positions_temp.push_back(*src_pos_iter);
						valid_positions_temp.back().push_back(txt_pos_iter);
					}

					++src_pos_iter;
				}

	 			++txt_pos_iter;
			}

			// swap valid_positions and valid_positions_temp
			swap(valid_positions, valid_positions_temp);

			log_stream << "valid positions: " << valid_positions.size() << "\n";

			++txt_iter;
		}

		log_stream << "number of valid positions: " << valid_positions.size() << "\n";

		if (valid_positions.size() == 0)
		{
			std::string str;
			cin >> str;
		}

		++grp_iter;
	}

	}
	*/


	/*
	// connection graph

	// sort all the connections
	sort(connections.begin(), connections.end());

	// remove the duplicates
	vector<connection_t>::iterator new_end = unique(connections.begin(), connections.end());
	connections.erase(new_end, connections.end());

	// plot all the lines
	canvas->set_pen_direct(thin_solid);
	conn_iter = connections.begin();
	while (conn_iter != connections.end())
	{
		canvas->line(conn_iter->first->bound_to, conn_iter->second->bound_to);
		++conn_iter;
	}

	log_stream << "time taken: " << swatch.get_msec() / 1000.0 << "sec\n";
	*/
