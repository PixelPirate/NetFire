//
//  NetFire.h
//  libNetFire
//
//  Created by Patrick Horlebein on 13.02.13.
//  Copyright (c) 2013 Patrick Horlebein. All rights reserved.
//

#ifndef __libNetFire__NetFire__
#define __libNetFire__NetFire__

#include <iostream>
#include <string>

namespace NetFire {
    
    class URL { //scheme://domain:port/path?query_string#fragment_id
    public:
        static NetFire::URL *new_URL_with_string(const std::string &url);
        virtual ~URL() {};
        virtual const std::string protocol() = 0;
        virtual const std::string hostname() = 0;
        virtual const std::string path() = 0;
        virtual const std::string query() = 0;
        virtual const std::string port() = 0;
    };
    
    class Server {
    public:
        static NetFire::Server *new_server_on_port(unsigned int port);
        virtual ~Server() {};
        virtual unsigned int clients() = 0;
        virtual void open() = 0;
        virtual void close() = 0;
        virtual void on_message_recieve(std::function<void (const std::string)> action) = 0;
    };
    
    class Client {
    public:
        static NetFire::Client *new_client_for_server(const std::string &url);
        virtual ~Client() {};
        virtual void connect() = 0;
        virtual void disconnect() = 0;
        virtual void send(const std::string &message) = 0;
    };
    
    class Runloop;
    class RunloopSource;
    class RunloopObserver;
    class Stream;
    class StreamDelegate;
    class InputStream;
    class OutputStream;
    
    class RunloopObserver {
    public:
        virtual void callout() = 0;
    };
    
    class RunloopSource {
    public:
        virtual void schedule() {}; // Called when added to a runloop.
        virtual void cancel() {};   // Called when removed from a runloop.
        virtual void perform() = 0; // Called when the source has fired.
    };
    
    class Socket : public RunloopSource {
    public:
        struct SocketSignature {
            int protocolFamily;
            int socketType;
            int protocol;
            const std::string URL;
        };
        typedef struct SocketSignature SocketSignature;
        enum ProtocolFamily {
            ProtocolFamilyIPv4,
            ProtocolFamilyIPv6,
            ProtocolFamilyAny,
        };
        typedef enum ProtocolFamily ProtocolFamily;
        static SocketSignature UDPSocketSignatureForAddress(const std::string &address, ProtocolFamily pf = ProtocolFamilyAny);
        static SocketSignature TCPSocketSignatureForAddress(const std::string &address, ProtocolFamily pf = ProtocolFamilyAny);
        
        static NetFire::Socket *new_socket(SocketSignature signature);
        static NetFire::Socket *new_socket_connected_to_signature(SocketSignature signature);
        static NetFire::Socket *new_socket_with_native_handle(SocketSignature signature, int sockfd);
        virtual ~Socket() {};
        
        virtual void set_data_avaliable_callback(std::function<void (NetFire::Socket *, const void *, size_t)> callback) = 0;
        
        virtual void connect_to_address(const std::string &address) = 0;
        virtual size_t send_data(const void *data, size_t length) = 0;
        
        virtual void schedule() = 0;
        virtual void cancel() = 0;
        virtual void perform() = 0;
        
    protected:
        virtual void data_available() = 0;
        virtual void accepted(int sockfd) = 0;
        virtual void data_recieved(const void *data, size_t length) = 0;
        virtual void connected(bool successfull) = 0;
        virtual void writeable() = 0;
    };
    
    class Stream : public RunloopSource {
    public:
        typedef enum {
            StreamEventNone,
            StreamEventOpenCompleted,
            StreamEventHasBytesAvailable,
            StreamEventHasSpaceAvailable,
            StreamEventErrorOccurred,
            StreamEventEndEncountered
        } StreamEvent;
    public:
        virtual void schedule_in_runloop(NetFire::Runloop *runloop) = 0;
        virtual void open() = 0;
        virtual void close() = 0;
        virtual void set_delegate(NetFire::StreamDelegate *delegate) = 0;
        static void get_streams_to_host_named(const std::string &hostName,
                                              int port,
                                              NetFire::InputStream **inputStreamPtr,
                                              NetFire::OutputStream **outputStreamPtr);
    };
    
    class StreamDelegate {
    public:
        virtual void stream_handle_event(Stream *stream, NetFire::Stream::StreamEvent eventCode) = 0;
    };
    
    class InputStream : public Stream {
    public:
        static NetFire::InputStream *new_input_stream_for_url(const std::string &url);
        static NetFire::InputStream *new_input_stream_for_socket(NetFire::Socket *socket);
        virtual size_t read(uint8_t *buffer, size_t max_length) = 0;
        
    };
    
    class OutputStream : public Stream {
    public:
        static NetFire::OutputStream *new_output_stream_for_url(const std::string &url);
        static NetFire::OutputStream *new_output_stream_for_socket(NetFire::Socket *socket);
        virtual size_t write(uint8_t *byte, size_t length) = 0;
    };
    
    class Runloop {
    public:
        typedef enum {
            RunloopReturnReasonFinishedHandlingObserver,
            RunloopReturnReasonClientsResigned,
            RunloopReturnReasonInternalError
        } RunloopReturnReason;
        typedef enum {
            RunloopStateBeforeSources,
        } RunloopState;
        
    public:
        static NetFire::Runloop *new_runloop();
        virtual ~Runloop() {};
        virtual NetFire::Runloop::RunloopReturnReason run() = 0;
        virtual void add_source(RunloopSource *source) = 0;
        virtual void remove_source(RunloopSource *source) = 0;
        static NetFire::Runloop *main_runloop();
        virtual void add_observer(RunloopObserver *observer, RunloopState state) = 0;
    };
    
    extern double absolute_time_get_current();
}

#endif /* defined(__libNetFire__NetFire__) */
