//
//  ConcreteOutputStream.h
//  libNetFire
//
//  Created by Patrick Horlebein on 14.02.13.
//  Copyright (c) 2013 Patrick Horlebein. All rights reserved.
//

#ifndef __libNetFire__ConcreteOutputStream__
#define __libNetFire__ConcreteOutputStream__

#include "NetFire.h"
#include <vector>

namespace NetFire {
    class ConcreteOutputStream : public NetFire::OutputStream {
        NetFire::StreamDelegate *_delegate;
        const int _fileDescriptor;
        NetFire::Socket *_socket;
        bool _remote;
        
    public:
        ConcreteOutputStream(const std::string &url);
        ConcreteOutputStream(NetFire::Socket *socket);
        virtual size_t write(uint8_t *byte, size_t length);
        virtual void schedule();
        virtual void cancel();
        virtual void perform();
        virtual void schedule_in_runloop(NetFire::Runloop *runloop);
        virtual void open();
        virtual void close();
        virtual void set_delegate(NetFire::StreamDelegate *delegate);
    };
}

#endif /* defined(__libNetFire__ConcreteOutputStream__) */
