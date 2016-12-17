//
// Created by alexander on 13.12.16.
//

#ifndef PROXYTHREADS_SOCKETS_UTIL_H
#define PROXYTHREADS_SOCKETS_UTIL_H

#include <bits/time.h>
#include <sys/socket.h>
#include <string>
#include <algorithm>

class sockets_util {
    sockets_util() = delete;

public:

    static int set_socket_timeout(int socket_fd, int seconds) {
        struct timeval timeout;
        timeout.tv_sec = seconds;
        timeout.tv_usec = 0;

        return setsockopt (socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                        sizeof(timeout));
    }

    static bool is_finished_request(size_t count_of_received_bytes, size_t current_pos_in_request, char* request) {
        size_t start_pos_for_checking_for_end_of_request;
        size_t length;

        if (current_pos_in_request >= 3 ) {
            start_pos_for_checking_for_end_of_request = current_pos_in_request - 3;
            length = count_of_received_bytes + 3;
        } else {
            start_pos_for_checking_for_end_of_request = 0;
            length = count_of_received_bytes;
        }

        std::string string(request + start_pos_for_checking_for_end_of_request, length);

        char end_of_request[] = {'\r', '\n', '\r', '\n'};
        size_t pos_of_end = string.find(end_of_request);

        return pos_of_end != string.length();
    }

    static std::string change_type_of_connection_to_close(std::string request) {
        size_t index = 0;

        while (true) {
            index = request.find("keep-alive", index);
            if (index >= request.length()) {
                break;
            }

            request.replace(index, 10, "Close     ");
            index += 10;
        }

        return request;
    }

    static std::string get_logger_filename_by_url(std::string url) {
        std::string new_logger_name(url);
        std::replace(new_logger_name.begin(), new_logger_name.end(), '.', '_');
        std::replace(new_logger_name.begin(), new_logger_name.end(), '/', '_');
        std::replace(new_logger_name.begin(), new_logger_name.end(), ':', '_');

        return new_logger_name;
    }
};
#endif //PROXYTHREADS_SOCKETS_UTIL_H