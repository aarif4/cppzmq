#include <zmq.hpp>
#include <iostream>
#include <sstream>

int main(int argc, char *argv[])
{
  int major, minor, patch;
  zmq_version (&major, &minor, &patch);
  std::cout << "Current 0MQ version is " << major << "." << minor << "." << patch << std::endl;


  zmq::context_t context(1);

  // Socket to talk to server
  std::cout << "Collecting updates from weather server...\n" << std::endl;
  zmq::socket_t subscriber(context, ZMQ_SUB);
  subscriber.connect("tcp://localhost:5556");

  // Subscribe to zipcode, default is NYC, 10001
  const char *filter = (argc > 1) ? argv[1]: "10001 ";
  subscriber.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));

  // Process 100 updates
  int update_nbr;
  long total_temp = 0;
  for (update_nbr = 0; update_nbr < 100; update_nbr++)
  {
    zmq::message_t update;
    int zipcode, temperature, relhumidity;

    subscriber.recv(&update);

    std::istringstream iss(static_cast<char *> (update.data()));
    printf("Got a message! \"%s\"\n", iss.str().c_str());
//    iss >> zipcode;
//    iss >> temperature;
//    iss >> relhumidity;
    iss >> zipcode >> temperature >> relhumidity;
    total_temp += temperature;

    std::cout << "Average temperature for zipcode '" << filter << "' was " << (int)(total_temp/((double)update_nbr)) << "F" << std::endl;
  }
  return 0;
}
