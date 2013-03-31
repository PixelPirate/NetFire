//
//  ConcreteInputStream.cpp
//  libNetFire
//
//  Created by Patrick Horlebein on 14.02.13.
//  Copyright (c) 2013 Patrick Horlebein. All rights reserved.
//

#include "ConcreteInputStream.h"

NetFire::ConcreteInputStream::ConcreteInputStream(const std::string &url_string) : _socket(nullptr), _fileDescriptor(-1), _remote(false), _position(0)
{
    NetFire::URL *url = NetFire::URL::new_URL_with_string(url_string);
    if (url->hostname().compare("localhost") == 0 || url->hostname().compare("127.0.0.1") == 0) {
        _remote = false;
    } else {
        _remote = true;
    }
    
    delete url;
}

NetFire::ConcreteInputStream::ConcreteInputStream(NetFire::Socket *socket) : _socket(socket), _fileDescriptor(-1), _remote(true), _position(0)
{
    socket->set_data_avaliable_callback([this](NetFire::Socket *socket, const void *bytes, size_t length){
        if (socket == this->_socket) {
            this->_buffer.append(std::string((const char *)bytes, length));
            this->_delegate->stream_handle_event(this, NetFire::Stream::StreamEventHasBytesAvailable);
        }
    });
}

size_t NetFire::ConcreteInputStream::read(uint8_t *buffer, size_t max_length)
{
    size_t length = std::min(max_length, _buffer.length());
    std::string buf = _buffer.substr(_position, length);
    _position += length;
    //_buffer.erase(0, length);
    std::copy(buf.begin(), buf.end(), buffer);
    free_non_seekable_buffer();
    return length;
}

void NetFire::ConcreteInputStream::schedule()
{
    
}

void NetFire::ConcreteInputStream::cancel()
{
    
}

void NetFire::ConcreteInputStream::perform()
{
    //_delegate->stream_handle_event(this, NetFire::Stream::StreamEventHasBytesAvailable);
}

void NetFire::ConcreteInputStream::schedule_in_runloop(NetFire::Runloop *runloop)
{
    runloop->add_source(this);
}

void NetFire::ConcreteInputStream::open() {
    
}

void NetFire::ConcreteInputStream::close()
{
    
}

void NetFire::ConcreteInputStream::set_delegate(NetFire::StreamDelegate *delegate)
{
    _delegate = delegate;
}

void NetFire::ConcreteInputStream::free_non_seekable_buffer()
{
    _buffer.clear();
    _position = 0;
}
