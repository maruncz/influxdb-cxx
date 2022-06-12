# influxdb-cxx-fork

This is a fork of the now archived influxdb access library writtren by https://github.com/awegrzyn/influxdb-cxx.git. The goal of this fork is to remove the dependencies to boost and curl. For a number of IoT applications there is no need for such a complex configuration.

THIS IS WORK IN PROGRESS; PLEASE WAIT UNTIL IT IS DONE!!!

InfluxDB C++ client library
 - Batch write
 - Data exploration
 - Supported transports
   - HTTP/HTTPS with Basic Auth
   - UDP
   - Unix datagram socket


## Installation

 __Build requirements__
 - CMake 3.12+
 - C++17 compliler

### Generic
 ```bash
git clone https://github.com/awegrzyn/influxdb-cxx.git
cd influxdb-cxx; mkdir build
cd build
cmake ..
sudo make install
 ```

### macOS
```bash
brew install awegrzyn/influxdata/influxdb-cxx
```

## Quick start

### Basic write

```cpp
// Provide complete URI
auto influxdb = influxdb::InfluxDBFactory::Get("http://localhost:8086/?db=test");
influxdb->write(Point{"test"}
  .addField("value", 10)
  .addTag("host", "localhost")
);
```

### Batch write

```cpp
// Provide complete URI
auto influxdb = influxdb::InfluxDBFactory::Get("http://localhost:8086/?db=test");
// Write batches of 100 points
influxdb->batchOf(100);

for (;;) {
  influxdb->write(Point{"test"}.addField("value", 10));
}
```

### Query

```cpp
// Available over HTTP only
auto influxdb = influxdb::InfluxDBFactory::Get("http://localhost:8086/?db=test");
/// Pass an IFQL to get list of points
std::vector<Point> points = idb->query("SELECT * FROM test");
```

## Transports

An underlying transport is fully configurable by passing an URI:
```
[protocol]://[username:password@]host:port[/?db=database]
```
<br>
List of supported transport is following:

| Name        | Dependency  | URI protocol   | Sample URI                            |
| ----------- |:-----------:|:--------------:| -------------------------------------:|
| HTTP        | none        | `http`         | `http://localhost:8086/?db=<db>`      |
| UDP         | none        | `udp`          | `udp://localhost:8094`                |

