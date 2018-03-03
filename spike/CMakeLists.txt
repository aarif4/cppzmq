project(TEST_ZMQ_SOCKETS)
set(CMAKE_CXX_STANDARD 11)
cmake_minimum_required(VERSION 3.5)

add_executable(reqTest test_req.cpp)
target_link_libraries(reqTest zmq)
add_executable(repTest test_rep.cpp)
target_link_libraries(repTest zmq)
#add_executable(pubTest test_pub.cpp)
#target_link_libraries(pubTest zmq)
#add_executable(pubTest2 test_pub2.cpp)
#target_link_libraries(pubTest2 zmq)
#add_executable(subTest test_sub.cpp)
#target_link_libraries(subTest zmq)

add_executable(pubSubTest test_pubsub.cpp)
target_link_libraries(pubSubTest zmq pthread)

add_executable(reqRepTest test_reqrep.cpp)
target_link_libraries(reqRepTest zmq pthread)

