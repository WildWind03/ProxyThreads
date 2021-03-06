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
#include "request_server.h"

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
            request_client *request_client1 = new request_client(new_fd, sockaddr_in1);
            request_client1->set_observer(this);
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
        case events::REQUEST_GOT: {
            request_client *request_client1 = (request_client *) data;
            cache_entry *cache_entry1 = cache.get(request_client1->get_url());

            if (NULL == cache_entry1) {
                request_client1->log("There is no appropriate data in cache");

                addrinfo *addrinfo1 = sockets_util::hostname_to_addrinfo(request_client1->get_host());

                int server_socket_fd = socket(addrinfo1->ai_family, addrinfo1->ai_socktype, addrinfo1->ai_protocol);
                if (-1 == server_socket_fd) {
                    request_client1->log("Can not create socket to server");
                    throw exception_invalid_http_data("Can not create socket");
                }

                cache_entry1 = new cache_entry(request_client1->get_url());
                cache_entry1->add_observer(this);
                cache.insert(request_client1->get_url(), cache_entry1);
                request_server *request_server1 = new request_server(server_socket_fd,
                                                                     addrinfo1,
                                                                     request_client1->get_request(), cache_entry1,
                                                                     request_client1->get_url());
                request_server1->set_observer(this);
                request_client1->set_buffer(cache_entry1);
                cache_entry1->add_reader();
                request_server1->start();
            } else {
                request_client1->log("There is appropriate data in cache");
                request_client1->set_buffer(cache_entry1);
                cache_entry1->add_reader();
            }

            break;
        }
        case events::DELETE_ENTRY_FROM_CACHE: {
            cache.erase(*((std::string *) data));
            break;
        }
        default: {
            break;
        }
    }

}

proxy_server::~proxy_server() {

}
