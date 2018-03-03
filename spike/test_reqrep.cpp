#include <thread>
#include <time.h>
#include <sstream>
#include <stdio.h>
#include <zmq.hpp>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>





//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
// Function that forever sends request to ZMQ REP Server
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
void runReq(std::string ipAddr, std::string filter)
{
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Prepare our context and publisher
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//{
    //Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t socket (context, ZMQ_REQ);
usleep(1000000);
    socket.connect(ipAddr.c_str());
    printf("runReq(): ZMQ REQ connects to \"%s\" with filter \"%s\"\n", 
                                                ipAddr.c_str(), filter.c_str());
    printf("runReq(): Going to send 1 message every 2 seconds\n");
    printf("runReq(): Waiting for 5 seconds before starting sending msgs...\n");
    usleep(5000000);
//}




  
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Generate and send requests to server
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//{
    int request_nbr = 0;
    while (++request_nbr > 0)
    { /* Every 2 seconds, send a request with a new ID and wait for response */
        // send request
        zmq::message_t request(6);
        memset((void *) request.data(),   // Preallocate msg with '/0'
               '\0', 
               6);
        memcpy(request.data(), "Hello", 5);
        std::cout << "Sending " << (char *) request.data() << " #" << request_nbr << "..." << std::endl;
        socket.send(request);
        std::cout << "\tMessage Sent!" << std::endl;

        // Get the reply
        zmq::message_t reply;
        socket.recv(&reply);
        std::cout << "Received " << (char *) reply.data() << " #" << request_nbr << std::endl;
        usleep(2000000);
    }
//}
}



//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
// Function that forever responds to REQ Client requests
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
void runRep(std::string ipAddr, std::string filter)
{
      //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
      // Prepare our context and subscriber
      //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//{
      zmq::context_t context(1);
      zmq::socket_t socket(context, ZMQ_REP);
      socket.bind(ipAddr.c_str());
  
      // Let the user know
      printf("runRep(): Binding to IP Address \"%s\" with filter \"%s\"\n", 
                                                ipAddr.c_str(), filter.c_str());
      printf("runRep(): Waiting for REQ Client to send message...\n");
//}


    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Receive and translate messages
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//{
    int request_nbr = 0;
    while(++request_nbr > 0) 
    {
        zmq::message_t request;
    
        std::cout << "Waiting for message...................." << std::endl;
        // Wait for next request from client
        socket.recv(&request);
        std::cout << "Received " << (char *) request.data() << " #" << request_nbr << std::endl;
    
        //Do some 'work'
        usleep(1);
    
        // Send reply back to client 
        zmq::message_t reply (5);
        memcpy(reply.data(), "World", 5);
        socket.send(reply);
    }
//}

}



//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
// Maine function
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
int main(int argc, char *argv[])
{
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Validate that executable was called with the right number of args
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    if (argc != 3)
    { /* If the user hasn't provided the right no. of args, let the user know */
        printf("ERROR: Not the right number of arguments!\n");
        printf("reqRepTest [ipAddr]\n");
        printf("\t[ipAddr] -> IP+Port to be used at publisher & subscriber\n");
        printf("\t\texample: reqRepTest 127.0.0.1:50000\n\n\n");
        fflush(stdout); // Flush the contents of stdout buffer

        return 1;
    }


    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Let user know ZMQ version running on their system
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    int major, minor, patch;
    zmq_version (&major, &minor, &patch);
    printf("Current ZMQ version is %d.%d.%d\n\n", major, minor, patch); 
  
    
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Run the Publisher's Thread and Subscriber's Thread using inputted args
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // We're currently "assuming" that our user is giving us "correct" values
    std::string filter (argv[2]);
    std::string ipAddr (argv[1]); ipAddr = "tcp://" + ipAddr;
    std::thread rep_thread(runRep, ipAddr, filter);
    std::thread req_thread(runReq, ipAddr, filter);


    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Run the Publisher's Thread and Subscriber's Thread using inputted args
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    rep_thread.join();
    req_thread.join();


    return 0;


}



//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
// Some notes to do
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
// TODO: Validate input arguments
// TODO: Handle keyboard interrupt in runPub() and runSub()

