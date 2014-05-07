#ifndef LEELOO_PORT_H
#define LEELOO_PORT_H

#include <cstdint>

namespace leeloo {

class port
{
public:
	enum class protocol_enum : uint16_t {
		TCP =  0x0100,
		UDP =  0x0200,
		SCTP = 0x0300
	};

public:
	port(uint16_t value, protocol_enum protocol)
	{
		_port.s.value = value;
		_port.s.protocol = (uint16_t) protocol;
	}

	port(uint32_t v)
	{
		_port.i32 = v;
	}

public:
	inline uint16_t value() const { return _port.s.value; }
	inline uint16_t protocol() const { return _port.s.protocol; }

	inline uint16_t& value() { return _port.s.value; }
	inline uint16_t& protocol() { return _port.s.protocol; }

	inline uint32_t as_u32() const { return _port.i32; }
	inline operator uint32_t() const { return as_u32(); }

private:
	union {
		struct {
			uint16_t value;
			uint16_t protocol;
		} s;
		uint32_t i32;
	} _port;
};

}

#endif
