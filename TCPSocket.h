#pragma once

#include <string>
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */

class TCPSocket {
public:
    using SocketHandle_t = int;
    
    /**
     *  Constructor for server listen socket.
     *  @param error is 0 if socket is created..
     */
    TCPSocket(int* error = nullptr);
    
    /**
     *  Constructor for tcp client.
     *  @param ip target host ip address (for example "192.168.1.12").
     *  @param port target host port.
     *  @param error is 0 if socket is created.
     */
    TCPSocket(const std::string& ip, uint32_t port, int* error = nullptr);
    
    /**
     *  Constructor for server clients.
     */
    TCPSocket(SocketHandle_t);
    TCPSocket(TCPSocket&&);
    TCPSocket(const TCPSocket&)=delete;
    ~TCPSocket();
    
    TCPSocket& operator=(const TCPSocket&)=delete;
    TCPSocket& operator=(TCPSocket&&);
    
    void close();
    int connect();
    size_t writeBytes(const void*,size_t);
    size_t readBytes(void* data, size_t bytesCount);
    int bindToPort(uint16_t);
    int listenForConnections(int maxNumberOfConnections);
    
    /**
     *  Should be called only for server listen socket (which is constructed using default c-tor).
     */
    TCPSocket acceptConnection();
    
    SocketHandle_t handle() const;
    int bytesToRead() const;
    const struct sockaddr_in& address() const;
    
    /**
     *  Socket set class, fd_set object representation.
     */
    class Set {
    public:
        Set();
        
        /**
         *  The same thing as addSocket.
         */
        void operator+=(const TCPSocket&);
        void addSocket(const TCPSocket&);
        
        /**
         *  The same thing as removeSocket.
         */
        void operator-=(const TCPSocket&);
        void removeSocket(const TCPSocket&);
        
        /**
         *  The same thing as socketIsSet.
         */
        bool operator[](const TCPSocket&) const;
        bool socketIsSet(const TCPSocket&) const;
    private:
        fd_set _set;
        
        friend class TCPSocket;
    };
    
    static int Select(Set* readSockets, Set* writeSockets, Set* errorSockets, struct timeval* timeout);
private:
    SocketHandle_t _handle;
    struct sockaddr_in _address;
    
    //  private c-tor for acception..
    TCPSocket(SocketHandle_t, const struct sockaddr_in&);
};
