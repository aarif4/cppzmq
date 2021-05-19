/*
SocketObject class:
- zmq::context  context (singleton format)
- zmq::socket_t socket
- std::string   endpoint
- abstract functions
  - setsockopt()
  - send()
  - receive()
  - poll()
- threading
  - void operator()(...)
*/

#ifndef _SOCKET_
#define _SOCKET_
#include <iostream>
#include <thread>
#include <string>

class Socket {
  public:
    Socket();
    virtual void operator()(std::string str);
};

#endif // _SOCKET_
