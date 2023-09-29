#include <stdio.h>
#include <zephyr/kernel.h>
#include <unit_communication.h>

#define STACK_SIZE 1024
#define EVENT_ID_ALL 0

const int cu_hd_run_command(struct custom_unit_t *cu, const struct command_t *cmd);
const int cu_hd_init(struct custom_unit_t *cu);
const int cu_hd_stop(struct custom_unit_t *cu, bool force);
const int cu_hd_register_event_listener(struct custom_unit_t *cu, const struct listener_t *lst);
int cu_set_state(struct custom_unit_t *cu, const enum unit_state new_state);
int cu_handle_state_not_ready(const struct custom_unit_t *cu);
int cu_handle_state_initializing(const struct custom_unit_t *cu);
int cu_handle_state_ready(const struct custom_unit_t *cu);
int cu_handle_state_pausing(const struct custom_unit_t *cu);
int cu_handle_state_paused(const struct custom_unit_t *cu);
int cu_handle_state_stopping(const struct custom_unit_t *cu);
int cu_handle_state_stopped(const struct custom_unit_t *cu);
int cu_handle_state_error(const struct custom_unit_t *cu);
void cu_event_dispatcher_thread(void *cua, void *a2, void *a3);
int add_to_thread_collection(struct thread_collection_t *tc, const k_tid_t *tid);

K_THREAD_STACK_DEFINE(my_stack_area, STACK_SIZE);
K_THREAD_STACK_DEFINE(my_stack_area2, STACK_SIZE);
// struct k_thread command_runner_thread;
int i = 0;

extern void test(void *payload, void *payload2, void *payload3)
{
    // k_sleep(K_MSEC(400));
    printk("test.\n");
    i = 5;
    while (1)
    {
        k_sleep(K_MSEC(1000));
        printk("test.\n");
    }
}
// k_tid_t my_tid;
// Dispatcher thread dispatches messages from the message queue
const int cu_hd_run_command(struct custom_unit_t *cu, const struct command_t *cmd)
{
    printk("Device %d is running command %d.\n", cu->id, cmd->id);

    const struct command_def_t *cd = cdr_get_command_def_by_id(&(cu->cmd_reg), cmd->id);
    if (cd == NULL || cd->cb == NULL)
    {
        return -1;
    }
    printk("Device %d found command %d.\n", cu->id, cmd->id);

    struct payload_t *payload = malloc(sizeof(struct payload_t));
    payload->m = malloc(cmd->payload.size);
    memcpy(payload, &cmd->payload, sizeof(struct payload_t));
    memcpy(payload->m, cmd->payload.m, sizeof(void *) * cmd->payload.size);

    K_THREAD_STACK_DECLARE(my_thread_stack, 1024);
    k_thread_stack_t *stack_ptr = malloc(K_THREAD_STACK_SIZEOF(my_thread_stack));

    struct k_thread command_runner_thread;
    k_tid_t my_tid = k_thread_create(&command_runner_thread, stack_ptr,
                                     K_THREAD_STACK_SIZEOF(my_thread_stack),
                                     cd->cb, payload, NULL, NULL,
                                     5, 0, (cd->start_delay == 0) ? K_NO_WAIT : K_MSEC(cd->start_delay));

    if (k_thread_join(&command_runner_thread, (cd->timeout == 0) ? K_FOREVER : K_MSEC(cd->timeout)))
    {
        cu_set_state(cu, ERROR);
    }

    free(payload->m);
    free(payload->m);
    free(stack_ptr);

    return 0;
}
k_tid_t my_tid;
K_HEAP_DEFINE(my_heap, 5000);
const int cu_hd_init(struct custom_unit_t *cu)
{
    printk("Device %d is initializing.\n", cu->id);
    cu_set_state(cu, INITIALIZING);
    // char __aligned(4) my_msgq_buffer[10 * sizeof(struct custom_event_t)];
    // Freeing this queue should be handled in the unit
    // struct k_msgq *event_q = malloc(sizeof(struct k_msgq));
    k_msgq_init(&(cu->event_q), cu->evt_q_buffer, sizeof(struct custom_event_t), 10);
    // cu->event_q = event_q;

    // struct k_thread *evt_dsp_thread = malloc(sizeof(evt_dsp_thread));
    // // k_thread_stack_t *thread_stack = k_thread_stack_alloc(STACK_SIZE, 0);
    // my_tid = k_thread_create(evt_dsp_thread, my_stack_area,
    //                                  K_THREAD_STACK_SIZEOF(my_stack_area),
    //                                  test,
    //                                  NULL, NULL, NULL,
    //                                  5, 0, K_NO_WAIT);

    // cu->evt_dsp_th = my_tid;
    // k_sleep(K_MSEC(1000));
    // TODO: Check error
    // printk("state %d is initialized.\n", cu->id);
    // cu_set_state(cu, READY);
    // printk("Device %d is initialized.\n", cu->id);
    return 0;
}

