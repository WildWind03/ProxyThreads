//
// Created by alexander on 13.12.16.
//
#pragma once
#ifndef PROXYTHREADS_REQUEST_BASE_H
#define PROXYTHREADS_REQUEST_BASE_H

#include <pthread.h>
#include <netinet/in.h>
#include "exception_can_not_create_request.h"

class request_base {
    int socket_fd;
    pthread_t thread;

public:
    bool is_running = true;

    request_base(int socket_fd) {
        this -> socket_fd = socket_fd;
    }

    static void* run(void* arg) {
        request_base *request_base1 = (request_base*) arg;
        request_base1->exec();
        delete request_base1;
        return nullptr;
    }

    virtual void start() {
        pthread_attr_t pthread_attr;
        pthread_attr_init(&pthread_attr);
        pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED);

        int result_of_thread_creating = pthread_create(&thread, &pthread_attr, run, this);

        pthread_attr_destroy(&pthread_attr);

        if (-1 == result_of_thread_creating) {
            throw exception_can_not_create_request("Can not create new thread");
        }
    }

    virtual void exec() = 0;

    virtual int get_socket_fd() final {
        return socket_fd;
    }

    virtual void stop() {
        is_running = false;
    }

    virtual ~request_base() {

    }
};
#endif //PROXYTHREADS_REQUEST_BASE_H
