#include <thread>
#include <time.h>
#include <sstream>
#include <stdio.h>
#include <zmq.hpp>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

// Make function definition to get random value that's b/w [0,num)
#define within(num) (int) ((float) num * random () / (RAND_MAX + 1.0))

// TODO: Pass in ip+port as runtime argument for runPub() and 
//         runSub()
// TODO: Handle keyboard interrupt in runPub() and runSub()





// Function that forever sends random weather parameters via
//   a socket in a string delimited with a space
void runPub(std::string ipAddr, std::string filter)
{
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
  // Prepare our context and publisher
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//{
  zmq::context_t context(1);
  zmq::socket_t  publisher(context, ZMQ_PUB);
  publisher.bind(ipAddr.c_str());
  printf("runPub(): ZMQ PUB connected to \"%s\" with filter \"%s\"\n", 
                                                ipAddr.c_str(), filter.c_str());
  printf("runPub(): Going to send 1 message every second\n");
  usleep(5000000);
//}


  
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
  // Generate and publish random weather messages
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//{
  // Initialize random number generator
  srandom((unsigned) time (NULL));
  int cnt = 0;
  while (++cnt > 0) // loop indefinitely
  { /* Forever generate random weather stuff and send it */
    int zipcode, temperature, relhumidity;

    // Get values that will fool the boss (Take that!)
    zipcode     = within(100000);
    temperature = within(215) - 80;
    relhumidity = within(50) + 10;

    // Send message to all subscribers
    zmq::message_t message(20);
    snprintf((char*) message.data(), // pointer to msg array
             20,                     // size of msg array
             "%05d %d %d",           // format string
             zipcode,                // val1
             temperature,            // val2
             relhumidity);           // val3

    // Finally send the message via your PUB socket
zmq::message_t msg(filter.size()+1);
memset((void *) msg.data(), '\0', filter.size()+1);
snprintf((char *) msg.data(), filter.size()+1, "%s", filter.c_str());
    publisher.send(msg, ZMQ_SNDMORE);
    publisher.send(message);
    printf("runPub(): Sent message #%d: \"%s\"\n", cnt, (char *)message.data());
    usleep(1000000);
  }
//}
}



// Function that forever subscribes to a particular ip+port 
//   and forever receives info
void runSub(std::string ipAddr, std::string filter)
{
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
  // Prepare our context and subscriber
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//{
//  char * ipAddr = "tcp://127.0.0.1:5556";
  zmq::context_t context(1);
  zmq::socket_t subscriber(context, ZMQ_SUB);
  subscriber.connect(ipAddr.c_str());
  //subscriber.setsockopt(ZMQ_SUBSCRIBE, filter.c_str(), filter.size());
  subscriber.setsockopt(ZMQ_SUBSCRIBE, filter.c_str(), sizeof(char));
  // Let the user know
  printf("runSub(): Conencting ZMQ SUB to \"%s\"\n", ipAddr.c_str());
  printf("runSub(): Waiting for PUB socket to send message...\n");
//}



  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
  // Receive and translate messages
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
  // Subscribe to zipcode, default is NYC, 10001
//  const char *filter = (argc > 1) ? argv[1]: "10001 ";
//  const char *filter = "10001 ";
//  const char *filter = "";
 //  subscriber.setsockopt(ZMQ_SUBSCRIBE, "HELLO", (size_t)5);

  // Process 100 updates
  int zipcode = 0; 
  int temperature = 0; 
  int relhumidity = 0;
  int update_nbr = 0;
  long total_temp = 0;
//  for (update_nbr = 1; update_nbr <= 100; update_nbr++)
  while (++update_nbr>0)
  {
    zmq::message_t update;

    subscriber.recv(&update);

    std::istringstream iss(static_cast<char *> (update.data()));
    // printf("Got a message! \"%s\"\n", iss.str().c_str());
    iss >> zipcode;
    iss >> temperature;
    iss >> relhumidity;
    iss >> zipcode >> temperature >> relhumidity;
    total_temp += temperature;

    printf("runSub(): Got message #%d: \"%s\"\n", update_nbr, (char *) update.data());
    std::cout << "\tAverage temperature for zipcode '" << zipcode << "' was " 
              << (int)(total_temp/((double)update_nbr)) << "F" << std::endl;
    usleep(500000);
  }
}



int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("ERROR: Not the right amount of arguments!\n");
    printf("pubSubTest [ipAddr] [filter]\n");
    printf("\t[ipAddr] -> IP + Port to be used at publisher and subscriber\n");
    printf("\t[filter] -> ZMQ Filter to be used during communication\n\n\n");
    return 1;
  }
  int major, minor, patch;
  zmq_version (&major, &minor, &patch);
  printf("Current 0MQ version is %d.%d.%d\n\n", major, minor, patch); 

  std::thread funcTest1(runPub, std::string("tcp://127.0.0.1:50000"), std::string("abc"));
  std::thread funcTest2(runSub, std::string("tcp://127.0.0.1:50000"), std::string("abc"));
//  std::thread funcTest1(runPub, std::string("tcp://127.0.0.1:50000"), std::string(""));
//  std::thread funcTest2(runSub, std::string("tcp://127.0.0.1:50000"), std::string(""));
  funcTest1.join();
  funcTest2.join();
  return 0;


}
