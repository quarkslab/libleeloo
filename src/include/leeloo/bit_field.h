#ifndef LEELOO_BIT_FIELD_H
#define LEELOO_BIT_FIELD_H

#include <boost/integer/static_log2.hpp>
#include <memory>
#include <cstring>

namespace leeloo {

class bit_field
{
public:
	typedef size_t integer_type;
	typedef std::allocator<integer_type> allocator_type;
	typedef size_t size_type;

private:
	static constexpr size_type bits_per_chunk = sizeof(integer_type)*8;
	static constexpr integer_type mask_chunk_bit = bits_per_chunk-1;
	static constexpr integer_type one = 1;
	static constexpr int ln2_bits_per_chunk = boost::static_log2<bits_per_chunk>::value;

public:
	bit_field():
		_buf(nullptr),
		_size(0)
	{ }

	bit_field(size_t nbits):
		_buf(nullptr),
		_size(0)
	{
		reserve(nbits);
	}

	~bit_field()
	{
		if (_buf) {
			allocator_type().deallocate(_buf, _size);
		}
	}

	bit_field(bit_field const& o)
	{
		copy(o);
	}

	bit_field(bit_field&& o)
	{
		move(std::move(o));
	}

public:
	void clear_all()
	{
		if (size_chunks() == 0 || buffer() == nullptr) {
			return;
		}
		memset(buffer(), 0, sizeof(integer_type)*size_chunks());
	}

	void set_all()
	{
		if (_size == 0 || _buf == nullptr) {
			return;
		}
		memset(buffer(), 0xFF, sizeof(integer_type)*size_chunks());
	}

	void set_all_odd()
	{
		if (_size == 0 || _buf == nullptr) {
			return;
		}
		memset(buffer(), 0xAA, sizeof(integer_type)*size_chunks());
	}

	void set_all_even()
	{
		if (_size == 0 || _buf == nullptr) {
			return;
		}
		memset(buffer(), 0x55, sizeof(integer_type)*size_chunks());
	}

	inline void reserve(size_type const nbits)
	{
		const size_type req_size = bit_count_to_chunk_count(nbits);
		if (req_size <= size_chunks()) {
			return;
		}
		if (buffer() == nullptr) {
			_buf = allocator_type().allocate(req_size);
			if (_buf == nullptr) {
				throw std::bad_alloc();
			}
			_size = req_size;
			return;
		}

		const size_type new_size = std::max(size_chunks() + 64, req_size);
		integer_type* new_buf = allocator_type().allocate(new_size);
		if (new_buf == nullptr) {
			throw std::bad_alloc();
		}
		memcpy(new_buf, buffer(), size_chunks());
		allocator_type().deallocate(buffer(), size_chunks());
		_size = new_size;
		_buf = new_buf;
	}

	inline void set_bit(size_type const idx)
	{
		reserve(idx+1);
		set_bit_fast(idx);
	}

	inline void clear_bit(size_type const idx)
	{
		reserve(idx+1);
		clear_bit_fast(idx);
	}

	inline bool get_bit(size_type const idx) const
	{
		if (idx >= size_bits()) {
			return false;
		}
		return get_bit_fast(idx);
	}

	inline void set_bit_fast(size_type const idx)
	{
		chunk_at(bit_index_to_chunk(idx)) |= one<<(bit_index_to_chunk_bit(idx));
	}

	inline void clear_bit_fast(size_type const idx)
	{
		chunk_at(bit_index_to_chunk(idx)) &= ~(one<<(bit_index_to_chunk_bit(idx)));
	}

	inline bool get_bit_fast(size_type const idx) const
	{
		return chunk_at(bit_index_to_chunk(idx)) & (one<<(bit_index_to_chunk_bit(idx)));
	}

public:
	bit_field& operator=(bit_field const& o)
	{
		if (&o != this) {
			copy(o);
		}
		return *this;
	}

	bit_field& operator=(bit_field&& o)
	{
		if (&o != this) {
			move(std::move(o));
		}
		return *this;
	}

private:
	inline integer_type* buffer() { return _buf; }
	inline integer_type const* buffer() const { return _buf; }

	inline integer_type& chunk_at(size_type const n) { return buffer()[n]; }
	inline integer_type const& chunk_at(size_type const n) const { return buffer()[n]; }

	inline size_type size_chunks() const { return _size; }
	inline size_type size_bits() const { return size_chunks()*bits_per_chunk; }

private:
	void copy(bit_field const& o)
	{
		_buf = allocator_type().allocate(o.size_chunks());
		if (_buf == nullptr) {
			throw std::bad_alloc();
		}
		_size = size_chunks();
		memcpy(buffer(), o.buffer(), size_chunks());
	}

	void move(bit_field&& o)
	{
		_buf = o._buf;
		_size = o._size;
		o._buf = nullptr;
		o._size = 0;
	}

private:
	static inline size_type bit_index_to_chunk(size_type const n) { return n >> ln2_bits_per_chunk; }
	static inline size_type bit_index_to_chunk_bit(size_type const n) { return n & mask_chunk_bit; }
	static inline size_type chunk_to_bit_index(size_type const n) { return n << ln2_bits_per_chunk; }
	static inline size_type bit_count_to_chunk_count(size_type const nbits) { return (nbits+bits_per_chunk-1)/bits_per_chunk; }

private:
	integer_type* _buf;
	size_t _size;
};

}

#endif
