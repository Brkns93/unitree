#ifndef _ACC_UNIT_COMMANDS_
#define _ACC_UNIT_COMMANDS_

#include <stdio.h>
#include <unitree.h>

void acc_unit_hd_cmd_01(void *context, void *p2, void *p3);

void acc_unit_hd_battery_unit_events(void *context, void *p2, void *p3);

static struct command_def_t acc_unit_command_defs[] = {
  {.id = 1, .cb = acc_unit_hd_cmd_01, .stack_size = 50, .timeout = 0},
};

static uint16_t acc_unit_command_count = 1;


static struct command_def_t acc_unit_evt_handler_defs[] = {
  {.id = 1, .cb = acc_unit_hd_battery_unit_events, .stack_size = 50, .timeout = 0},
};

static uint16_t acc_unit_evt_handler_count = 1;

#endif // _ACC_UNIT_COMMANDS_
