/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <acceleration_unit.h>
#include <battery_unit.h>

int cb(const void *event)
{
	printk("cb\n");
	return 0;
}

struct acc_unit_context_t
{
	uint32_t r_loop;
	uint32_t e_loop;
};

struct acc_unit_config_t
{
	uint32_t spi_pin;
	uint32_t frequency;
};

struct battery_unit_context_t
{
	uint32_t r_loop;
	uint32_t e_loop;
};

struct battery_unit_config_t
{
	uint32_t spi_pin;
	uint32_t frequency;
};

struct unitree_unit_t r(const struct unitree_unit_def_t *unit_def){
	struct unitree_unit_t nu;
	return nu;
}

int main(void)
{
	printk("Starting! Board: %s\n", CONFIG_BOARD);
	struct battery_unit_context_t battery_unit_context = {
		.r_loop = 120,
		.e_loop = 18000};
	const struct battery_unit_config_t battery_unit_config = {
		.spi_pin = 18,
		.frequency = 200000};
	const struct unitree_unit_def_t battery_unit_def = {
		.id = 4,
		.config = {.p = (void *)&battery_unit_config, .size = sizeof(struct battery_unit_config_t)},
		.context = {.p = (void *)&battery_unit_context, .size = sizeof(struct battery_unit_context_t)}};
	struct acc_unit_context_t acc_unit_context = {
		.r_loop = 120,
		.e_loop = 18000};
	const struct acc_unit_config_t acc_unit_config = {
		.spi_pin = 12,
		.frequency = 200000};
	const struct unitree_unit_def_t acc_unit_def = {
		.id = 5,
		.config = {.p = (void *)&acc_unit_config, .size = sizeof(struct acc_unit_config_t)},
		.context = {.p = (void *)&acc_unit_context, .size = sizeof(struct acc_unit_context_t)}};
	struct unitree_unit_t acc_unit = acc_unit_create_ut(&acc_unit_def);
	// k_sleep(K_MSEC(1000));
	// struct unitree_unit_t bat_unit = r(&battery_unit_def);
	struct unitree_unit_t bat_unit = battery_unit_create_ut(&battery_unit_def);

	ut_register_subunit(&acc_unit, &bat_unit);

	ut_init(&acc_unit);

	k_sleep(K_MSEC(1000));

	uint8_t pl = 13;
	const struct command_t cmd = { .id = 1, .payload = { .m = &pl, .size = sizeof(pl) } };
	// ut_run_command(&acc_unit, &cmd);
	ut_run_command(&bat_unit, &cmd);

	// uint8_t pl = 13;
	// const struct command_t cmd = {.id = 1, .payload = {.m = &pl, .size = sizeof(pl)}};
	// ut_run_command(&acc_unit, &cmd);

	while (1)
	{
		k_sleep(K_MSEC(1000));
	};

	return 0;
}
