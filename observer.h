//
// Created by alexander on 09.12.16.
//

#ifndef PROXY_OBSERVER_H
#define PROXY_OBSERVER_H

#include "event_type.h"

class observer {

public:

    virtual void update(event_type event_type1) = 0;
};
#endif //PROXY_OBSERVER_H
