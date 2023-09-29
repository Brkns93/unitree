#ifndef _UNIT_COMMUNICATION_
#define _UNIT_COMMUNICATION_

#include <stdio.h>
#include <zephyr/kernel.h>
#include <listener_registry.h>
#include <command_def_registry.h>


enum unit_state
{
    NOT_READY,
    INITIALIZING,
    READY,
    PAUSING,
    PAUSED,
    STOPPING,
    STOPPED,
    ERROR,
};

struct payload_t
{
    void *m;
    size_t size;
};

// Queue structures
struct custom_event_t
{
    uint16_t id;
    struct payload_t payload;
} __attribute__((aligned(4)));

struct command_t
{
    uint16_t id;
    struct payload_t payload;
} __attribute__((aligned(4)));


struct thread_collection_t
{
    k_tid_t *ids;
    size_t count;
};

struct custom_unit_t
{
    uint16_t id;
    enum unit_state state;
    struct command_def_registry_t cmd_reg;
    struct k_msgq event_q;
    struct listener_registry_t evt_lst_reg;
    struct thread_collection_t cmd_threads;
    k_tid_t evt_dsp_th;
    char __aligned(4) evt_q_buffer[10 * sizeof(struct custom_event_t)];
    int (*set_config)(const void *config);
    int (*init)();
    int (*stop)(const bool force);
    int (*register_event_listener)(const struct listener_t *listener);
    int (*run_command)(const struct command_t *command);
};

struct unit_registry_t
{
    struct custom_unit_t units[16];
    size_t unit_count;
    int (*register_unit)(const struct custom_unit_t *config);
};

// Custom unit handler definitions
const int cu_hd_run_command(struct custom_unit_t *cu, const struct command_t *cmd);
const int cu_hd_init(struct custom_unit_t *cu);
const int cu_hd_stop(struct custom_unit_t *cu, const bool force);
const int cu_hd_register_event_listener(struct custom_unit_t *cu, const struct listener_t *lst);

// Public functions
// static struct custom_unit_t cu_create();
void cu_send_event(const struct custom_unit_t *cu, const uint16_t event_id, const struct payload_t *payload);


#endif // _UNIT_COMMUNICATION_
