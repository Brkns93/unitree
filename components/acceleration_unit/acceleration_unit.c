#include <acceleration_unit.h>
#include <acc_unit_commands.h>

static int on_init()
{
	printk("Acc unit on init callback\n");
  return 0;
}

static int event_handler(const void *p1)
{
	printk("Acc unit event handler\n");

  return 0;
}

struct unitree_unit_t acc_unit_create_ut(const struct unitree_unit_def_t *unit_def)
{
	printk("Creating an acc unit with id %d\n", unit_def->id);
  struct unitree_unit_t new_unit = ut_create();
  // k_sleep(K_MSEC(100));
	printk("Created an acc unit with id %d\n", unit_def->id);
  new_unit.def = *unit_def;
  new_unit.on_init = on_init;
  new_unit.cmd_reg = (struct command_def_registry_t){.cbs = acc_unit_command_defs, .count = acc_unit_command_count};
  new_unit.evt_handler_reg = (struct callback_registry_t){.cbs = acc_unit_evt_handler_defs, .count = acc_unit_evt_handler_count};

	printk("Created an acc unit with id %d\n", unit_def->id);
  return new_unit;
}
