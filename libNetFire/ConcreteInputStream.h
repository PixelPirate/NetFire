//
//  ConcreteInputStream.h
//  libNetFire
//
//  Created by Patrick Horlebein on 14.02.13.
//  Copyright (c) 2013 Patrick Horlebein. All rights reserved.
//

#ifndef __libNetFire__ConcreteInputStream__
#define __libNetFire__ConcreteInputStream__

#include "NetFire.h"
#include <vector>

namespace NetFire {
    class ConcreteInputStream : public NetFire::InputStream {
        NetFire::StreamDelegate *_delegate;
        const int _fileDescriptor;
        NetFire::Socket *_socket;
        bool _remote;
        std::string _buffer;
        unsigned long _position;
        
    public:
        ConcreteInputStream(const std::string &url);
        ConcreteInputStream(NetFire::Socket *socket);
        virtual size_t read(uint8_t *buffer, size_t max_length);
        virtual void schedule();
        virtual void cancel();
        virtual void perform();
        virtual void schedule_in_runloop(NetFire::Runloop *runloop);
        virtual void open();
        virtual void close();
        virtual void set_delegate(NetFire::StreamDelegate *delegate);
        
        void free_non_seekable_buffer();
    };
}

#endif /* defined(__libNetFire__ConcreteInputStream__) */
