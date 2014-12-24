//
//  Socket.h
//  Achilles
//
//  Created by zhenik on 14.04.14.
//  Copyright (c) 2014 Venus. All rights reserved.
//

#pragma once

#include <string>
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */

class TCPSocket{
public:
    typedef int SocketHandle_t;
    
    //  constructor for server listen socket..
    TCPSocket(int *error=nullptr);
    
    //  constructor for tcp client..
    //  error is 0 if socket is created..
    TCPSocket(const std::string &ip,uint32_t port,int *error=nullptr);
    
    //  constructor for server clients..
    TCPSocket(SocketHandle_t);
    TCPSocket(TCPSocket&&);
    TCPSocket(const TCPSocket&)=delete;
    ~TCPSocket();
    
    TCPSocket& operator=(const TCPSocket&)=delete;
    TCPSocket& operator=(TCPSocket&&);
    
    void close();
    int connect();
    size_t writeBytes(const void*,size_t);
    size_t readBytes(void *data,size_t bytesCount);
    int bindToPort(uint16_t);
    int listenForConnections(int maxNumberOfConnections);
    
    //  should be called only for server listen socket (constructed using default c-tor)..
    TCPSocket acceptConnection();
    
    SocketHandle_t handle() const;
    int bytesToRead() const;
    const struct sockaddr_in& address() const;
    
    //  socket set class, fd_set object representation..
    class Set{
    public:
        Set();
        
        void addSocket(const TCPSocket&);
        void removeSocket(const TCPSocket&);
        
        bool socketIsSet(const TCPSocket&) const;
    private:
        fd_set _set;
        
        friend class TCPSocket;
    };
    
    static int Select(Set *readSockets,Set *writeSockets,Set *errorSockets,struct timeval *timeout);
private:
    SocketHandle_t _handle;
    struct sockaddr_in _address;
    
    //  private c-tor for acception..
    TCPSocket(SocketHandle_t,const struct sockaddr_in&);
};
