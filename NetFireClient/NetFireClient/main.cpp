//
//  main.cpp
//  NetFireClient
//
//  Created by Patrick Horlebein on 13.02.13.
//  Copyright (c) 2013 Patrick Horlebein. All rights reserved.
//

#include <iostream>
#include "NetFire.h"
#include <thread>

class StreamDelegate : public NetFire::StreamDelegate {
public:
    void stream_handle_event(NetFire::Stream *stream, NetFire::Stream::StreamEvent eventCode)
    {
        if (eventCode == NetFire::Stream::StreamEventHasBytesAvailable) {
            NetFire::InputStream *input_stream = (NetFire::InputStream *)stream;
            char buffer[256];
            size_t bytes_read = input_stream->read((uint8_t *)buffer, 255);
            buffer[bytes_read] = '\0';
            std::string str(buffer, bytes_read);
            std::cout << "Client: Received " << bytes_read << " bytes <" << str << ">" << std::endl;
        }
    }
};

class RunloopObserver : public NetFire::RunloopObserver {
    double _fireDate;
    
public:
    RunloopObserver() : _fireDate(NetFire::absolute_time_get_current() + 5.0)
    {
        
    }
    
    void callout()
    {
        if (NetFire::absolute_time_get_current() >= _fireDate) {
            _fireDate = NetFire::absolute_time_get_current() + 5.0;
            
            output_stream->write((uint8_t *)"WAMBO", 5);
        }
    }
    
    NetFire::OutputStream *output_stream;
};

int main(int argc, const char * argv[])
{
    NetFire::InputStream *input_stream = nullptr;
    NetFire::OutputStream *output_stream = nullptr;
    NetFire::Stream::get_streams_to_host_named("127.0.0.1", 54242, &input_stream, &output_stream);
    StreamDelegate delegate;
    input_stream->set_delegate(&delegate);
    
    RunloopObserver observer;
    observer.output_stream = output_stream;
    NetFire::Runloop::main_runloop()->add_observer(&observer, NetFire::Runloop::RunloopStateBeforeSources);
    NetFire::Runloop::main_runloop()->run();
    
    return 0;
}

