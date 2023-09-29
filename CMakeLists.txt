# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(hello_world)

# target_include_directories(app PRIVATE components/acceleration_unit components/acceleration_service components/nvs_api)
# file(GLOB app_sources src/*.c)
# file(GLOB component_sources components/acceleration_unit/*.c)
add_subdirectory(components/acceleration_unit)
target_include_directories(app PRIVATE components/acceleration_unit)
target_sources(app PRIVATE src/main.c)
