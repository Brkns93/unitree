#include <stdio.h>
#include <acc_unit_commands.h>

void acc_unit_hd_cmd_01(void *ut, void *p2, void *p3){
  printk("acc_unit_hd_cmd_01\n");
  int data = 13;
  const struct payload_t payload = {
      .m = &data,
      .size = sizeof(data)};
  ut_send_event(*(struct unitree_unit_t**)ut, ERROR, &payload);
  printk("acc_unit_hd_cmd_01 event sent\n");
}
