///
/// \author Adam Wegrzynek
///

#ifndef INFLUXDATA_TRANSPORTS_UDP_H
#define INFLUXDATA_TRANSPORTS_UDP_H

#ifdef INFLUXDB_WITH_BOOST

#include "Transport.h"

#include <boost/asio.hpp>
#include <chrono>
#include <string>

#ifdef LIB_NAMESPACE
namespace LIB_NAMESPACE {
#else
namespace influxdb {
#endif
namespace transports {

/// \brief UDP transport
class UDP : public Transport
{
  public:
    /// Constructor
    UDP(const std::string &hostname, int port);

    /// Default destructor
    ~UDP() = default;
 
    /// Sends blob via UDP
    void send(std::string&& message) override;   

  private:
    /// Boost Asio I/O functionality
    boost::asio::io_service mIoService;

    /// UDP socket
    boost::asio::ip::udp::socket mSocket;

    /// UDP endpoint
    boost::asio::ip::udp::endpoint mEndpoint;

};

} // namespace transports
} // namespace influxdb

#endif // INFLUXDB_WITH_BOOST
#endif // INFLUXDATA_TRANSPORTS_UDP_H
