#include <leeloo/bit_field.h>
#include <stdlib.h>

void leeloo::bit_field::set_random()
{
	for (size_type i = 0; i < size_chunks(); i++) {
		_buf[i] = rand()*rand();
	}
}
