#include <map>

struct star
{
    double  diameter;

    enum { objectID = 1 };
};

struct galaxy
{
    double  diameter;

    enum { objectID = 2 };
};

template <typename ObjectType>
struct mag_storage
{
    double min_magnitude;
    double max_magnitude;
};

struct storage
{
    typedef std::map<size_t, std::pair<double, double> >    mag_range_storage_t;
    mag_range_storage_t  mag_storage;

    std::pair<double, double>& get_object_mag_range(const size_t& oid)
    {
        mag_range_storage_t::iterator srch_res(mag_storage.find(oid));
        if (srch_res == mag_storage.end())
            return *(mag_storage.insert(mag_range_storage_t::value_type(oid, std::make_pair(1e9, -1e9))).first);
            //std::pair<double, double> a = *(mag_storage.insert(mag_range_storage_t::value_type(oid, std::make_pair(1e9, -1e9))).first);

        return srch_res->second;
    }

    double get_minmag(const size_t& oid)
    {
        mag_range_storage_t::iterator srch_res(mag_storage.find(oid));
        if (srch_res == mag_storage.end())
            return mag_storage.insert(mag_range_storage_t::value_type(oid, std::make_pair(1e9, -1e9))).first->first;

        return srch_res->second.first;
    }

    double get_maxmag(const size_t& oid)
    {
        mag_range_storage_t::iterator srch_res(mag_storage.find(oid));
        if (srch_res == mag_storage.end())
            return mag_storage.insert(mag_range_storage_t::value_type(oid, std::make_pair(1e9, -1e9))).first->second;

        return srch_res->second.second;
    }

    template <typename T>
    double get_min_magnitude()
    {
        return get_minmag(T::objectID());
    }

    template <typename T>
    double get_max_magnitude()
    {
        return get_maxmag(T::objectID());
    }
};

struct update_diameter
{
    void operator () () const
    {
    }
};

int main()
{
    return 0;
}
