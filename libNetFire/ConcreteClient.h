//
//  ConcreteClient.h
//  libNetFire
//
//  Created by Patrick Horlebein on 13.02.13.
//  Copyright (c) 2013 Patrick Horlebein. All rights reserved.
//

#ifndef __libNetFire__ConcreteClient__
#define __libNetFire__ConcreteClient__

#include "NetFire.h"

namespace NetFire {
    class ConcreteClient : public NetFire::Client {
    public:
        ConcreteClient(const std::string &url);
        virtual ~ConcreteClient();
        virtual void connect();
        virtual void disconnect();
        virtual void send(const std::string &message);
    };
}

#endif /* defined(__libNetFire__ConcreteClient__) */
