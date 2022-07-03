//
//
#ifdef _WIN32
    #include <Winsock2.h> // before Windows.h, else Winsock 1 conflict
    #include <Ws2tcpip.h> // needed for ip_mreq definition for multicast
    #include <Windows.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif

#include <cstdint>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <UDPSocket.h>

#ifdef LIB_NAMESPACE
namespace LIB_NAMESPACE {
#else
namespace influxdb {
#endif
namespace transports {


static void close_socket(int socket_fd) {
    if (socket_fd >= 0) {
#ifdef _WIN32
        int result = closesocket(socket_fd);
        WSACleanup();
#else
        int result = ::close(socket_fd);
#endif
    }
}


UDPSocket::UDPSocket(const std::string& hostname, int port) {
    this->hostname = hostname;
    this->port = port;
    socket_fd = -1;

#ifdef _WIN32
    // initialize Windows Socket API with given VERSION.
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        perror("WSAStartup failure");
        return;
    }
#endif
    // create what looks like an ordinary UDP socket
    socket_fd = (int)socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (socket_fd < 0) {
        perror("cannot create socket");
        return;
    }

    // allow multiple sockets to use the same PORT number
    uint32_t yes = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)) < 0) { // (char *) cast for WIN32 compatibility
        perror("Reusing ADDR failed");
        close_socket(socket_fd);
        return;
    }

    // open connection to udp server
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d", port);
    struct addrinfo* addr = NULL;
    if (getaddrinfo(hostname.c_str(), buffer, NULL, &addr) == 0) {
        while (addr != NULL) {
            if (connect(socket_fd, addr->ai_addr, (int)addr->ai_addrlen) >= 0) {
                freeaddrinfo(addr);
                return;
            }
            addr = addr->ai_next;
        }
        freeaddrinfo(addr);
    }
    perror("connecting stream socket failure");
    close_socket(socket_fd);
}


UDPSocket::~UDPSocket(void) {
    close_socket(socket_fd);
}


// send udp packet to the speedwire multicast address
void UDPSocket::send(std::string&& message) {

    // now just sendto() our destination
    int nbytes = ::send(socket_fd, message.c_str(), (int)message.size(), 0);
    if (nbytes < 0) {
        perror("sendto failure");
    }
}


} // namespace transports
} // namespace influxdb
