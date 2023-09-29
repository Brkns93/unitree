#include <stdio.h>
#include <zephyr/kernel.h>
#include <unitree.h>

#define STACK_SIZE 1024
#define EVENT_ID_ALL 0

int ut_run_command(struct unitree_unit_t *ut, const struct command_t *cmd);
int ut_init(struct unitree_unit_t *ut);
int ut_stop(struct unitree_unit_t *ut, bool force);
int ut_register_event_listener(struct unitree_unit_t *ut, const struct listener_t *lst);

// Private functions
int ut_set_state(struct unitree_unit_t *ut, const enum unit_state new_state);
int ut_handle_state_not_ready(const struct unitree_unit_t *ut);
int ut_handle_state_initializing(const struct unitree_unit_t *ut);
int ut_handle_state_ready(const struct unitree_unit_t *ut);
int ut_handle_state_pausing(const struct unitree_unit_t *ut);
int ut_handle_state_paused(const struct unitree_unit_t *ut);
int ut_handle_state_stopping(const struct unitree_unit_t *ut);
int ut_handle_state_stopped(const struct unitree_unit_t *ut);
int ut_handle_state_error(const struct unitree_unit_t *ut);
void ut_event_dispatcher_thread(void *uta, void *a2, void *a3);

// Dispatcher thread dispatches messages from the message queue
void ut_event_dispatcher_thread(void *uta, void *a2, void *a3)
{
  printk("ut_event_dispatcher_thread\n");

  // TODO: Implement stop mechanisim
  while (1)
  {
    printk("ut_event_dispatcher_thread\n");
    k_sleep(K_MSEC(1000));
  }
  struct unitree_unit_t *ut = *(struct unitree_unit_t **)uta;
  while (1)
  {
    struct custom_event_t rcv_event;
    k_msgq_get(&(ut->event_q), &rcv_event, K_FOREVER); // Wait for incoming messages

    struct callback_t *cb = cb_get_callback_by_id(&(ut->evt_hnd_reg), rcv_event.origin);
    if (cb->cb != NULL)
    {
      cb->cb(&rcv_event, NULL, NULL);
    }
    else
    {
      ut_set_state(ut, ERROR);
    }

    free(rcv_event.payload.m); // Frees payload allocated in send_event
  }
}

// Dispatcher thread dispatches messages from the message queue
int ut_run_command(struct unitree_unit_t *ut, const struct command_t *cmd)
{
  printk("Device %d is running command %d.\n", ut->id, cmd->id);

  const struct command_def_t *cd = cdr_get_command_def_by_id(&(ut->cmd_reg), cmd->id);
  if (cd == NULL || cd->cb == NULL)
  {
    return -1;
  }
  printk("Device %d found command %d.\n", ut->id, cmd->id);

  struct payload_t *payload = malloc(sizeof(struct payload_t));
  payload->m = malloc(cmd->payload.size);
  memcpy(payload, &cmd->payload, sizeof(struct payload_t));
  memcpy(payload->m, cmd->payload.m, sizeof(void *) * cmd->payload.size);

  K_THREAD_STACK_DECLARE(my_thread_stack, 1024);
  k_thread_stack_t *stack_ptr = malloc(K_THREAD_STACK_SIZEOF(my_thread_stack));

  struct k_thread command_runner_thread;
  k_tid_t my_tid = k_thread_create(&command_runner_thread, stack_ptr,
                                   K_THREAD_STACK_SIZEOF(my_thread_stack),
                                   cd->cb, &ut, payload, NULL,
                                   5, 0, (cd->start_delay == 0) ? K_NO_WAIT : K_MSEC(cd->start_delay));

  if (k_thread_join(&command_runner_thread, (cd->timeout == 0) ? K_FOREVER : K_MSEC(cd->timeout)))
  {
    ut_set_state(ut, ERROR);
  }

  free(payload->m);
  free(payload->m);
  free(stack_ptr);

  printk("Device %d - command %d is finished.\n", ut->id, cmd->id);

  return 0;
}

