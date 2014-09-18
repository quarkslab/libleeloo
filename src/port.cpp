#include <leeloo/port.h>

#include <sys/socket.h>
#include <netinet/in.h>

leeloo::port::protocol_enum leeloo::port::protocol_from_socket_type(int type, int proto)
{
	switch (type) {
		case SOCK_DGRAM:
			return protocol_enum::UDP;

		case SOCK_STREAM:
			switch (proto) {
				case IPPROTO_SCTP:
					return protocol_enum::SCTP;
				case IPPROTO_TCP:
					return protocol_enum::TCP;
				default:
					return protocol_enum::UNSUPPORTED;
			}

		default:
			return protocol_enum::UNSUPPORTED;
	}
	return protocol_enum::UNSUPPORTED;
}

int leeloo::port::socket_type() const
{
	switch (protocol()) {
		case static_cast<uint16_t>(protocol_enum::TCP):
		case static_cast<uint16_t>(protocol_enum::SCTP):
			return SOCK_STREAM;
		case static_cast<uint16_t>(protocol_enum::UDP):
			return SOCK_DGRAM;
		default:
			return -1;
	}

	return -1;
}

int leeloo::port::socket_proto() const
{
	switch (protocol()) {
		case static_cast<uint16_t>(protocol_enum::TCP):
		case static_cast<uint16_t>(protocol_enum::UDP):
			return 0;
		case static_cast<uint16_t>(protocol_enum::SCTP):
			return IPPROTO_SCTP;
	}

	return -1;
}

const char* leeloo::port::protocol_name(uint16_t proto)
{
	switch (proto) {
		case static_cast<uint16_t>(protocol_enum::TCP):
			return "TCP";
		case static_cast<uint16_t>(protocol_enum::UDP):
			return "UDP";
		case static_cast<uint16_t>(protocol_enum::SCTP):
			return "SCTP";
	}

	return "unknown";
}
