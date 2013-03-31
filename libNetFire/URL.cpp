//
//  URL.cpp
//  libNetFire
//
//  Created by Patrick Horlebein on 23.02.13.
//  Copyright (c) 2013 Patrick Horlebein. All rights reserved.
//

#include "NetFire.h"

class ConcreteURL : public NetFire::URL {
    std::string _protocol;
    std::string _hostname;
    std::string _path;
    std::string _query;
    std::string _port;
    
public:
    ConcreteURL(const std::string &url)
    {
        const std::string prot_end("://");
        std::string::const_iterator prot_i = search(url.begin(), url.end(),
                                                    prot_end.begin(), prot_end.end());
        if (prot_i != url.end()) {
        _protocol.reserve(distance(url.begin(), prot_i));
        transform(url.begin(), prot_i,
                  back_inserter(_protocol),
                  std::ptr_fun<int,int>(tolower)); // protocol is icase
            advance(prot_i, prot_end.length());
        } else {
            prot_i = url.begin();
        }
        //if (prot_i == url.end()) return;
        
        
        std::string::const_iterator port_i = find(prot_i, url.end(), ':');
        _hostname.reserve(distance(prot_i, port_i));
        transform(prot_i, port_i,
                  back_inserter(_hostname),
                  std::ptr_fun<int,int>(tolower)); // host is icase
        
        advance(port_i, 1);
        std::string::const_iterator path_i = find(port_i, url.end(), '/');
        _port.reserve(distance(port_i, path_i));
        transform(port_i, path_i,
                  back_inserter(_port),
                  std::ptr_fun<int,int>(tolower)); // port is icase
        
        advance(port_i, _port.length());
        std::string::const_iterator query_i = find(port_i, url.end(), '?');
        _path.assign(port_i, query_i);
        
        if (query_i != url.end()) ++query_i;
        
        _query.assign(query_i, url.end());
        
        
        /*
         const std::string prot_end("://");
         std::string::const_iterator prot_i = search(url.begin(), url.end(),
         prot_end.begin(), prot_end.end());
         _protocol.reserve(distance(url.begin(), prot_i));
         transform(url.begin(), prot_i,
         back_inserter(_protocol),
         std::ptr_fun<int,int>(tolower)); // protocol is icase
         
         if (prot_i == url.end()) return;
         
         advance(prot_i, prot_end.length());
         std::string::const_iterator path_i = find(prot_i, url.end(), '/');
         _hostname.reserve(distance(prot_i, path_i));
         transform(prot_i, path_i,
         back_inserter(_hostname),
         std::ptr_fun<int,int>(tolower)); // host is icase
         std::string::const_iterator query_i = find(path_i, url.end(), '?');
         _path.assign(path_i, query_i);
         
         if (query_i != url.end()) ++query_i;
         
         _query.assign(query_i, url.end());
         */
    }
    
    const std::string protocol()
    {
        return _protocol;
    }
    
    const std::string hostname()
    {
        return _hostname;
    }
    
    const std::string path()
    {
        return _path;
    }
    
    const std::string query()
    {
        return _query;
    }
    
    const std::string port()
    {
        return _port;
    }
};

NetFire::URL *NetFire::URL::new_URL_with_string(const std::string &url_string)
{
    NetFire::URL *url = new ConcreteURL(url_string);
    return url;
}
