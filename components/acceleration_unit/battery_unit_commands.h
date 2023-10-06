#ifndef _BATTERY_UNIT_COMMANDS_
#define _BATTERY_UNIT_COMMANDS_

#include <stdio.h>
#include <unitree.h>

void battery_unit_hd_cmd_01(void *context, void *p2, void *p3);

static struct command_def_t battery_unit_command_defs[] = {
  {.id = 1, .cb = battery_unit_hd_cmd_01, .stack_size = 50, .timeout = 0},
};

static uint16_t battery_unit_command_count = 1;

#endif // _BATTERY_UNIT_COMMANDS_