const int cu_hd_stop(struct custom_unit_t *cu, bool force)
{
    cu_set_state(cu, STOPPING);
    cu_set_state(cu, STOPPED);
    // Keep thread ids and wait join if not forced
    // End the event dispatcher
    // Free all

    return 0;
}

const int cu_hd_register_event_listener(struct custom_unit_t *cu, const struct listener_t *lst)
{
    return lr_register_listener(&(cu->evt_lst_reg), lst);
}

// static struct custom_unit_t cu_create()
// {
//     struct custom_unit_t cu;
//     memset(&cu, 0, sizeof(struct custom_unit_t));
//     return cu;
// }

// Frames the incoming message with topic message pair and puts in the queue
void cu_send_event(const struct custom_unit_t *cu, const uint16_t event_id, const struct payload_t *payload)
{
    struct custom_event_t event; // = malloc(sizeof(struct custom_event_t *));
    event.id = event_id;
    event.payload.size = payload->size;
    event.payload.m = malloc(payload->size); // Will be freed in the consumer thread
    memcpy(event.payload.m, payload->m, payload->size);

    k_msgq_put(&(cu->event_q), &event, K_FOREVER);
}

int cu_set_state(struct custom_unit_t *cu, const enum unit_state new_state)
{
    switch (new_state)
    {
    case NOT_READY:
        if (cu->state != NOT_READY)
        {
            return -1;
        }
        cu_handle_state_not_ready(cu);
        break;
    case INITIALIZING:
        if (cu->state != NOT_READY && cu->state != STOPPED && cu->state != ERROR)
        {
            return -1;
        }
        cu_handle_state_initializing(cu);
        break;
    case READY:
        if (cu->state != INITIALIZING && cu->state != PAUSED)
        {
            return -1;
        }
        cu_handle_state_ready(cu);
        break;
    case PAUSING:
        if (cu->state != READY)
        {
            return -1;
        }
        cu_handle_state_pausing(cu);
        break;
    case PAUSED:
        if (cu->state != PAUSING)
        {
            return -1;
        }
        cu_handle_state_paused(cu);
        break;
    case STOPPING:
        if (cu->state != READY && cu->state != PAUSED)
        {
            return -1;
        }
        cu_handle_state_stopping(cu);
        break;
    case STOPPED:
        if (cu->state != STOPPING)
        {
            return -1;
        }
        cu_handle_state_stopped(cu);
        break;
    case ERROR:
        cu_handle_state_error(cu);
        break;

    default:
        return -1;
    }

    cu->state = new_state;
    return 0;
}

int cu_handle_state_not_ready(const struct custom_unit_t *cu)
{
    return 0;
}

int cu_handle_state_initializing(const struct custom_unit_t *cu)
{
    return 0;
}

int cu_handle_state_ready(const struct custom_unit_t *cu)
{
    return 0;
}

int cu_handle_state_pausing(const struct custom_unit_t *cu)
{
    return 0;
}

int cu_handle_state_paused(const struct custom_unit_t *cu)
{
    return 0;
}

int cu_handle_state_stopping(const struct custom_unit_t *cu)
{
    return 0;
}

int cu_handle_state_stopped(const struct custom_unit_t *cu)
{
    return 0;
}

int cu_handle_state_error(const struct custom_unit_t *cu)
{
    return 0;
}

// Dispatcher thread dispatches messages from the message queue
void cu_event_dispatcher_thread(void *cua, void *a2, void *a3)
{
    printk("cu_event_dispatcher_thread\n");
    struct custom_unit_t *cu = (struct custom_unit_t *)cua;

    // TODO: Implement stop mechanisim
    while (1)
    {
        printk("cu_event_dispatcher_thread\n");
        k_sleep(K_MSEC(1000));
    }
    while (1)
    {
        struct custom_event_t rcv_event;
        k_msgq_get(&(cu->event_q), &rcv_event, K_FOREVER); // Wait for incoming messages

        uint16_t lst_count = cu->evt_lst_reg.listener_count;
        // Find event listeners and invoke their callback
        for (int i = 0; i < lst_count; ++i)
        {
            const struct listener_t *reg_lst = &(cu->evt_lst_reg.listeners[i]);
            if (reg_lst->event_id == rcv_event.id || reg_lst->event_id == EVENT_ID_ALL)
            {
                (*(reg_lst->cb))((void *)(&rcv_event));
            }
        }
        free(rcv_event.payload.m); // Frees payload allocated in send_event
    }
}

int add_to_thread_collection(struct thread_collection_t *tc, const k_tid_t *tid)
{
    if (tc->count == 0)
    {
        tc->ids = (k_tid_t *)malloc(sizeof(k_tid_t));
    }
    else
    {
        tc->ids = (k_tid_t *)realloc(tc->ids + tc->count, sizeof(k_tid_t) * (tc->count + 1));
    }
    if (tc->ids == NULL)
    {
        return -1;
    }
    memcpy(tc->ids + tc->count, tid, sizeof(k_tid_t));
    tc->count++;

    return 0;
}
