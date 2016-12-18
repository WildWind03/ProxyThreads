//
// Created by alexander on 13.12.16.
//
#pragma once

#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <csignal>

#include "proxy_server.h"
#include "exception_can_not_start_server.h"
#include "request_client.h"

proxy_server::proxy_server(int port) {

    if (port <= 0) {
        throw exception_can_not_start_server ("Typed port (" + std::to_string(port) + ") is less than zero. The port must be positive");
    }

    socket_fd = socket (AF_INET, SOCK_STREAM, 0);

    if (-1 == socket_fd) {
        throw exception_can_not_start_server("Can not create socket");
    }

    int flag = 1;
    int is_socket_became_reusing = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
    if (-1 == is_socket_became_reusing) {
        throw exception_can_not_start_server("Can't make soccket reusing");
    }

    struct sockaddr_in sockaddr_in1;
    sockaddr_in1.sin_family = AF_INET;
    sockaddr_in1.sin_port = htons(port);
    sockaddr_in1.sin_addr.s_addr = inet_addr(BIND_IP);

    int result_of_binding = bind(socket_fd, (struct sockaddr *) &sockaddr_in1, sizeof(sockaddr_in1));

    if (-1 == result_of_binding) {
        throw exception_can_not_start_server("Can not bind socket");
    }

    int result_of_listening_socket = listen(socket_fd, MAX_COUNT_OF_PENDING_REQUESTS);
    if (0 != result_of_listening_socket) {
        throw exception_can_not_start_server("Can't make socket listen");
    }
}

void proxy_server::start() {
    while (is_running) {
        struct sockaddr_in sockaddr_in1;
        socklen_t socklen = sizeof(sockaddr_in1);

        int new_fd = accept(socket_fd, (struct sockaddr *) &sockaddr_in1, &socklen);
        if (-1 == new_fd) {
            std::cerr << "Error while accepting" << std::endl;
            continue;
        }

        try {
            std::cout << "New CLient" << std::endl;
            request_client *request_client1 = new request_client(new_fd, sockaddr_in1);
            request_client1->set_observer(this);
            requests.insert(new_fd, request_client1);
            request_client1->start();
        } catch (const exception_can_not_create_request & can_not_create_request) {
            std::cout << can_not_create_request.get_text() << std::endl;
            close(new_fd);
        }

    }
}

void proxy_server::stop() {
    is_running = false;
}

void proxy_server::update(int event_type1, void *data) {
    switch(event_type1) {
        case events::REQUEST_GOT:
            request_client* request_client1 = (request_client*) data;
            cache.lock_write();
            cache_entry *cache_entry1;

            auto iter = cache.find(request_client1->get_url());
            if (cache.end() == iter) {
                request_client1->log("There is no appropriate data in cache");
                cache_entry1 = new cache_entry(request_client1->get_url());
                cache_entry1->add_observer(this);
                cache.insert(request_client1->get_url(), cache_entry1);

            } else {
                request_client1->log("There is appropriate data in cache");
                cache_entry1 = iter.operator*().second;
            }

            request_client1 -> set_buffer(cache_entry1);
            cache_entry1 -> add_reader(request_client1->get_socket_fd(), request_client1);

            cache.unlock();

            break;
        case events::DELETE_REQUEST:
            requests.erase((int) data);
            break;
        case events::DELETE_ENTRY_FROM_CACHE:
            cache.erase((char*) data);
            break;
        case events::STREAM_ENTRY:
            break;
        case events::SEND_TO_BROWSER_ERROR:
            break;
        default:
            break;
    }

}

proxy_server::~proxy_server() {

}
