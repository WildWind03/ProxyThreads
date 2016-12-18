//
// Created by alexander on 17.12.16.
//
#pragma once
#ifndef PROXYTHREADS_CONCURRENT_HASH_MAP_H
#define PROXYTHREADS_CONCURRENT_HASH_MAP_H

#include <map>

template <typename K, typename V>
class concurrent_hash_map {
    std::map<K,V> map;
    pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

public:
    concurrent_hash_map() {
    }

    void insert(K key, V value) {
        pthread_rwlock_wrlock(&rwlock);
        map.insert(std::pair<K,V>(key, value));
        pthread_rwlock_unlock(&rwlock);
    }

    void erase(K key) {
        pthread_rwlock_wrlock(&rwlock);
        map.erase(key);
        pthread_rwlock_unlock(&rwlock);
    }

    typename std::map<K, V>::const_iterator find(K key) {
        return map.find(key);
    }

    void lock_read() {
        pthread_rwlock_rdlock(&rwlock);
    }

    void unlock() {
        pthread_rwlock_unlock(&rwlock);
    }

    void lock_write() {
        pthread_rwlock_wrlock(&rwlock);
    }


    typename std::map<K,V>::const_iterator begin() const {
        return map.begin();
    }

    typename std::map<K,V>::const_iterator end() const {
        return map.end();
    }

    virtual ~concurrent_hash_map() {
        pthread_rwlock_destroy(&rwlock);
    }
};
#endif //PROXYTHREADS_CONCURRENT_HASH_MAP_H
