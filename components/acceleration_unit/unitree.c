#include <stdio.h>
#include <zephyr/kernel.h>
#include <unitree.h>

// #define STATE_GUARD(allowed_state) if(ut->state != allowed_state) return 3;
#define STATE_GUARD(...) \
    for (enum unit_state *state = (enum unit_state[]){__VA_ARGS__}, i = 0; i < sizeof((enum unit_state[]){__VA_ARGS__}) / sizeof(enum unit_state); i++) { \
        if(ut->state != *state) return 3; \
    }

#define STACK_SIZE 1024
#define EVENT_ID_ALL 0

int ut_run_command(struct unitree_unit_t *ut, const struct command_t *cmd);
int ut_init(struct unitree_unit_t *ut);
int ut_stop(struct unitree_unit_t *ut, bool force);
int ut_register_event_listener(struct unitree_unit_t *provider, const struct listener_t *listener);
int ut_set_state(struct unitree_unit_t *ut, const enum unit_state new_state);

// Private functions
int _ut_handle_state_not_ready(const struct unitree_unit_t *ut);
int _ut_handle_state_initializing(const struct unitree_unit_t *ut);
int _ut_handle_state_ready(const struct unitree_unit_t *ut);
int _ut_handle_state_pausing(const struct unitree_unit_t *ut);
int _ut_handle_state_paused(const struct unitree_unit_t *ut);
int _ut_handle_state_stopping(const struct unitree_unit_t *ut);
int _ut_handle_state_stopped(const struct unitree_unit_t *ut);
int _ut_handle_state_error(const struct unitree_unit_t *ut);
void _ut_event_dispatcher_thread(void *uta, void *a2, void *a3);

int _ur_register_unit(struct unitree_unit_registry_t *unit_reg, const struct unitree_unit_t *unitree_unit)
{
  if (unit_reg->count == 0)
  {
    unit_reg->units = malloc(sizeof(struct unitree_unit_t *));
  }
  else
  {
    unit_reg->units = realloc(unit_reg->units, sizeof(struct unitree_unit_t *) * (unit_reg->count + 1));
  }
  if (unit_reg->units == NULL)
  {
    return -1;
  }
  unit_reg->units[unit_reg->count] = unitree_unit;
  unit_reg->count++;

  return 0;
}

int ut_register_subunit(struct unitree_unit_t *ut, struct unitree_unit_t *sub_unit)
{
  STATE_GUARD(NOT_READY);
  printk("Registering unit %d to unit %d\n", sub_unit->def.id, ut->def.id);
  int ret = 0;
  struct listener_t listener = {
      .event_id = 0, // All events TODO: Implement event id mask
      .event_q = ut->event_q};
  ret = lr_register_listener(&(sub_unit->evt_lst_reg), &listener);
  if (ret)
  {
    printk("Registering unit %d to unit %d failed!\n", sub_unit->def.id, ut->def.id);
    return ret;
  }
  ret = _ur_register_unit(&(ut->subunit_reg), sub_unit);

  printk("Registered unit %d to unit %d\n", sub_unit->def.id, ut->def.id);
  return ret;
}

// Dispatcher thread dispatches messages from the message queue
void _ut_event_dispatcher_thread(void *uta, void *a2, void *a3)
{
  struct unitree_unit_t *ut = ((struct unitree_unit_t *)uta);
  printk("Starting event dispatcher for unit %d\n", ut->def.id);
  printk("(ut->event_q) %d\n", (ut->event_q));

  // while (1)
  // {
  //   // printk("Starting event dispatcher for unit %d\n", ut->def.id);
  //   // k_msgq_alloc_init
  //   k_sleep(K_MSEC(100));
  // }
  while (1)
  {
    struct custom_event_t *rcv_event = malloc(sizeof(struct custom_event_t));
    printk("(ut->event_q) evt%d\n", (ut->event_q));
    k_msgq_get((ut->event_q), rcv_event, K_FOREVER); // Wait for incoming messages
    printk("Got message\n");
    printk("Event %d received from unit %d for unit %d\n", rcv_event->id, rcv_event->origin, ut->def.id);

    _ut_handle_event(ut, &(rcv_event->id), &(rcv_event->payload));

    // struct callback_t *cb = cb_get_callback_by_id(&(ut->evt_handler_reg), rcv_event.origin);
    // if (cb != NULL && cb->cb != NULL)
    // {
    //   printk("Running callback for event id %d for unit %d\n", rcv_event.id, ut->def.id);
    //   cb->cb(&rcv_event, NULL, NULL);
    // }
    // else
    // {
    //   ut_set_state(ut, ERROR);
    // }

    // free(rcv_event.payload.m); // Frees payload allocated in send_event
  }
}

int _ut_command_runner(void (*cb)(void* payload1, void* payload2, void* payload3), int a){
  
}

