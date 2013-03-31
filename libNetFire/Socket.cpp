//
//  Socket.c
//  libNetFire
//
//  Created by Patrick Horlebein on 23.02.13.
//  Copyright (c) 2013 Patrick Horlebein. All rights reserved.
//

#include "NetFire.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <thread>

static void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
static void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

NetFire::Socket::SocketSignature NetFire::Socket::UDPSocketSignatureForAddress(const std::string &address, ProtocolFamily pf)
{
    int fam = PF_UNSPEC;
    switch (pf) {
        case ProtocolFamilyAny:
            fam = PF_UNSPEC;
            break;
        case ProtocolFamilyIPv4:
            fam = PF_INET;
            break;
        case ProtocolFamilyIPv6:
            fam = PF_INET6;
            break;
        default:
            break;
    }
    
    NetFire::Socket::SocketSignature signature = {fam, SOCK_DGRAM, IPPROTO_UDP, address};
    
    return signature;
}

NetFire::Socket::SocketSignature NetFire::Socket::TCPSocketSignatureForAddress(const std::string &address, ProtocolFamily pf)
{
    int fam = PF_UNSPEC;
    switch (pf) {
        case ProtocolFamilyAny:
            fam = PF_UNSPEC;
            break;
        case ProtocolFamilyIPv4:
            fam = PF_INET;
            break;
        case ProtocolFamilyIPv6:
            fam = PF_INET6;
            break;
        default:
            break;
    }
    
    NetFire::Socket::SocketSignature signature = {fam, SOCK_STREAM, IPPROTO_TCP, address};
    
    return signature;
}

class ConcreteSocket : public NetFire::Socket {
    NetFire::Socket::SocketSignature _signature;
    int _sockfd;
    std::function<void (NetFire::Socket *, const void *, size_t)> _data_avaliable_callback;
    
public:
    ConcreteSocket(NetFire::Socket::SocketSignature signature);
    ConcreteSocket(NetFire::Socket::SocketSignature signature, int sockfd);
    void connect_to_address(const std::string &address);
    size_t send_data(const void *data, size_t length);
    
    void set_data_avaliable_callback(std::function<void (NetFire::Socket *, const void *, size_t)> callback);
    
    void schedule();
    void cancel();
    void perform();
    
protected:
    void data_available();
    void accepted(int sockfd);
    void data_recieved(const void *data, size_t length);
    void connected(bool successfull);
    void writeable();
    
    void init_connected_to_socket(NetFire::Socket::SocketSignature signature);
};

class ListenerSocket : public NetFire::Socket {
    NetFire::Socket::SocketSignature _signature;
    int _sockfd;
    std::function<void (NetFire::Socket *, const void *, size_t)> _data_avaliable_callback;
    
public:
    ListenerSocket(NetFire::Socket::SocketSignature signature);
    
    void set_data_avaliable_callback(std::function<void (NetFire::Socket *, const void *, size_t)> callback);
    
    void schedule();
    void cancel();
    void perform();
    
    void accepted(int sockfd);
    
    void connect_to_address(const std::string &address) {};
    size_t send_data(const void *data, size_t length) { return 0; };
    void data_recieved(const void *data, size_t length) {};
    void connected(bool successfull) {};
    void writeable() {};
    void data_available() {};
    
    void init_listen_socket(NetFire::Socket::SocketSignature signature);
};

NetFire::Socket *NetFire::Socket::new_socket(SocketSignature signature)
{
    NetFire::Socket *socket = new ListenerSocket(signature);
    return socket;
}

NetFire::Socket *NetFire::Socket::new_socket_connected_to_signature(SocketSignature signature)
{
    NetFire::Socket *socket = new ConcreteSocket(signature);
    return socket;
}

NetFire::Socket *NetFire::Socket::new_socket_with_native_handle(SocketSignature signature, int sockfd)
{
    NetFire::Socket *socket = new ConcreteSocket(signature, sockfd);
    return socket;
}

ConcreteSocket::ConcreteSocket(NetFire::Socket::SocketSignature signature) : _sockfd(-1), _data_avaliable_callback(nullptr)
{
    init_connected_to_socket(signature);
}

ConcreteSocket::ConcreteSocket(NetFire::Socket::SocketSignature signature, int sockfd) : _signature(signature), _sockfd(sockfd), _data_avaliable_callback(nullptr)
{
    
}

void ConcreteSocket::set_data_avaliable_callback(std::function<void (NetFire::Socket *, const void *, size_t)> callback)
{
    _data_avaliable_callback = callback;
}

void ConcreteSocket::connect_to_address(const std::string &address)
{
    
}

size_t ConcreteSocket::send_data(const void *data, size_t length)
{
    size_t bytes_send = 0;
    if ((bytes_send = send(_sockfd, data, length, 0)) != length) {
        char m[255];
        sprintf(m, "%zd from %zd bytes send", bytes_send, length);
        perror(m);
    }
    
    return bytes_send;
}

void ConcreteSocket::schedule()
{
    
}

void ConcreteSocket::cancel()
{
    ::close(_sockfd);
}

