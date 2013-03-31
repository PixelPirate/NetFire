//
//  ConcreteOutputStream.cpp
//  libNetFire
//
//  Created by Patrick Horlebein on 14.02.13.
//  Copyright (c) 2013 Patrick Horlebein. All rights reserved.
//

#include "ConcreteOutputStream.h"

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

NetFire::ConcreteOutputStream::ConcreteOutputStream(const std::string &url_string) : _fileDescriptor(-1), _remote(false)
{
    NetFire::URL *url = NetFire::URL::new_URL_with_string(url_string);
    delete url;
}

NetFire::ConcreteOutputStream::ConcreteOutputStream(NetFire::Socket *socket) : _socket(socket), _fileDescriptor(-1), _remote(true)
{
    
}

size_t NetFire::ConcreteOutputStream::write(uint8_t *byte, size_t length)
{
    return _socket->send_data(byte, length);
}

void NetFire::ConcreteOutputStream::schedule()
{
    
}

void NetFire::ConcreteOutputStream::cancel()
{
    
}

void NetFire::ConcreteOutputStream::perform()
{
    
}

void NetFire::ConcreteOutputStream::schedule_in_runloop(NetFire::Runloop *runloop)
{
    runloop->add_source(this);
}

void NetFire::ConcreteOutputStream::open()
{
    
}

void NetFire::ConcreteOutputStream::close()
{
    
}

void NetFire::ConcreteOutputStream::set_delegate(NetFire::StreamDelegate *delegate)
{
    _delegate = delegate;
}
