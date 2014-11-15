/* 
 * Copyright (c) 2013-2014, Quarkslab
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

#include <leeloo/interval.h>
#include <leeloo/list_intervals.h>

#ifdef LEELOO_BOOST_SERIALIZE
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#endif

#include <iostream>
#include <sstream>

template <class Interval>
void print_intervals(Interval const& l)
{
	typedef typename Interval::interval_type interval_type;
	for (interval_type const& i: l.intervals()) {
		std::cout << i.lower() << " " << i.upper() << std::endl;
	}
}

// Interval of type [a,b[
typedef leeloo::list_intervals<leeloo::interval<uint32_t>, uint32_t> list_intervals;

int main()
{
	FILE* f_tmp = tmpfile();

	list_intervals ref;
	ref.add(0, 2);
	ref.add(5, 9);
	ref.add(1, 5);
	ref.add(8, 9);
	ref.add(9, 15);
	ref.add(19, 21);
	ref.aggregate();
	ref.dump_to_file(f_tmp);

	if (fseek(f_tmp, 0, SEEK_SET) == -1) {
		perror("fseek");
		return 1;
	}

	list_intervals list;
	list.read_from_file(f_tmp);
	if (ref != list) {
		std::cerr << "Read after dump does not give the same result!" << std::endl;
		return 1;
	}

	fclose(f_tmp);

	{
		std::stringstream ss;
		ref.dump_stream(ss);

		list_intervals list_ss;
		ss.seekg(0, std::stringstream::beg);
		list_ss.read_stream(ss);

		if (ref != list_ss) {
			std::cerr << "Deserialize after serialize with std::stream didn't give the same result!" << std::endl;
			return 1;
		}
	}

#ifdef LEELOO_BOOST_SERIALIZE
	// Serialisation with boost::archive
	{
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		oa << ref;

		list_intervals list_boost;
		ss.seekg(0, std::stringstream::beg);
		boost::archive::text_iarchive ia(ss);
		ia >> list_boost;
		if (ref != list_boost) {
			std::cerr << "Deserialize after serialize didn't give the same result!" << std::endl;
			return 1;
		}
	}
#endif

	return 0;
}