// Dispatcher thread dispatches messages from the message queue
// Copies the command
int ut_run_command(struct unitree_unit_t *ut, const struct command_t *cmd)
{
  STATE_GUARD(READY);
  printk("Unit %d is running command %d.\n", ut->def.id, cmd->id);

  const struct command_def_t *cd = cdr_get_command_def_by_id(&(ut->cmd_reg), cmd->id);
  if (cd == NULL || cd->cb == NULL)
  {
    return -1;
  }
  printk("Unit %d found command %d.\n", ut->def.id, cmd->id);

  struct payload_t *payload = malloc(sizeof(struct payload_t));
  payload->m = malloc(cmd->payload.size);
  memcpy(payload, &cmd->payload, sizeof(struct payload_t));
  memcpy(payload->m, cmd->payload.m, cmd->payload.size);

  K_THREAD_STACK_DECLARE(my_thread_stack, 1024);
  printk("TEST %d\n", Z_THREAD_STACK_SIZE_ADJUST(cd->stack_size) - K_THREAD_STACK_RESERVED);
  k_thread_stack_t *stack_ptr = malloc(K_THREAD_STACK_SIZEOF(my_thread_stack));

  struct k_thread *command_runner_thread = k_malloc(sizeof(struct k_thread));
  k_tid_t my_tid = k_thread_create(command_runner_thread, stack_ptr,
                                   K_THREAD_STACK_SIZEOF(my_thread_stack),
                                   cd->cb, ut, payload, NULL,
                                   5, 0, (cd->start_delay == 0) ? K_NO_WAIT : K_MSEC(cd->start_delay));

  if (k_thread_join(command_runner_thread, (cd->timeout == 0) ? K_FOREVER : K_MSEC(cd->timeout)))
  {
    ut_set_state(ut, ERROR);
  }

  // free(payload->m);
  // free(payload->m);
  // free(stack_ptr);

  printk("Unit %d finished running command %d\n", ut->def.id, cmd->id);

  return 0;
}

int ut_init(struct unitree_unit_t *ut)
{
  STATE_GUARD(NOT_READY, STOPPED);
  printk("Unit %d is initializing.\n", ut->def.id);

  ut_set_state(ut, INITIALIZING);
  printk("ut->event_q %d\n", ut->event_q);

  for (int i = 0; i < ut->subunit_reg.count; ++i)
  {
    ut_init(ut->subunit_reg.units[i]);
  }

  if (ut->on_init != NULL)
  {
    ut->on_init();
  }

  printk("Initializing event queue for unit %d.\n", ut->def.id);
  k_msgq_init((ut->event_q), ut->evt_q_buffer, sizeof(struct custom_event_t), 10);
  printk("Initialized event queue for unit %d.\n", ut->def.id);

  struct k_thread *evt_dsp_thread = k_malloc(sizeof(struct k_thread));

  K_THREAD_STACK_DECLARE(my_thread_stack, 1024);
  k_thread_stack_t *stack_ptr = malloc(K_THREAD_STACK_SIZEOF(my_thread_stack));

  k_tid_t my_tid = k_thread_create(evt_dsp_thread, stack_ptr,
                                   K_KERNEL_STACK_SIZEOF(my_thread_stack),
                                   _ut_event_dispatcher_thread,
                                   ut, NULL, NULL,
                                   5, 0, K_NO_WAIT);

  ut->evt_dsp_th = evt_dsp_thread;

  ut_set_state(ut, READY);
  printk("Unit %d is initialized.\n", ut->def.id);
  return 0;
}

int ut_stop(struct unitree_unit_t *ut, bool force)
{
  STATE_GUARD(INITIALIZING, READY, ERROR);
  ut_set_state(ut, STOPPING);
  ut_set_state(ut, STOPPED);
  // Keep thread ids and wait join if not forced
  // End the event dispatcher
  // Free all

  return 0;
}

struct unitree_unit_t ut_create()
{
  // printk("Creating a new unit\n");
  struct unitree_unit_t new_unit;
  // printk("Size of new unit is %d\n", sizeof(new_unit));
  memset(&new_unit, 0, sizeof(struct unitree_unit_t));
  new_unit.event_q = malloc(sizeof(struct k_msgq));
  return new_unit;
}

// Registers event listener to the unit
int ut_register_event_listener(struct unitree_unit_t *ut, const struct listener_t *lst)
{
  STATE_GUARD(NOT_READY, STOPPED);
  return lr_register_listener(&(ut->evt_lst_reg), lst);
}
int ik = 10;
// Frames the incoming message with topic message pair and puts in the queue
int ut_send_event(struct unitree_unit_t *ut, const uint16_t event_id, const struct payload_t *payload)
{
  struct custom_event_t event;// = k_malloc(sizeof(struct custom_event_t));
  memset(&event, 0, sizeof(struct custom_event_t));
  event.origin = ut->def.id;
  event.id = event_id;
  printk("event->id %d\n", event.id);
  event.payload.size = payload->size;
  printk("event.payload.size %d\n", event.payload.size);
  event.payload.m = k_malloc(payload->size); // Will be freed in the consumer thread
  if (event.payload.m == NULL)
  {
    printk("event pl null\n");
  }
  printk("event.origin %d\n", event.origin);
  memcpy(event.payload.m, payload->m, payload->size);

  // printk("event.payload.m %d\n", *(int*)event.payload.m);
  printk("ut->evt_lst_reg.listener_count %d\n", ut->evt_lst_reg.listener_count);
  // printk("ut->evt_lst_reg.listeners[0].event_q %d\n", ut->evt_lst_reg.listeners[0].event_q);
  for (int i = 0; i < ut->evt_lst_reg.listener_count; ++i)
  {
    printk("(ut->event_q) %d\n", (ut->evt_lst_reg.listeners[i].event_q));
    k_msgq_put(ut->evt_lst_reg.listeners[i].event_q, &event, K_FOREVER);
  }

  return 0;
}

