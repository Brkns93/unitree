#include <stdio.h>
#include <acc_unit_commands.h>

void acc_unit_hd_cmd_01(void *ut, void *p2, void *p3){
  printk("acc_unit_hd_cmd_01\n");
  int data = 13;
  const struct payload_t payload = {
      .m = &data,
      .size = sizeof(data)};
  printk("ut->def.id %d\n", ((struct unitree_unit_t*)ut)->def.id);
  ut_send_event((struct unitree_unit_t*)ut, ERROR, &payload);
  printk("acc_unit_hd_cmd_01 event sent\n");
}

void acc_unit_hd_battery_unit_events(void *ut, void *event_id, void *payload){
  printk("acc_unit_hd_battery_unit_events\n");
  printk("ut->def.id %d\n", ((struct unitree_unit_t*)ut)->def.id);
  printk("event_id %u\n", *(uint16_t*)event_id);
}
