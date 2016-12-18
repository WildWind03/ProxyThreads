//
// Created by alexander on 13.12.16.
//
#pragma once
#ifndef PROXYTHREADS_REQUEST_SERVER_H
#define PROXYTHREADS_REQUEST_SERVER_H

#include <netdb.h>
#include "request_base.h"
#include "cache_entry.h"
#include "logger.h"
#include "util.h"
#include "events.h"

class request_server : public request_base, public observer {
    std::string request;
    cache_entry *cache_entry1;
    logger *logger1;
    addrinfo *addrinfo1;
    observer *observer1;
    bool is_write = true;
    size_t pos_in_sending_data = 0;
    std::string url;

public:
    request_server(int socket_fd, addrinfo *addrinfo1, std::string request, cache_entry *cache_entry1, std::string url) : request_base(socket_fd) {
        this->request = request;
        this -> cache_entry1 = cache_entry1;
        this -> logger1 = new logger("server", "/home/alexander/ClionProjects/Proxy/log/" + sockets_util::get_logger_filename_by_url(url));
        this -> url = url;
    }

    void set_observer(observer *observer2) {
        this -> observer1 = observer2;
    }

    virtual void exec() override {
        int connect_result = connect(get_socket_fd(), addrinfo1 -> ai_addr, addrinfo1 -> ai_addrlen);

        if (-1 == connect_result) {
            log("Can not connect");
            observer1 -> update(events::DELETE_REQUEST, (void*) get_socket_fd());
            observer1 -> update(events::DELETE_ENTRY_FROM_CACHE, (void*) url.c_str());
            cache_entry1 -> mark_invalid();
            return;
        }

        while (is_running) {
            if (is_write) {
                ssize_t count_of_send_data = send(get_socket_fd(), request.c_str() + pos_in_sending_data,
                                                  request.length() - pos_in_sending_data, 0);

                if (-1 == count_of_send_data) {
                    log ("Error while sending data to server");
                    observer1 -> update(events::DELETE_REQUEST, (void*) get_socket_fd());
                    observer1 -> update(events::DELETE_ENTRY_FROM_CACHE, (void*) url.c_str());
                    cache_entry1 -> mark_invalid();
                    return;
                }

                pos_in_sending_data += count_of_send_data;

                if (pos_in_sending_data == request.length()) {
                    is_write = false;
                    log("The request was successfully sent to server");
                }

            } else {
                int result = cache_entry1 -> write(get_socket_fd());

                if (-1 == result) {
                    observer1 -> update(events::DELETE_REQUEST, (void*) get_socket_fd());
                    observer1 -> update(events::DELETE_ENTRY_FROM_CACHE, (void*) url.c_str());
                    cache_entry1 -> mark_invalid();
                }

                return;
            }
        }

        return;
    }

    void log(std::string string) {
        logger1 -> log(string);
    }

    virtual ~request_server() {
        delete logger1;
    }

};
#endif //PROXYTHREADS_REQUEST_SERVER_H
