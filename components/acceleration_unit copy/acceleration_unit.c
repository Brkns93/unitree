#include <stdio.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <acceleration_unit.h>

enum acceleration_unit_event
{
    ACCELERATION_ERROR
};

int acceleration_unit_init();
int acceleration_unit_stop(const bool force);
int acceleration_unit_run_command(const struct command_t *cmd);
int acceleration_unit_register_event_listener(const struct listener_t *);

struct custom_unit_t acceleration_unit = {
    .id = 1234,
    .init = acceleration_unit_init,
    .stop = acceleration_unit_stop,
    .run_command = acceleration_unit_run_command,
    .register_event_listener = acceleration_unit_register_event_listener
};

struct custom_unit_t* acceleration_unit_get() {
    return &acceleration_unit;
};

void handle_01_cmd(void *p1, void *p2, void *p3)
{
    uint8_t p = *(uint8_t*)p1;
	printk("handle_01_cmd %d\n", p);
    int data = 13;
    const struct payload_t payload = {
        .m = &data,
        .size = sizeof(data)
    };
    cu_send_event(&acceleration_unit, ERROR, &payload);
}

int acceleration_unit_register_event_listener(const struct listener_t *listener)
{
    return cu_hd_register_event_listener(&acceleration_unit, listener);
}

int acceleration_unit_stop(const bool force)
{
    return cu_hd_stop(&acceleration_unit, force);
}

int acceleration_unit_run_command(const struct command_t *cmd)
{
    return cu_hd_run_command(&acceleration_unit, cmd);
}

int acceleration_unit_init()
{
    const struct command_def_t cmd_01 = { .id=1, .cb=handle_01_cmd, .stack_size=50, .timeout=0 };
    cdr_register_command_def(&acceleration_unit.cmd_reg, &cmd_01);
    return cu_hd_init(&acceleration_unit);
}
