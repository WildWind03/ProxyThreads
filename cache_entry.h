//
// Created by alexander on 18.12.16.
//

#ifndef PROXYTHREADS_CACHE_ENTRY_H
#define PROXYTHREADS_CACHE_ENTRY_H

#include <cstring>
#include "my_observer.h"
#include "concurrent_hash_map.h"
#include "events.h"
#include "atomic_counter.h"

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

    const size_t MAX_DATA_SIZE = 5 * 1024 * 1024;

public:

    const static int COMMON_ERROR = -1;
    const static int DELETE_CACHE_ENTRY = -2;

    cache_entry(std::string url) {
        data = new char[MAX_DATA_SIZE];
        mutex = PTHREAD_MUTEX_INITIALIZER;
        cond_reader = PTHREAD_COND_INITIALIZER;
        cond_writer = PTHREAD_COND_INITIALIZER;
        this -> url = url;
    }

    int read(int socket_fd, char* src) {
        size_t pos = 0;
        int result;

        pthread_mutex_lock(&mutex);

        while(true) {
            while (pos >= current_length && !is_invalid && !(is_finished && current_length == pos)) {
                pthread_cond_wait(&cond_reader, &mutex);
            }

            if (is_finished && current_length == pos) {
                count_of_readers--;

                result = 0;
                if (is_streaming && 0 == count_of_readers) {
                    result = DELETE_CACHE_ENTRY;
                }
                pthread_mutex_unlock(&mutex);

                return result;

            }

            if (is_invalid) {
                result = COMMON_ERROR;
                count_of_readers--;

                if (0 == count_of_readers) {
                    result = DELETE_CACHE_ENTRY;
                }

                pthread_mutex_unlock(&mutex);

                return result;
            }

            ssize_t count_of_sent_chars = send(socket_fd, data + pos, current_length - pos, MSG_NOSIGNAL);

            if (-1 == count_of_sent_chars) {
                result = COMMON_ERROR;
                count_of_readers--;

                if (is_streaming && 0 == count_of_readers) {
                    result = DELETE_CACHE_ENTRY;
                }

                break;
            }

            pos += count_of_sent_chars;

            if (pos == MAX_DATA_SIZE && !is_finished) {
                count_of_readers_which_have_read_all_buffer++;

                if (count_of_readers_which_have_read_all_buffer == count_of_readers) {
                    pthread_cond_signal(&cond_writer);
                }
            }
        }

        pthread_mutex_unlock(&mutex);

        return result;
    }

    int write(int socket_fd) {
        int result = COMMON_ERROR;

        pthread_mutex_lock(&mutex);

        while(!is_finished) {

            while (0 == MAX_DATA_SIZE - current_length) {
                pthread_cond_wait(&cond_writer, &mutex);

                if (0 == count_of_readers && is_streaming) {
                    result = DELETE_CACHE_ENTRY;
                    pthread_mutex_unlock(&mutex);
                    return result;
                }

                if (count_of_readers_which_have_read_all_buffer >= count_of_readers) {
                    is_streaming = true;
                    observer1 -> update(events::DELETE_ENTRY_FROM_CACHE, (void*) url.c_str());
                    count_of_readers_which_have_read_all_buffer = 0;
                    current_length = 0;
                }
            }
            ssize_t count_of_received_bytes = recv(socket_fd, data + current_length,
                                                   MAX_DATA_SIZE - current_length, 0);

            current_length += count_of_received_bytes;

            if (-1 == count_of_received_bytes) {
                result = COMMON_ERROR;
                is_invalid = true;
            }

            if (0 == count_of_received_bytes) {
                is_finished = true;
                result = 0;
            }

            pthread_cond_broadcast(&cond_reader);
        }

        pthread_mutex_unlock(&mutex);

        return result;
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

    void delete_reader() {
        pthread_mutex_lock(&mutex);
        count_of_readers--;
        pthread_mutex_unlock(&mutex);
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
