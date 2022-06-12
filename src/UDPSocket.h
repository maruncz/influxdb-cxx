#ifndef __INFLUXDATA_TRANSPORTS_UDP_SOCKET_HPP__
#define __INFLUXDATA_TRANSPORTS_UDP_SOCKET_HPP__

#include "Transport.h"
#include <string>

namespace influxdb
{
    namespace transports
    {

        /// \brief UDP transport
        class UDPSocket : public Transport {

        protected:
            std::string hostname;
            int port;
            int socket_fd;

        public:
            UDPSocket(const std::string& hostname, int port);
            virtual ~UDPSocket(void);

            virtual void send(std::string&& message);
        };

    } // namespace transports
} // namespace influxdb

#endif // __INFLUXDATA_TRANSPORTS_UDP_SOCKET_HPP__
