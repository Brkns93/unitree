# find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
# target_include_directories(app PRIVATE ../acceleration_service)

# add_subdirectory(../acceleration_service)

target_sources(
    app PRIVATE "acceleration_unit.c" "unit_communication.c" "listener_registry.c" "command_def_registry.c"
)
