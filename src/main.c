/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <acceleration_unit.h>

int cb(const void* event){
	printk("cb\n");
}

int main(void)
{
	struct unitree_unit_t acc_unit = acc_unit_create_ut();
	struct unitree_unit_t acc_unit2 = acc_unit_create_ut();
	ut_init(&acc_unit);
	ut_init(&acc_unit2);
	uint8_t pl = 13;
	const struct command_t cmd = { .id = 1, .payload = { .m = &pl, .size = sizeof(pl) } };
	ut_run_command(&acc_unit, &cmd);
	// k_sleep(K_MSEC(500));
	printk("Hello World! %s\n", CONFIG_BOARD);
	printk("Starting! %s\n", CONFIG_BOARD);
	while(1){
		k_sleep(K_MSEC(500));
		printk("Hello World! %s\n", CONFIG_BOARD);
	};
	return 0;
}
