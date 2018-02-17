#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>

int main()
{

  int major, minor, patch;
  zmq_version (&major, &minor, &patch);
  std::cout << "Current 0MQ version is " << major << "." << minor << "." << patch << std::endl;

  zmq::context_t context(1);
  zmq::socket_t socket(context, ZMQ_REP);
  socket.bind("tcp://*:5555");

  while(true) 
  {
    zmq::message_t request;

    // Wait for next request from client
    socket.recv(&request);
    std::cout << "Received Hello" << std::endl;

    //Do some 'work'
    sleep(1);

    // Send reply back to client 
    zmq::message_t reply (5);
    memcpy(reply.data(), "World", 5);
    socket.send(reply);
  }
  return 0;
}
