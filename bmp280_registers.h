#ifndef BMP280_BMP280_REGISTERS_H
#define BMP280_BMP280_REGISTERS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t calibration_data : 16;
} bmp280_calib_reg_t;

typedef struct {
    uint8_t chip_id : 8;
} bmp280_id_reg_t;

typedef struct {
    uint8_t reset : 8;
} bmp280_reset_reg_t;

typedef struct {
    uint8_t measuring : 1;
    uint8_t im_update : 1;
} bmp280_status_reg_t;

typedef struct {
    uint8_t osrs_t : 3;
    uint8_t osrs_p : 3;
    uint8_t mode : 2;
} bmp280_ctrl_meas_reg_t;

typedef struct {
    uint8_t spi3w_en : 1;
    uint8_t filter : 3;
    uint8_t t_sb : 3;
} bmp280_config_reg_t;

typedef struct {
    int16_t press : 16;
} bmp280_press_reg_t;

typedef struct {
    uint8_t press_x : 4;
} bmp280_press_x_reg_t;

typedef struct {
    int16_t temp : 16;
} bmp280_temp_reg_t;

typedef struct {
    uint8_t temp_x : 4;
} bmp280_temp_x_reg_t;

#ifdef __cplusplus
}
#endif

#endif // BMP280_BMP280_REGISTERS_H