int ut_init(struct unitree_unit_t *ut)
{
  printk("Device %d is initializing.\n", ut->id);

  ut_set_state(ut, INITIALIZING);

  if (ut->on_init != NULL)
  {
    ut->on_init();
  }

  k_msgq_init(&(ut->event_q), ut->evt_q_buffer, sizeof(struct custom_event_t), 10);

  struct k_thread *evt_dsp_thread = k_malloc(sizeof(evt_dsp_thread));
  K_THREAD_STACK_DECLARE(my_thread_stack, 1024);
  k_thread_stack_t *stack_ptr = malloc(K_THREAD_STACK_SIZEOF(my_thread_stack));

  k_tid_t my_tid = k_thread_create(evt_dsp_thread, stack_ptr,
                                   K_THREAD_STACK_SIZEOF(my_thread_stack),
                                   ut_event_dispatcher_thread,
                                   &ut, NULL, NULL,
                                   5, 0, K_NO_WAIT);
  // ut->evt_dsp_th = my_tid;
  ut->evt_dsp_th = evt_dsp_thread;

  // // k_sleep(K_MSEC(1000));
  // ut_set_state(ut, READY);
  // while (1)
  // {
  //   k_sleep(K_MSEC(1000));
  // }

  printk("Device %d is initialized.\n", ut->id);
  return 0;
}

int ut_stop(struct unitree_unit_t *ut, bool force)
{
  ut_set_state(ut, STOPPING);
  ut_set_state(ut, STOPPED);
  // Keep thread ids and wait join if not forced
  // End the event dispatcher
  // Free all

  return 0;
}

struct unitree_unit_t ut_create()
{
  struct unitree_unit_t new_unit;
  memset(&new_unit, 0, sizeof(struct unitree_unit_t));
  return new_unit;
}

// Registers event listener to the unit
int ut_register_event_listener(struct unitree_unit_t *ut, const struct listener_t *lst)
{
  return lr_register_listener(&(ut->evt_lst_reg), lst);
}

// Frames the incoming message with topic message pair and puts in the queue
int ut_send_event(struct unitree_unit_t *ut, const uint16_t event_id, const struct payload_t *payload)
{
  struct custom_event_t event; // = malloc(sizeof(struct custom_event_t *));
  event.origin = ut->id;
  event.id = event_id;
  event.payload.size = payload->size;
  event.payload.m = malloc(payload->size); // Will be freed in the consumer thread
  memcpy(event.payload.m, payload->m, payload->size);

  printk("ut->evt_lst_reg.listener_count %d\n", ut->evt_lst_reg.listener_count);
  for (int i = 0; i < ut->evt_lst_reg.listener_count; ++i)
  {
    k_msgq_put(ut->evt_lst_reg.listeners[i].event_q, &event, K_FOREVER);
  }
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
    ut_handle_state_not_ready(ut);
    break;
  case INITIALIZING:
    if (ut->state != NOT_READY && ut->state != STOPPED && ut->state != ERROR)
    {
      return -1;
    }
    ut_handle_state_initializing(ut);
    break;
  case READY:
    if (ut->state != INITIALIZING && ut->state != PAUSED)
    {
      return -1;
    }
    ut_handle_state_ready(ut);
    break;
  case PAUSING:
    if (ut->state != READY)
    {
      return -1;
    }
    ut_handle_state_pausing(ut);
    break;
  case PAUSED:
    if (ut->state != PAUSING)
    {
      return -1;
    }
    ut_handle_state_paused(ut);
    break;
  case STOPPING:
    if (ut->state != READY && ut->state != PAUSED)
    {
      return -1;
    }
    ut_handle_state_stopping(ut);
    break;
  case STOPPED:
    if (ut->state != STOPPING)
    {
      return -1;
    }
    ut_handle_state_stopped(ut);
    break;
  case ERROR:
    ut_handle_state_error(ut);
    break;

  default:
    return -1;
  }

  ut->state = new_state;
  return 0;
}

int ut_handle_state_not_ready(const struct unitree_unit_t *ut)
{
  return 0;
}

int ut_handle_state_initializing(const struct unitree_unit_t *ut)
{
  return 0;
}

int ut_handle_state_ready(const struct unitree_unit_t *ut)
{
  return 0;
}

int ut_handle_state_pausing(const struct unitree_unit_t *ut)
{
  return 0;
}

int ut_handle_state_paused(const struct unitree_unit_t *ut)
{
  return 0;
}

int ut_handle_state_stopping(const struct unitree_unit_t *ut)
{
  return 0;
}

int ut_handle_state_stopped(const struct unitree_unit_t *ut)
{
  return 0;
}

int ut_handle_state_error(const struct unitree_unit_t *ut)
{
  return 0;
}
