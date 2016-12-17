//
// Created by alexander on 13.12.16.
//

#ifndef PROXYTHREADS_REQUEST_CLIENT_H
#define PROXYTHREADS_REQUEST_CLIENT_H

#include <netinet/in.h>
#include "request_base.h"
#include "logger.h"
#include "util.h"
#include "http_request_parser.h"
#include "exception_invalid_http_data.h"
#include "my_observable.h"

class request_client : public request_base, public observable {
    const int MAX_SIZE_OF_REQUEST = 2 * 1024;
    size_t read_bytes_of_request = 0;

    logger *logger1;
    char* request;
    bool is_read = true;

public:
    request_client(int socket_fd, sockaddr_in addr) : request_base(socket_fd, addr) {
        request = new char[MAX_SIZE_OF_REQUEST];
    }

    virtual void* exec() override {
        while (is_running) {
            if (is_read) {
                ssize_t count_of_received_bytes = recv(get_socket_fd(), request, MAX_SIZE_OF_REQUEST - read_bytes_of_request, 0);

                if (-1 == count_of_received_bytes || 0 == count_of_received_bytes) {
                    std::cerr << "Invalid read operation. The connection will be closed" << std::endl;
                    break;
                }

                if (sockets_util::is_finished_request(count_of_received_bytes, read_bytes_of_request, request)) {
                    std::string handled_request = request;
                    free(request);


                    handled_request = sockets_util::change_type_of_connection_to_close(handled_request);

                    request = (char*) malloc(sizeof(char) * handled_request.length());

                    for (int k = 0; k < handled_request.length(); ++k) {
                        request[k] = handled_request.c_str()[k];
                    }

                    try {
                        http_request_parser http_parser1(request);


                        int major_v = http_parser1.get_major_version();
                        int minor_v = http_parser1.get_minor_version();

                        std::string url = http_parser1.get_uri();
                        std::string host = http_parser1.get_host();

                        if (major_v != 1 || minor_v != 0) {
                            std::cout << "[POSSIBLE INCORRECT BEHAVIOR] The version of http protocol is not supported" + std::to_string(major_v) + "." + std::to_string(minor_v) << std::endl;
                        }

                        int request_type = http_parser1.get_request_type();
                        switch (request_type) {
                            case http_request_parser::GET_REQUEST : {
                                is_read = false;
                                logger1 = new logger("client", "/home/alexander/ClionProjects/Proxy/log/" + sockets_util::get_logger_filename_by_url(url));
                                break;
                            }
                            default:
                                std::cerr << "There is not get request. The connection will be closed" << std::endl;
                                return nullptr;
                        }

                    } catch (exception_invalid_http_data & e) {
                        std::cerr << e.get_text() << std::endl;
                        return nullptr;
                    }
                }

            } else {
                std::cout << "STOP" << std::endl;
                break;
            }
        }
        return nullptr;
    }

    virtual ~request_client() {
        close(get_socket_fd());
        delete request;
    }



};
#endif //PROXYTHREADS_REQUEST_CLIENT_H
