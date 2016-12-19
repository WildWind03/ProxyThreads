//
// Created by alexander on 18.12.16.
//

#ifndef PROXYTHREADS_CACHE_ENTRY_H
#define PROXYTHREADS_CACHE_ENTRY_H

#include <cstring>
#include <sstream>
#include "my_observer.h"
#include "concurrent_hash_map.h"
#include "events.h"

class cache_entry {
    std::string url;
    char* data;
    size_t current_length = 0;
    pthread_mutex_t mutex;
    pthread_cond_t cond_reader;
    pthread_cond_t cond_writer;

    int count_of_readers_which_have_read_all_buffer = 0;
    int count_of_readers = 0;

    observer *observer1;

    bool is_finished = false;
    bool is_invalid = false;
    bool is_streaming = false;
    bool is_checked_response = false;

    const size_t MAX_DATA_SIZE = 110 * 1024 * 1024;
    const int OK_RESPONSE = 200;

public:

    const static int SUCCESS = 0;
    const static int COMMON_ERROR = -1;
    const static int DELETE_CACHE_ENTRY = -2;

    cache_entry(std::string url) {
        data = new char[MAX_DATA_SIZE];
        mutex = PTHREAD_MUTEX_INITIALIZER;
        cond_reader = PTHREAD_COND_INITIALIZER;
        cond_writer = PTHREAD_COND_INITIALIZER;
        this -> url = url;
    }

    int read_to_browser(int socket_fd, char *src) {
        ssize_t pos = 0;

        while(true) {
            pthread_mutex_lock(&mutex);

            while ((pos >= current_length && !is_finished && !is_invalid) || -1 == pos) {
                pthread_cond_wait(&cond_reader, &mutex);

                if (-1 == pos && count_of_readers_which_have_read_all_buffer != count_of_readers) {
                    pos = 0;
                }
            }

            if (is_finished && current_length == pos) {
                count_of_readers--;

                int result = SUCCESS;

                if (is_streaming && 0 == count_of_readers) {
                    result = DELETE_CACHE_ENTRY;
                }

                pthread_mutex_unlock(&mutex);
                return result;

            }

            if (is_invalid) {
                int result = COMMON_ERROR;
                count_of_readers--;

                if (0 == count_of_readers) {
                    result = DELETE_CACHE_ENTRY;
                }

                pthread_mutex_unlock(&mutex);
                return result;
            }


            pthread_mutex_unlock(&mutex);

            ssize_t count_of_sent_chars = send(socket_fd, data + pos, current_length - pos, MSG_NOSIGNAL);

            pthread_mutex_lock(&mutex);
            if (-1 == count_of_sent_chars) {
                count_of_readers--;

                if (is_finished) {
                    if (is_streaming) {
                        if (0 == count_of_readers) {
                            pthread_mutex_unlock(&mutex);
                            return DELETE_CACHE_ENTRY;
                        } else {
                            pthread_mutex_unlock(&mutex);
                            return COMMON_ERROR;
                        }
                    } else {
                        pthread_mutex_unlock(&mutex);
                        return COMMON_ERROR;
                    }
                } else {
                    pthread_cond_signal(&cond_writer);
                    pthread_mutex_unlock(&mutex);
                    return COMMON_ERROR;
                }
            }

            pos += count_of_sent_chars;

            if (pos == MAX_DATA_SIZE && !is_finished) {
                count_of_readers_which_have_read_all_buffer++;

                if (count_of_readers_which_have_read_all_buffer >= count_of_readers) {
                    pthread_cond_signal(&cond_writer);
                }

                pos = -1;
            }

            pthread_mutex_unlock(&mutex);
        }
    }

    int read_from_server(int socket_fd) {
        while(true) {
            pthread_mutex_lock(&mutex);

            while (0 == MAX_DATA_SIZE - current_length &&
                   count_of_readers_which_have_read_all_buffer < count_of_readers) {
                pthread_cond_wait(&cond_writer, &mutex);
            }

            if (0 == MAX_DATA_SIZE - current_length && count_of_readers_which_have_read_all_buffer >= count_of_readers) {
                if (!is_streaming) {
                    observer1 -> update(events::DELETE_ENTRY_FROM_CACHE, &url);
                    is_streaming = true;
                }
                if (0 == count_of_readers) {
                    pthread_mutex_unlock(&mutex);
                    return DELETE_CACHE_ENTRY;
                }

                count_of_readers_which_have_read_all_buffer = 0;
                current_length = 0;
            }

            pthread_mutex_unlock(&mutex);

            ssize_t count_of_received_bytes = recv(socket_fd, data + current_length,
                                                   MAX_DATA_SIZE - current_length, 0);

            pthread_mutex_lock(&mutex);


            current_length += count_of_received_bytes;

            if (-1 == count_of_received_bytes) {
                if (0 == count_of_readers) {
                    observer1->update(events::DELETE_ENTRY_FROM_CACHE, &url);
                    pthread_mutex_unlock(&mutex);
                    return DELETE_CACHE_ENTRY;
                } else {
                    is_invalid = true;
                    pthread_cond_broadcast(&cond_reader);
                    pthread_mutex_unlock(&mutex);
                    return COMMON_ERROR;
                }
            }

            if (0 == count_of_received_bytes) {
                is_finished = true;
                pthread_cond_broadcast(&cond_reader);
                pthread_mutex_unlock(&mutex);
                return SUCCESS;
            }

            if (!is_checked_response) {
                for (size_t k = 0; k < count_of_received_bytes; ++k) {
                    if (data[current_length - count_of_received_bytes + k] == '\n') {
                        std::stringstream stringstream1(data);

                        std::string buffer;

                        stringstream1 >> buffer;
                        stringstream1 >> buffer;

                        int response_code = std::stoi(buffer);

                        if (OK_RESPONSE != response_code) {
                            observer1 -> update(events::DELETE_ENTRY_FROM_CACHE, &url);
                        }

                        is_checked_response = true;
                        break;
                    }
                }
            }

            pthread_cond_broadcast(&cond_reader);
            pthread_mutex_unlock(&mutex);
        }
    }

    void add_reader() {
        pthread_mutex_lock(&mutex);
        count_of_readers++;
        pthread_mutex_unlock(&mutex);
    }

    void add_observer(observer *observer2) {
        pthread_mutex_lock(&mutex);
        this -> observer1 = observer2;
        pthread_mutex_unlock(&mutex);
    }

    int get_count_of_readers() {
        return count_of_readers;
    }

    void mark_invalid() {
        pthread_mutex_lock(&mutex);

        this -> is_invalid = true;
        pthread_cond_broadcast(&cond_reader);

        pthread_mutex_unlock(&mutex);
    }


    virtual ~cache_entry() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond_reader);
        pthread_cond_destroy(&cond_writer);
        delete data;
    }

};
#endif //PROXYTHREADS_CACHE_ENTRY_H
