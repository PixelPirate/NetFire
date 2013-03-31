//
//  main.cpp
//  NetFireServer
//
//  Created by Patrick Horlebein on 13.02.13.
//  Copyright (c) 2013 Patrick Horlebein. All rights reserved.
//

#include <iostream>
#include "NetFire.h"
#include <thread>

int main(int argc, const char * argv[])
{
    NetFire::Runloop *runloop = NetFire::Runloop::main_runloop();
    NetFire::Socket::SocketSignature signature = NetFire::Socket::TCPSocketSignatureForAddress("127.0.0.1:54242");
    NetFire::Socket *socket = NetFire::Socket::new_socket(signature);
    socket->set_data_avaliable_callback([](NetFire::Socket *socket, const void *data, size_t length) {
        std::cout << "Server: Recieved " << length << " bytes <" << (const char *)data << ">" << std::endl;
        socket->send_data("WUBWUB", 6);
    });
    runloop->add_source(socket);
    runloop->run();
    delete socket;
    
    return 0;
}
