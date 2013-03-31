//
//  NetFire.cpp
//  libNetFire
//
//  Created by Patrick Horlebein on 13.02.13.
//  Copyright (c) 2013 Patrick Horlebein. All rights reserved.
//

#include "NetFire.h"
#include "ConcreteServer.h"
#include "ConcreteClient.h"
#include "ConcreteInputStream.h"
#include "ConcreteOutputStream.h"
#include "ConcreteRunloop.h"
#include <cassert>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#include <sys/time.h>


NetFire::Server *NetFire::Server::new_server_on_port(unsigned int port)
{
    NetFire::ConcreteServer *concrete = new NetFire::ConcreteServer(port);
    return concrete;
}

NetFire::Client *NetFire::Client::new_client_for_server(const std::string &url)
{
    NetFire::ConcreteClient *concrete = new NetFire::ConcreteClient(url);
    return concrete;
}

NetFire::Runloop *NetFire::Runloop::new_runloop()
{
    NetFire::Runloop *runloop = new NetFire::ConcreteRunloop;
    return runloop;
}

NetFire::Runloop *NetFire::Runloop::main_runloop() {
    
    static NetFire::Runloop *_main_runloop = nullptr;
    if (_main_runloop == nullptr) {
        _main_runloop = new ConcreteRunloop;
    }
    return _main_runloop;
}

void NetFire::Stream::get_streams_to_host_named(const std::string &hostName,
                                                int port,
                                                NetFire::InputStream **inputStreamPtr,
                                                NetFire::OutputStream **outputStreamPtr)
{
    NetFire::InputStream *readStream = nullptr;
    NetFire::OutputStream *writeStream = nullptr;
    
    assert(!hostName.empty() && "Hostname must not be empty");
    assert((port > 0) && (port < 65536) && "Port needs to be in range 0 - 65536");
    assert(((inputStreamPtr != nullptr) || (outputStreamPtr != nullptr)) && "Only one stream parameter is allowed to be nullptr");
    
    NetFire::Socket::SocketSignature signature = NetFire::Socket::TCPSocketSignatureForAddress(hostName + ":" + std::to_string(port));
    NetFire::Socket *socket = NetFire::Socket::new_socket_connected_to_signature(signature);
    NetFire::Runloop *runloop = NetFire::Runloop::main_runloop();
    
    if (inputStreamPtr) readStream = NetFire::InputStream::new_input_stream_for_socket(socket);
    if (outputStreamPtr) writeStream = NetFire::OutputStream::new_output_stream_for_socket(socket);
    
    runloop->add_source(socket);
    
    if (inputStreamPtr != nullptr) {
        *inputStreamPtr = readStream;
    }
    if (outputStreamPtr != nullptr) {
        *outputStreamPtr = writeStream;
    }
}

NetFire::InputStream *NetFire::InputStream::new_input_stream_for_url(const std::string &url)
{
    NetFire::InputStream *input_stream = new NetFire::ConcreteInputStream(url);
    return input_stream;
}

NetFire::InputStream *NetFire::InputStream::new_input_stream_for_socket(NetFire::Socket *socket)
{
    NetFire::InputStream *input_stream = new NetFire::ConcreteInputStream(socket);
    return input_stream;
}

NetFire::OutputStream *NetFire::OutputStream::new_output_stream_for_url(const std::string &url)
{
    NetFire::OutputStream *output_stream = new NetFire::ConcreteOutputStream(url);
    return output_stream;
}

NetFire::OutputStream *NetFire::OutputStream::new_output_stream_for_socket(NetFire::Socket *socket)
{
    NetFire::OutputStream *output_stream = new NetFire::ConcreteOutputStream(socket);
    return output_stream;
}

double NetFire::absolute_time_get_current()
{
    double ret;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ret = (double)tv.tv_sec - 978307200.0L;
    ret += (1.0E-6 * (double)tv.tv_usec);
    
    return ret;
}