#ifndef LEELOO_SORT_PERMUTE_H
#define LEELOO_SORT_PERMUTE_H

// Adapted from http://www.stanford.edu/~dgleich/notebook/2006/03/sorting_two_arrays_simultaneou.html by David Gleich
// 

#include <boost/iterator/iterator_facade.hpp>

#include <iterator>
#include <tuple>
#include <functional>

namespace leeloo {

template <class SortIter, class PermuteIter>
struct sort_permute_iter_helper_type
{
    typedef std::tuple<
        typename std::iterator_traits<SortIter>::value_type,
        typename std::iterator_traits<PermuteIter>::value_type >
        value_type;

    typedef std::tuple<
        typename std::iterator_traits<SortIter>::reference,
        typename std::iterator_traits<PermuteIter>::reference>
		ref_type;
};
 
template <class SortIter, class PermuteIter>
class sort_permute_iter
    : public boost::iterator_facade<
        sort_permute_iter<SortIter, PermuteIter>,
        typename sort_permute_iter_helper_type<
           SortIter, PermuteIter>::value_type,
        std::random_access_iterator_tag,
        typename sort_permute_iter_helper_type<
            SortIter, PermuteIter>::ref_type,
        typename std::iterator_traits<SortIter>::difference_type
    >
{
public:
	typedef typename std::iterator_traits<SortIter>::difference_type difference_type;
public:
    sort_permute_iter()
    {}

    sort_permute_iter(SortIter ci, PermuteIter vi)
        : _ci(ci), _vi(vi)
    {
    }

    SortIter _ci;
    PermuteIter _vi;


private:
    friend class boost::iterator_core_access;

    void increment()
    {
        ++_ci; ++_vi;
    }

    void decrement()
    {
        --_ci; --_vi;
    }

    bool equal(sort_permute_iter const& other) const
    {
        return (_ci == other._ci);
    }

    typename
        sort_permute_iter_helper_type<
            SortIter, PermuteIter>::ref_type dereference() const
    {
        return (typename
            sort_permute_iter_helper_type<
                SortIter, PermuteIter>::ref_type(*_ci, *_vi));
    }

    void advance(difference_type n)
    {
        std::advance(_ci, n);
        std::advance(_vi, n);
    }

    difference_type distance_to(sort_permute_iter const& other) const
    {
        return std::distance( _ci, other._ci );
    }
};


template <class SortIter, class PermuteIter>
struct sort_permute_iter_compare
    : public std::binary_function<
    typename sort_permute_iter_helper_type<
        SortIter, PermuteIter>::value_type,
    typename sort_permute_iter_helper_type<
        SortIter, PermuteIter>::value_type,
    bool>
{
    typedef
        typename sort_permute_iter_helper_type<
            SortIter, PermuteIter>::value_type T;
    inline bool operator() (const  T& t1, const T& t2)
    {
        return (std::get<0>(t1) < std::get<0>(t2));
    }
};

template <class SortIter, class PermuteIter>
sort_permute_iter<SortIter, PermuteIter>
make_sort_permute_iter(SortIter ci, PermuteIter vi)
{
    return sort_permute_iter<SortIter, PermuteIter>(ci, vi);
}; 

}

namespace std {

template <class A, class B>
void swap(std::tuple<A&, B> && a,
          std::tuple<A&, B> && b)
{
	std::swap(std::get<0>(a), std::get<0>(b));
	std::swap(std::get<1>(a), std::get<1>(b));
}

}


#endif
