#include <stdio.h>
#include <battery_unit_commands.h>

void battery_unit_hd_cmd_01(void *ut, void *p2, void *p3){
  printk("battery_unit_hd_cmd_01\n");
  int data = 13;
  const struct payload_t payload = {
      .m = &data,
      .size = sizeof(data)};
  ut_send_event((struct unitree_unit_t*)ut, ERROR, &payload);
  printk("battery_unit_hd_cmd_01 event sent\n");
}
