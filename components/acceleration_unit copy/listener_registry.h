#ifndef _EVENT_LISTENER_REGISTRY_
#define _EVENT_LISTENER_REGISTRY_


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


struct listener_t {
    uint16_t event_id;
    int (*cb)(const void* event);
};

struct listener_registry_t {
    struct listener_t *listeners;
    uint16_t listener_count;
};

struct listener_registry_t lr_create();
int lr_register_listener(struct listener_registry_t* elr, const struct listener_t* listener);


#endif // _EVENT_LISTENER_REGISTRY_
