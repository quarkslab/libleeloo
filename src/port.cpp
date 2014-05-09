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
				case 0:
					return protocol_enum::TCP;
				default:
					return protocol_enum::UNSUPPORTED;
			}

		default:
			return protocol_enum::UNSUPPORTED;
	}
	return protocol_enum::UNSUPPORTED;
}
