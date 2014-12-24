//
//  Socket.cpp
//  Achilles
//
//  Created by zhenik on 14.04.14.
//  Copyright (c) 2014 Venus. All rights reserved.
//

#include "TCPSocket.h"
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <sys/ioctl.h>
#include <unistd.h>
#ifndef __APPLE__
#include <cstring>
#endif

TCPSocket::TCPSocket(int *error){
    _handle=::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(_handle<0){
		::perror("socket returned error: ");
        if(error){
            *error=1;
        }
	}
}

TCPSocket::TCPSocket(const std::string &ip,uint32_t port,int *error){
    
    /* Create a reliable, stream socket using TCP */
    _handle = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(_handle<0){
        perror("socket() failed");
        if(error){
            *error=1;
        }
    }else{
        
        /* Construct the server address structure */
        ::memset(&_address, 0, sizeof(_address));     /* Zero out structure */
        _address.sin_family      = AF_INET;             /* Internet address family */
        _address.sin_addr.s_addr = ::inet_addr(ip.c_str());   /* Server IP address */
        _address.sin_port        = htons(port); /* Server port */
    }
}

TCPSocket::TCPSocket(SocketHandle_t handle_):
_handle(handle_)
{
    //..
}

TCPSocket::TCPSocket(TCPSocket &&other):
_handle(other._handle),
_address(other._address)
{
    other._handle=-1;
}

TCPSocket::~TCPSocket(){
    this->close();
}

TCPSocket& TCPSocket::operator=(TCPSocket &&other){
    this->close();
    _handle=other._handle;
    other._handle=-1;
    _address=other._address;
    return *this;
}

#pragma mark - Private

TCPSocket::TCPSocket(SocketHandle_t handle_,const struct sockaddr_in &address_):
_handle(handle_),
_address(address_)
{
    //..
}

#pragma mark - Getters

const struct sockaddr_in& TCPSocket::address() const{
    return _address;
}

int TCPSocket::bytesToRead() const{
    int res;
    ::ioctl(_handle,FIONREAD,&res);
    return res;
}

TCPSocket::SocketHandle_t TCPSocket::handle() const{
    return _handle;
}

#pragma mark - Public

void TCPSocket::close(){
    if(_handle!=-1){
        ::close(_handle);
    }
}

TCPSocket TCPSocket::acceptConnection(){
    struct sockaddr_in client_address;
    int client_len=sizeof(client_address);
    int client_sockfd=::accept(_handle,(struct sockaddr*)&client_address,(socklen_t*)&client_len);
    return TCPSocket(client_sockfd,client_address);
}

int TCPSocket::listenForConnections(int maxNumberOfConnections){
    return ::listen(_handle,maxNumberOfConnections);
}

int TCPSocket::bindToPort(uint16_t thePort){
	_address.sin_family=AF_INET;
	_address.sin_addr.s_addr=htonl(INADDR_ANY);
	_address.sin_port=htons(thePort);
	int server_len=sizeof(_address);
	return ::bind(_handle,(struct sockaddr*)&_address,server_len);
}

int TCPSocket::connect(){
    int res;
    if (::connect(_handle, (struct sockaddr *) &_address, sizeof(_address)) < 0){
        perror("connect() failed");
        res=1;
    }else{
        res=0;
    }
    return res;
}

size_t TCPSocket::readBytes(void *data, size_t bytesCount){
    return ::read(_handle,data,bytesCount);
}

size_t TCPSocket::writeBytes(const void *bytesPointer,size_t bytesCount){
    return ::send(_handle,bytesPointer,bytesCount,0);
}

#pragma mark - Static

int TCPSocket::Select(Set *readSockets,Set *writeSockets,Set *errorSockets,struct timeval *timeout){
    fd_set *readFdsPointer=readSockets?&readSockets->_set:nullptr;
    fd_set *writeFdsPointer=writeSockets?&writeSockets->_set:nullptr;
    fd_set *errorFdsPointer=errorSockets?&errorSockets->_set:nullptr;
    return ::select(FD_SETSIZE,readFdsPointer,writeFdsPointer,errorFdsPointer,timeout);
}

#pragma mark - TCPSocket::Set

TCPSocket::Set::Set(){
    FD_ZERO(&_set);
}

#pragma mark - Getters

bool TCPSocket::Set::socketIsSet(const TCPSocket &sock) const{
    return FD_ISSET(sock.handle(),&_set);
}

#pragma mark - Public

void TCPSocket::Set::removeSocket(const TCPSocket &sock){
    FD_CLR(sock.handle(),&_set);
}

void TCPSocket::Set::addSocket(const TCPSocket &sock){
    FD_SET(sock.handle(),&_set);
}
