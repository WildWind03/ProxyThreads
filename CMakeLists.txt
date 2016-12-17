cmake_minimum_required(VERSION 3.6)
project(ProxyThreads)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES main.cpp proxy_server.cpp proxy_server.h exception_base.h exception_can_not_start_server.h request_base.h request_client.h request_server.h logger.h exception_can_not_create_request.h exception_can_not_accept.h util.h)
add_executable(ProxyThreads ${SOURCE_FILES})