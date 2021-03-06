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






//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
// Function that forever sends random weather parameters via
//   a socket in a string delimited with a space
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
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
    printf("runPub(): Going to send 1 message every 2 seconds\n");
    printf("runPub(): Waiting for 5 seconds before starting sending msgs...\n");
    usleep(5000000);
//}


  
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
  // Generate and publish random weather messages
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//{
    // Initialize random number generator
    srandom((unsigned) time (NULL));
    int update_nbr = 0;
    long total_temp = 0; 
    while (++update_nbr > 0) // loop indefinitely
    { /* Forever generate random weather stuff and send it */
        int zipcode, temperature, relhumidity;
    
        // 1. Get values that will fool the boss (Take that!)
        zipcode     = within(100000);
        temperature = within(215) - 80;
        relhumidity = within(50) + 10;
        total_temp += temperature;
    
        // 2. Create and serialize the message you want to send
        zmq::message_t message(20);
        snprintf((char*) message.data(), // pointer to msg array
                 20,                     // size of msg array
                 "%05d %d %d",           // format string
                 zipcode,                // first val to pack
                 temperature,            // second val to pack
                 relhumidity);           // third val to pack
    
        // 3. Send the message via PUB socket
        if (filter.size() != 0)
        { /* If we have a filter, send that first before sending the message */
            size_t filt_len = filter.size()+1; // get filter message's size
            zmq::message_t filt_msg(filt_len); // create zmq msg with proper sz
            memset((void *) filt_msg.data(),   // Preallocate msg with '/0'
                   '\0', 
                   filt_len);
            snprintf((char *) filt_msg.data(), // Copy filter string to msg
                     filt_len, 
                     "%s", 
                     filter.c_str());

            // Send the filter via socket
            publisher.send(filt_msg, ZMQ_SNDMORE);
        }
        // Send the message via socket
        publisher.send(message);

        // 4. Inform the user message's contents and sleep for 2 seconds
        printf("runPub(): Sent message #%d: \"%s\"\n", update_nbr, 
                                                        (char *)message.data());
            printf("\tExpected Average temperature for zipcode: '%d' was %dF\n", 
                               zipcode, (int)(total_temp/((double)update_nbr))); 
        usleep(2000000);
    }
//}
}



//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
// Function that forever subscribes to a particular ip+port 
//   and forever receives info
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
void runSub(std::string ipAddr, std::string filter)
{
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
  // Prepare our context and subscriber
  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//{
    zmq::context_t context(1);
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.connect(ipAddr.c_str());
    subscriber.setsockopt(ZMQ_SUBSCRIBE, filter.c_str(), sizeof(char));

    // Let the user know
    printf("runSub(): Conencting ZMQ SUB to \"%s\" with filter \"%s\"\n", 
                                                ipAddr.c_str(), filter.c_str());
    printf("runSub(): Waiting for PUB socket to send message...\n");
//}



    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Receive and translate messages
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    int zipcode     = 0; 
    int temperature = 0; 
    int relhumidity = 0;
    int update_nbr  = 0;
    long total_temp = 0;
    while (++update_nbr > 0)
    {
        // 1. Block until you receive a message
        zmq::message_t update;
        subscriber.recv(&update);

        // 2. Check to see if you got the right message by verifying filter
        std::string rcvd_filter((char *)update.data());
        if (strcmp(rcvd_filter.c_str(), filter.c_str()) == 0)
        {
            // 3. Block until you receive desired message
            subscriber.recv(&update);

            // 4. Parse the message that was received 
            std::istringstream iss(static_cast<char *> (update.data()));
            iss >> zipcode;
            iss >> temperature;
            iss >> relhumidity;
//            iss >> zipcode >> temperature >> relhumidity;
            total_temp += temperature;

            // 5. Let user konw
            printf("runSub(): Got message #%d: \"%s\"\n", update_nbr, 
                                                        (char *) update.data());
            printf("\tAverage temperature for zipcode: '%d' was %dF\n\n", 
                               zipcode, (int)(total_temp/((double)update_nbr))); 
        }

        // 6. Sleep for 0.5 sec
        usleep(500000);
    }
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
        printf("pubSubTest [ipAddr] [filter]\n");
        printf("\t[ipAddr] -> IP+Port to be used at publisher & subscriber\n");
        printf("\t[filter] -> ZMQ Filter to be used during communication\n\n");
        printf("\t\texample: pubSubTest 127.0.0.1:50000 abcdef\n\n\n");
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
    std::thread pub_thread(runPub, ipAddr, filter);
    std::thread sub_thread(runSub, ipAddr, filter);


    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    // Run the Publisher's Thread and Subscriber's Thread using inputted args
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
    pub_thread.join();
    sub_thread.join();


    return 0;


}



//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
// Some notes to do
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
// TODO: Validate input arguments
// TODO: Handle keyboard interrupt in runPub() and runSub()

