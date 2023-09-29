#ifndef _ACCELERATION_UNIT_
#define _ACCELERATION_UNIT_

#include <stdio.h>
#include <unit_communication.h>

typedef enum acceleration_return_code {
    ACCELERATION_UNIT_FAIL = -1,
    ACCELERATION_UNIT_SUCCESS = 0,
    ENABLE_SENSOR_ERR = 1101,
    DISABLE_SENSOR_ERR = 1102,
    SENSOR_NOT_AVAILABLE = 1103,
    ACC_SERVICE_INIT_ERR = 1104,
    SPI_0_INIT_ERR = 1105,
    ADXL357_DRIVER_INIT_ERR = 1106,
} acceleration_return_code;

int acceleration_unit_init();
struct custom_unit_t* acceleration_unit_get();


#endif // _ACCELERATION_UNIT_
