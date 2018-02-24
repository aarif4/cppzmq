#include <zmq.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define within(num) (int) ((float) num * random () / (RAND_MAX + 1.0))

int main()
{
  // Prepare our context and publisher
  zmq::context_t context(1);
  zmq::socket_t publisher(context, ZMQ_PUB);
  publisher.bind("tcp://*:5556");
  
  while (1)
  {

    // Send message to all subscribers
    zmq::message_t message(20);
    snprintf((char*) message.data(), 20, "HELLO there friend!");
    publisher.send(message);

  }
  return 0;
}