void ConcreteSocket::perform()
{
    if (_sockfd == -1) {
        return;
        // invalidate;
    }
    
    const int MAXDATASIZE = 100;
    char buf[MAXDATASIZE];
    size_t numbytes = 0;
    if ((numbytes = recv(_sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        //perror("Socket recv");
        return;
    }
    if (numbytes == 0) return; // Invalidate ?
    
    buf[numbytes] = '\0';
    
    data_recieved(buf, numbytes);
}

void ConcreteSocket::data_available()
{
    
}

void ConcreteSocket::accepted(int sockfd) // Called in a different thread.
{
    
}

void ConcreteSocket::data_recieved(const void *data, size_t length)
{
    if (_data_avaliable_callback != nullptr) {
        _data_avaliable_callback(this, data, length);
    }
}

void ConcreteSocket::connected(bool successfull)
{
    
}

void ConcreteSocket::writeable()
{
    std::cout << __TIMESTAMP__ << ": " <<  __PRETTY_FUNCTION__ << std::endl;
}

void ConcreteSocket::init_connected_to_socket(NetFire::Socket::SocketSignature signature)
{
//    const size_t maximal_buffer_size = 100;
    
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
//    ssize_t numbytes;
//    struct sockaddr_storage their_addr;
//    char buf[maximal_buffer_size];
//    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];
//    char yes = 0;
    
    NetFire::URL *url = NetFire::URL::new_URL_with_string(signature.URL);
    int service = std::atoi(url->port().c_str());
    const std::string host = url->hostname();
    delete url;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = signature.protocolFamily; //AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = signature.socketType; //SOCK_DGRAM; //SOCK_STREAM; SOCK_SEQPACKET;
    hints.ai_flags = AI_PASSIVE; // use my IP
    hints.ai_protocol = signature.protocol;
    
    std::string port_string = std::to_string(service);
    
    if ((rv = getaddrinfo(host.c_str(), port_string.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
    }
    
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "client: failed to bind socket\n");
        return;
    }
    
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);
    
    _sockfd = sockfd;
    
    freeaddrinfo(servinfo);
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    
    // Clients can now send_data(2) to the socket.
    // To read anything from the socket, it must be scheduled in a runloop, there it will send notifications about new data.
    
    
    
    //printf("listener: waiting to recvfrom...\n");
    //fcntl(sockfd, F_SETFL, O_NONBLOCK);
    /*
    addr_len = sizeof their_addr;
    if ((numbytes = recvfrom(sockfd, buf, maximal_buffer_size-1 , 0,
                             (struct sockaddr *)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }
    
    printf("listener: got packet from %s\n",
           inet_ntop(their_addr.ss_family,
                     get_in_addr((struct sockaddr *)&their_addr),
                     s, sizeof s));
    printf("listener: packet is %zd bytes long\n", numbytes);
    buf[numbytes] = '\0';
    printf("listener: packet contains \"%s\"\n", buf);
    
    ::close(sockfd);
     */
}

ListenerSocket::ListenerSocket(NetFire::Socket::SocketSignature signature) : _sockfd(-1), _signature(signature)
{
    
}

void ListenerSocket::set_data_avaliable_callback(std::function<void (NetFire::Socket *, const void *, size_t)> callback)
{
    _data_avaliable_callback = callback;
}

void ListenerSocket::schedule()
{
    init_listen_socket(_signature);
}

void ListenerSocket::cancel()
{
    ::close(_sockfd);
}

void ListenerSocket::perform()
{
    struct sockaddr_storage their_addr;
    socklen_t sin_size = sizeof their_addr;
    int new_fd = accept(_sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
        //perror("accept");
        return;
    }
    
    char s[INET6_ADDRSTRLEN];
    inet_ntop(their_addr.ss_family,
              get_in_addr((struct sockaddr *)&their_addr),
              s, sizeof s);
    printf("Server: Got connection from %s\n", s);
    
    std::thread([this, new_fd]() mutable throw(std::exception) [[carries_dependency]] -> void {
        this->accepted(new_fd);
    }).join();
}

void ListenerSocket::accepted(int sockfd)
{
    NetFire::Socket *socket = NetFire::Socket::new_socket_with_native_handle(_signature, sockfd);
    socket->set_data_avaliable_callback(_data_avaliable_callback);
    
    std::cout << "Made a new socket for new connection" << std::endl;
    
    NetFire::Runloop *runloop = NetFire::Runloop::main_runloop();
    runloop->add_source(socket);
}

void ListenerSocket::init_listen_socket(NetFire::Socket::SocketSignature signature)
{
    int sockfd;//, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    //struct sockaddr_storage their_addr; // connector's address information
    //socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    //char s[INET6_ADDRSTRLEN];
    int rv;
    
    NetFire::URL *url = NetFire::URL::new_URL_with_string(signature.URL);
    int service = std::atoi(url->port().c_str());
    delete url;
    
    const short backlog = 10;     // how many pending connections queue will hold
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = signature.protocolFamily; //AF_UNSPEC;
    hints.ai_socktype = signature.socketType; //SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    hints.ai_protocol = signature.protocol;
    
    std::string port_string = std::to_string(service);
    if ((rv = getaddrinfo(NULL, port_string.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        throw std::runtime_error("Could not get address info");
    }
    
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) { // Maybe as a server we should bind all of them; support ipv6 and ipv4.
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            throw std::runtime_error("Coud not set socket to resuse address");
        }
        
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            ::close(sockfd);
            perror("server: bind");
            continue;
        }
        
        break;
    }
    
    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        throw std::runtime_error("Could not bind to socket");
    }
    
    freeaddrinfo(servinfo); // all done with this structure
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    
    if (listen(sockfd, backlog) == -1) {
        perror("listen");
        throw std::runtime_error("Could not open listen socket");
    }
    
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        throw std::runtime_error("Error with sigaction");
    }
    
    printf("server: waiting for connections...\n");
    
    _sockfd = sockfd;
}
