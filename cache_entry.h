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

    atomic_counter atomic_counter1 = 0;
    atomic_counter count_of_exit_readers = 0;

    concurrent_hash_map<int, observer*> readers;

    observer *observer1;

    bool is_finished = false;
    bool is_invalid = false;
    bool is_streaming = false;

    const size_t MAX_DATA_SIZE = 5 * 1024 * 1024;

public:
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

            if (pos == current_length && is_finished) {
                result = 0;
                break;
            }

            if (pos == MAX_DATA_SIZE && !is_finished) {
                atomic_counter1.increment();

                readers.lock_read();

                if (atomic_counter1.get() == readers.size()) {
                    if (!is_streaming) {
                        is_streaming = true;
                        observer1 -> update(events::DELETE_ENTRY_FROM_CACHE, (void*) url.c_str());
                    }
                    pthread_cond_signal(&cond_writer);
                }

                readers.unlock();
            }

            while (pos == current_length) {
                pthread_cond_wait(&cond_reader, &mutex);

                if (is_invalid) {
                    pthread_mutex_unlock(&mutex);
                    count_of_exit_readers.increment();

                    if (count_of_exit_readers.get() >= readers.size()) {
                        return -2;
                    }

                    return -1;
                }
            }

            ssize_t count_of_sent_chars = send(socket_fd, data + pos, current_length - pos, MSG_NOSIGNAL);

            if (-1 == count_of_sent_chars) {
                result = -1;
                break;
            }

            pos += count_of_sent_chars;
        }

        pthread_mutex_unlock(&mutex);

        return result;
    }

    int write(int socket_fd) {
        int result = -1;

        pthread_mutex_lock(&mutex);

        while(!is_finished) {
            while (0 == MAX_DATA_SIZE - current_length) {
                pthread_cond_wait(&cond_writer, &mutex);

                if (atomic_counter1.get() >= readers.size()) {
                    atomic_counter1.set_value(0);
                    current_length = 0;
                }
            }

            ssize_t count_of_received_bytes = recv(socket_fd, data + current_length,
                                                   MAX_DATA_SIZE - current_length, 0);

            current_length += count_of_received_bytes;

            if (-1 == count_of_received_bytes) {
                result = -1;
                break;
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

    void add_reader(int fd, observer *observer1) {
        readers.insert(fd, observer1);
    }

    void add_observer(observer *observer2) {
        this -> observer1 = observer2;
    }

    void delete_reader(int fd) {
        readers.erase(fd);
    }

    void mark_invalid() {
        this -> is_invalid = true;

        pthread_mutex_lock(&mutex);

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
