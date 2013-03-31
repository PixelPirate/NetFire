//
//  ConcreteRunloop.h
//  libNetFire
//
//  Created by Patrick Horlebein on 14.02.13.
//  Copyright (c) 2013 Patrick Horlebein. All rights reserved.
//

#ifndef __libNetFire__ConcreteRunloop__
#define __libNetFire__ConcreteRunloop__

#include "NetFire.h"
#include <set>
#include <map>

namespace NetFire {
    class ConcreteRunloop : public NetFire::Runloop {
        std::set<NetFire::RunloopSource *> _sources;
        std::multimap<NetFire::Runloop::RunloopState, NetFire::RunloopObserver *> _observers;
        
    public:
        virtual NetFire::Runloop::RunloopReturnReason run();
        virtual void add_source(NetFire::RunloopSource *source);
        virtual void remove_source(NetFire::RunloopSource *source);
        void add_observer(RunloopObserver *observer, RunloopState state);
    };
}

#endif /* defined(__libNetFire__ConcreteRunloop__) */