// Frames the incoming message with topic message pair and puts in the queue
int _ut_handle_event(struct unitree_unit_t *ut, const uint16_t *event_id, struct payload_t *payload)
{
  STATE_GUARD(READY);
  printk("ut->evt_handler_reg.count %d\n", ut->evt_handler_reg.count);
  if(payload == NULL){
    printk("payload is null\n");
  }
  int *payload_addr = malloc(sizeof(int));
  *payload_addr = (int)payload;
  printk("*payload_addr %d\n", *payload_addr);
  
  for (int i = 0; i < ut->evt_handler_reg.count; ++i)
  {
    K_THREAD_STACK_DECLARE(my_thread_stack, 1024);
    struct k_thread *evt_dsp_thread = k_malloc(sizeof(struct k_thread));
    k_thread_stack_t *stack_ptr = malloc(K_THREAD_STACK_SIZEOF(my_thread_stack));

    k_tid_t my_tid = k_thread_create(evt_dsp_thread, stack_ptr,
                                     K_THREAD_STACK_SIZEOF(my_thread_stack),
                                     ut->evt_handler_reg.cbs[i].cb, ut, event_id, payload,
                                     5, 0, K_NO_WAIT);
    k_thread_join(evt_dsp_thread, K_FOREVER);
    printk("*payload_addr %d\n", *payload_addr);
  }

  return 0;
}

int ut_set_state(struct unitree_unit_t *ut, const enum unit_state new_state)
{
  switch (new_state)
  {
  case NOT_READY:
    if (ut->state != NOT_READY)
    {
      return -1;
    }
    _ut_handle_state_not_ready(ut);
    break;
  case INITIALIZING:
    if (ut->state != NOT_READY && ut->state != STOPPED && ut->state != ERROR)
    {
      return -1;
    }
    _ut_handle_state_initializing(ut);
    break;
  case READY:
    STATE_GUARD(INITIALIZING, PAUSED);
    if (ut->state != INITIALIZING && ut->state != PAUSED)
    {
      return -1;
    }
    _ut_handle_state_ready(ut);
    break;
  case PAUSING:
    if (ut->state != READY)
    {
      return -1;
    }
    _ut_handle_state_pausing(ut);
    break;
  case PAUSED:
    if (ut->state != PAUSING)
    {
      return -1;
    }
    _ut_handle_state_paused(ut);
    break;
  case STOPPING:
    if (ut->state != READY && ut->state != PAUSED)
    {
      return -1;
    }
    _ut_handle_state_stopping(ut);
    break;
  case STOPPED:
    if (ut->state != STOPPING)
    {
      return -1;
    }
    _ut_handle_state_stopped(ut);
    break;
  case ERROR:
    _ut_handle_state_error(ut);
    break;

  default:
    return -1;
  }

  ut->state = new_state;
  return 0;
}

int _ut_handle_state_not_ready(const struct unitree_unit_t *ut)
{
  printk("Handle state 'not_ready' for the device %d.\n", ut->def.id);
  return 0;
}

int _ut_handle_state_initializing(const struct unitree_unit_t *ut)
{
  printk("Handle state 'initializing' for the device %d.\n", ut->def.id);
  return 0;
}

int _ut_handle_state_ready(const struct unitree_unit_t *ut)
{
  printk("Handle state 'ready' for the device %d.\n", ut->def.id);
  return 0;
}

int _ut_handle_state_pausing(const struct unitree_unit_t *ut)
{
  printk("Handle state 'pausing' for the device %d.\n", ut->def.id);
  return 0;
}

int _ut_handle_state_paused(const struct unitree_unit_t *ut)
{
  printk("Handle state 'paused' for the device %d.\n", ut->def.id);
  return 0;
}

int _ut_handle_state_stopping(const struct unitree_unit_t *ut)
{
  printk("Handle state 'stopping' for the device %d.\n", ut->def.id);
  return 0;
}

int _ut_handle_state_stopped(const struct unitree_unit_t *ut)
{
  printk("Handle state 'stopped' for the device %d.\n", ut->def.id);
  return 0;
}

int _ut_handle_state_error(const struct unitree_unit_t *ut)
{
  printk("Handle state 'error' for the device %d.\n", ut->def.id);
  return 0;
}
