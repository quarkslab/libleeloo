/* 
 * Copyright (c) 2014, Quarkslab
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * - Neither the name of Quarkslab nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LEELOO_SORT_PERMUTE_H
#define LEELOO_SORT_PERMUTE_H

// Adapted from http://www.stanford.edu/~dgleich/notebook/2006/03/sorting_two_arrays_simultaneou.html by David Gleich
// 

#include <boost/iterator/iterator_facade.hpp>

#include <iterator>
#include <tuple>
#include <functional>
#include <iostream>

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
		_ci += n;
		_vi += n;
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
