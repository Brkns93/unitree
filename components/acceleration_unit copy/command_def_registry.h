#ifndef _CALLBACK_REGISTRY_
#define _CALLBACK_REGISTRY_


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


struct command_def_t {
    uint16_t id;
    void (*cb)(void* payload1, void* payload2, void* payload3);
    const size_t stack_size;
    const uint32_t timeout;
    const uint32_t start_delay;
};

struct command_def_registry_t {
    struct command_def_t *cbs;
    uint16_t count;
};

struct command_def_registry_t cdr_create();
int cdr_register_command_def(struct command_def_registry_t* cbr, const struct command_def_t* command);
struct command_def_t *cdr_get_command_def_by_id(struct command_def_registry_t* cbr, const uint16_t cmd_id);

#endif // _CALLBACK_REGISTRY_
