#include <acceleration_unit.h>
#include <acc_unit_commands.h>


int on_init(){
 return 0;
}

struct unitree_unit_t acc_unit_create_ut()
{
  struct unitree_unit_t new_unit = ut_create();
  new_unit.id = 18;
  new_unit.on_init = on_init;
  // new_unit.subunit_reg = (struct unitree_unit_registry_t ) {.units={...}, .count=acc_unit_command_count};
  new_unit.cmd_reg = (struct command_def_registry_t) {.cbs=acc_unit_command_defs, .count=acc_unit_command_count};

  return new_unit;
}
