//
// Created by alexander on 09.12.16.
//
#pragma once

#ifndef PROXY_OBSERVER_H
#define PROXY_OBSERVER_H

class observer {

public:
    virtual void update(int event_type1, void* data) = 0;
};
#endif //PROXY_OBSERVER_H
