//
// Created by alexander on 13.12.16.
//

#ifndef PROXYTHREADS_REQUEST_CLIENT_H
#define PROXYTHREADS_REQUEST_CLIENT_H

#include <netinet/in.h>
#include "request_base.h"

class request_client : public request_base {
    const int MAX_SIZE_OF_REQUEST = 2 * 1024;
    char* request;

public:
    request_client(int socket_fd, sockaddr_in addr) : request_base(socket_fd, addr) {
        request = new char[MAX_SIZE_OF_REQUEST];
    }

    virtual void *exec() override {
        while (is_running) {

        }
        return nullptr;
    }

    virtual ~request_client() {
        delete request;
    }



};
#endif //PROXYTHREADS_REQUEST_CLIENT_H
