///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "InfluxDBFactory.h"
#include <functional>
#include <string>
#include <memory>
#include <map>
#include "UriParser.h"
#include "InfluxDBException.h"

#ifdef INFLUXDB_WITH_CURL
#include "HTTP.h"
#else
#include "HttpClientWrapper.h"
#endif

#ifdef INFLUXDB_WITH_BOOST
#include "UDP.h"
#include "UnixSocket.h"
#else
#include "UDPSocket.h"
#endif

#ifdef LIB_NAMESPACE
namespace LIB_NAMESPACE {
#else
namespace influxdb {
#endif

#ifdef INFLUXDB_WITH_BOOST
std::unique_ptr<Transport> withUdpTransport(const http::url& uri) {
  return std::make_unique<transports::UDP>(uri.host, uri.port);
}
std::unique_ptr<Transport> withUnixSocketTransport(const http::url& uri) {
  return std::make_unique<transports::UnixSocket>(uri.path);
}
#else
std::unique_ptr<Transport> withUdpTransport(const http::url& uri) {
  return std::make_unique<transports::UDPSocket>(uri.host, uri.port);
}
std::unique_ptr<Transport> withUnixSocketTransport(const http::url& /*uri*/) {
  throw InfluxDBException("InfluxDBFactory", "Unix socket transport requires Boost");
}
#endif

#ifdef INFLUXDB_WITH_CURL
std::unique_ptr<Transport> withHttpTransport(const http::url& uri) {
  auto transport = std::make_unique<transports::HTTP>(uri.full_url);
  if (!uri.user.empty()) {
    transport->enableBasicAuth(uri.user + ":" + uri.password);
  }
  if (uri.protocol == "https") {
    transport->enableSsl();
  }
  return transport;
}
std::unique_ptr<Transport> withHttpsTransport(const http::url& uri) {
  return withHttpTransport(uri);
}
#else
std::unique_ptr<Transport> withHttpTransport(const http::url& uri) {
  return std::make_unique<transports::HttpClientWrapper>(uri.full_url);
}
std::unique_ptr<Transport> withHttpsTransport(const http::url& /*uri*/) {
  throw InfluxDBException("InfluxDBFactory", "HTTPStransport requires curl");
}
#endif

std::unique_ptr<Transport> InfluxDBFactory::GetTransport(std::string url) {
  static const std::map<std::string, std::function<std::unique_ptr<Transport>(const http::url&)>> map = {
    {"udp", withUdpTransport},
    {"http", withHttpTransport},
    {"https", withHttpsTransport},
    {"unix", withUnixSocketTransport},
  };

  http::url parsedUrl = http::ParseHttpUrl(url);
  if (parsedUrl.protocol.empty()) {
    throw InfluxDBException("InfluxDBFactory::GetTransport", "Ill-formed URI");
  }

  auto iterator = map.find(parsedUrl.protocol);
  if (iterator == map.end()) {
    throw InfluxDBException("InfluxDBFactory::GetTransport", "Unrecognized backend " + parsedUrl.protocol);
  }

  return iterator->second(parsedUrl);
}

std::unique_ptr<InfluxDB> InfluxDBFactory::Get(std::string url)
{
  return std::make_unique<InfluxDB>(InfluxDBFactory::GetTransport(url));
}

} // namespace influxdb
