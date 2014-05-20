#ifndef LEELOO_PORT_H
#define LEELOO_PORT_H

#include <cstdint>
#include <leeloo/exports.h>

namespace leeloo {

class LEELOO_API port
{
public:
	enum class protocol_enum : uint16_t {
		TCP =  0x0100,
		UDP =  0x0200,
		SCTP = 0x0300,
		UNSUPPORTED = 0xFFFF
	};

public:
	port()
	{ }

	port(uint16_t value, protocol_enum protocol)
	{
		_port.s.value = value;
		_port.s.protocol = (uint16_t) protocol;
	}

	explicit port(uint32_t v)
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

	int socket_type() const;
	int socket_proto() const;

	static protocol_enum protocol_from_socket_type(int type, int protocol);
	static const char* protocol_name(uint16_t proto);

	inline bool operator==(port const& o) const { return as_u32() == o.as_u32(); }
	inline bool operator!=(port const& o) const { return as_u32() != o.as_u32(); }
	inline bool operator<(port const& o) const  { return as_u32() <  o.as_u32(); }

private:
	union {
		struct {
			uint16_t value;
			uint16_t protocol;
		} s;
		uint32_t i32;
	} _port;
};

// Helper functions
static inline port tcp_port(uint16_t value)  { return port(value, port::protocol_enum::TCP); }
static inline port udp_port(uint16_t value)  { return port(value, port::protocol_enum::UDP); }
static inline port sctp_port(uint16_t value) { return port(value, port::protocol_enum::SCTP); }

}

#endif
