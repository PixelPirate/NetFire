//
//  ConcreteServer.h
//  libNetFire
//
//  Created by Patrick Horlebein on 13.02.13.
//  Copyright (c) 2013 Patrick Horlebein. All rights reserved.
//

#ifndef __libNetFire__ConcreteServer__
#define __libNetFire__ConcreteServer__

#include "NetFire.h"
#include <thread>

namespace NetFire {
    class ConcreteServer : public NetFire::Server {
        
        typedef enum {
            ConcreteServerStateStopped,
            ConcreteServerStateOpening,
            ConcreteServerStateRunning,
            ConcreteServerStateClosing
        } ConcreteServerState;
        
        ConcreteServerState _state;
        std::function<void (const std::string)> _recieve_action;
        std::thread _runloop;
        struct {
            unsigned int _is_running:1;
        } _flags;
        
    public:
        ConcreteServer(unsigned int port);
        virtual ~ConcreteServer();
        virtual unsigned int clients();
        virtual void open();
        virtual void close();
        virtual void on_message_recieve(std::function<void (const std::string)> action);
    };
}

#endif /* defined(__libNetFire__ConcreteServer__) */
