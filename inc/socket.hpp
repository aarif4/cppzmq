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
