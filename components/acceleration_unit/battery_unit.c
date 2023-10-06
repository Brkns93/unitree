#include <battery_unit.h>
#include <battery_unit_commands.h>

// static int on_init()
// {
// 	printk("Battery unit on init callback\n");
//   return 1;
// }

struct unitree_unit_t battery_unit_create_ut(const struct unitree_unit_def_t *unit_def)
{
	printk("Creating a battery unit with id %d\n", unit_def->id);
  struct unitree_unit_t new_unit = ut_create();
  new_unit.def = *unit_def;
  // new_unit.on_init = on_init;
  new_unit.cmd_reg = (struct command_def_registry_t){.cbs = battery_unit_command_defs, .count = battery_unit_command_count};

  return new_unit;
}
