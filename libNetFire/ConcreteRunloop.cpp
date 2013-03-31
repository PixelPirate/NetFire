//
//  ConcreteRunloop.cpp
//  libNetFire
//
//  Created by Patrick Horlebein on 14.02.13.
//  Copyright (c) 2013 Patrick Horlebein. All rights reserved.
//

#include "ConcreteRunloop.h"
#include <unistd.h>

NetFire::Runloop::RunloopReturnReason NetFire::ConcreteRunloop::run()
{
    RunloopReturnReason returnReason = RunloopReturnReasonInternalError;
    
    while (true) {
        
        if (_sources.empty()) {
            returnReason = RunloopReturnReasonClientsResigned;
            break;
        }
        
        auto range = _observers.equal_range(NetFire::Runloop::RunloopStateBeforeSources);
        std::for_each(range.first, range.second, [](std::pair<RunloopState, RunloopObserver*> ob) {
            ob.second->callout();
        });
        
        for (NetFire::RunloopSource *source : _sources) {
            source->perform();
        }
        
        usleep(14); // Runloop runs ~70 times a second.
    }
    
    return returnReason;
}

void NetFire::ConcreteRunloop::add_source(NetFire::RunloopSource *source)
{
    _sources.insert(source);
    source->schedule();
}

void NetFire::ConcreteRunloop::remove_source(NetFire::RunloopSource *source)
{
    std::set<NetFire::RunloopSource *>::iterator the_source = std::find(_sources.begin(), _sources.end(), source);
    if (the_source != _sources.end()) {
        source->cancel();
        _sources.erase(the_source);
    }
}

void NetFire::ConcreteRunloop::add_observer(NetFire::RunloopObserver *observer, NetFire::Runloop::RunloopState state)
{
    _observers.insert(std::make_pair(state, observer));
}