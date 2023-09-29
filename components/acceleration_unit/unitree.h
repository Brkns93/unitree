#ifndef _UNITREE_
#define _UNITREE_

#include <stdio.h>
#include <zephyr/kernel.h>
#include <listener_registry.h>
#include <command_def_registry.h>
#include <callback_registry.h>

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
    uint16_t origin; // Id of the producer unit
    struct payload_t payload;
} __attribute__((aligned(4)));

struct command_t
{
    uint16_t id;
    struct payload_t payload;
} __attribute__((aligned(4)));


struct unitree_unit_registry_t {
  struct unitree_unit_t *units;
  uint16_t count;
};

int uur_register_unitree_unit(struct unitree_unit_registry_t* unit_reg, const struct unitree_unit_t* unitree_unit){
    if(unit_reg->count == 0){
        unit_reg->units = malloc(sizeof(struct unitree_unit_t));
    }
    else{
        unit_reg->units = realloc(unit_reg->units, sizeof(struct unitree_unit_t)*(unit_reg->count+1));
    }
    if(unit_reg->units == NULL){
        return -1;
    }
    memcpy(unit_reg->units+unit_reg->count, unitree_unit, sizeof(struct unitree_unit_t));
    unit_reg->count++;
    
    return 0;
}


struct unitree_unit_t {
    uint16_t id;
    enum unit_state state;
    struct command_def_registry_t cmd_reg;
    struct callback_registry_t evt_hnd_reg;
    struct listener_registry_t evt_lst_reg;
    struct unitree_unit_registry_t subunit_reg;
    char __aligned(4) evt_q_buffer[10 * sizeof(struct custom_event_t)];
    struct k_msgq event_q;
    struct k_thread * evt_dsp_th;
    int (*on_init)();
    int (*on_config)();
    int (*on_stop)(const bool force);
};

struct unitree_unit_t ut_create();
int ut_run_command(struct unitree_unit_t *cu, const struct command_t *cmd);
int ut_init(struct unitree_unit_t *cu);
int ut_stop(struct unitree_unit_t *cu, const bool force);
int ut_register_event_listener(struct unitree_unit_t *cu, const struct listener_t *lst);
int ut_send_event(struct unitree_unit_t *cu, const uint16_t event_id, const struct payload_t *payload);


#endif // _UNITREE_
