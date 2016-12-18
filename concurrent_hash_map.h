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

    V erase(K key) {
        pthread_rwlock_wrlock(&rwlock);
        V value = map.find(key).operator*().second;
        map.erase(key);
        pthread_rwlock_unlock(&rwlock);

        return value;
    }

    V get(K key) {
        pthread_rwlock_rdlock(&rwlock);

        V value;

        auto iter = map.find(key);

        if (iter == map.end()) {
            value = nullptr;
        } else {
            value = iter.second;
        }

        pthread_rwlock_unlock(&rwlock);

        return value;
    }

    virtual ~concurrent_hash_map() {
        pthread_rwlock_destroy(&rwlock);
    }
};
#endif //PROXYTHREADS_CONCURRENT_HASH_MAP_H
