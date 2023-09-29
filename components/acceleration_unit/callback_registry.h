#ifndef _CALLBACK_REGISTRY_
#define _CALLBACK_REGISTRY_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct callback_t
{
  uint16_t id;
  void (*cb)(void *payload1, void *payload2, void *payload3);
};

struct callback_registry_t
{
  struct callback_t *cbs;
  uint16_t count;
};

struct callback_registry_t cb_create();
int cb_register_callback(struct callback_registry_t *elr, const struct callback_t *callback);
struct callback_t *cb_get_callback_by_id(struct callback_registry_t* cbr, const uint16_t id);

#endif // _CALLBACK_REGISTRY_
