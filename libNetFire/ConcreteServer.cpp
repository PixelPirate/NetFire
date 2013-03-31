//
//  ConcreteServer.cpp
//  libNetFire
//
//  Created by Patrick Horlebein on 13.02.13.
//  Copyright (c) 2013 Patrick Horlebein. All rights reserved.
//

#include "ConcreteServer.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define MAXBUFLEN 100
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

static void open_on_port(unsigned int port)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    ssize_t numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    
    std::string portString = std::to_string(port);
    
    if ((rv = getaddrinfo(NULL, portString.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        throw "LOL";
    }
    
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }
        
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }
        
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        throw "ROFL";
    }
    
    freeaddrinfo(servinfo);
    
    printf("listener: waiting to recvfrom...\n");
    
    addr_len = sizeof their_addr;
    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
                             (struct sockaddr *)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }
    
    printf("listener: got packet from %s\n",
           inet_ntop(their_addr.ss_family,
                     get_in_addr((struct sockaddr *)&their_addr),
                     s, sizeof s));
    printf("listener: packet is %zd bytes long\n", numbytes);
    buf[numbytes] = '\0';
    printf("listener: packet contains \"%s\"\n", buf);
    
    close(sockfd);
}

NetFire::ConcreteServer::ConcreteServer(unsigned int port) : _state(ConcreteServerStateStopped)
{
    _flags._is_running = 1;
    _runloop = std::thread([this, port]() {
        while (_flags._is_running == 1) {
            switch (_state) {
                case ConcreteServerStateOpening:
                    open_on_port(port);
                    break;
                case ConcreteServerStateRunning:
                    
                    break;
                case ConcreteServerStateClosing:
                    _flags._is_running = 0;
                    break;
                default:
                    break;
            }
        }
    });
}

NetFire::ConcreteServer::~ConcreteServer()
{
    
}

unsigned int NetFire::ConcreteServer::clients()
{
    return 0;
}

void NetFire::ConcreteServer::open()
{
    _state = ConcreteServerStateOpening;
}

void NetFire::ConcreteServer::close()
{
    _state = ConcreteServerStateClosing;
    _runloop.join();
}

void NetFire::ConcreteServer::on_message_recieve(std::function<void (const std::string)> action)
{
    _recieve_action = action;
}