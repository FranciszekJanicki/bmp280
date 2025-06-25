#ifndef BMP280_BMP280_H
#define BMP280_BMP280_H

#include "bmp280_config.h"
#include "bmp280_registers.h"

typedef struct {
    bmp280_config_t config;
    bmp280_interface_t interface;
    bmp280_trim_data_t trim_data;
} bmp280_t;

bmp280_err_t bmp280_initialize(bmp280_t* bmp280, bmp280_config_t const* config, bmp280_interface_t const* interface);
bmp280_err_t bmp280_deinitialize(bmp280_t* bmp280);

bmp280_err_t bmp280_get_temp_data(bmp280_t* bmp280, float32_t* data);
bmp280_err_t bmp280_get_press_data(bmp280_t* bmp280, int32_t* data);

bmp280_err_t bmp280_get_trim_data(bmp280_t const* bmp280, bmp280_trim_data_t* trim_data);

bmp280_err_t bmp280_set_mode(bmp280_t const* bmp280, bmp280_mode_t mode);
bmp280_err_t bmp280_get_mode(bmp280_t const* bmp280, bmp280_mode_t* mode);

bmp280_err_t bmp280_get_calib_reg(bmp280_t const* bmp280, uint8_t num, bmp280_calib_reg_t* reg);
bmp280_err_t bmp280_set_calib_reg(bmp280_t const* bmp280, uint8_t num, bmp280_calib_reg_t const* reg);

bmp280_err_t bmp280_get_reset_reg(bmp280_t const* bmp280, bmp280_reset_reg_t* reg);
bmp280_err_t bmp280_set_reset_reg(bmp280_t const* bmp280, bmp280_reset_reg_t const* reg);

bmp280_err_t bmp280_get_config_reg(bmp280_t const* bmp280, bmp280_config_reg_t* reg);
bmp280_err_t bmp280_set_config_reg(bmp280_t const* bmp280, bmp280_config_reg_t const* reg);

bmp280_err_t bmp280_get_ctrl_meas_reg(bmp280_t const* bmp280, bmp280_ctrl_meas_reg_t* reg);
bmp280_err_t bmp280_set_ctrl_meas_reg(bmp280_t const* bmp280, bmp280_ctrl_meas_reg_t const* reg);

bmp280_err_t bmp280_get_id_reg(bmp280_t const* bmp280, bmp280_id_reg_t* reg);

bmp280_err_t bmp280_get_status_reg(bmp280_t const* bmp280, bmp280_status_reg_t* reg);

bmp280_err_t bmp280_get_press_reg(bmp280_t const* bmp280, bmp280_press_reg_t* reg);

bmp280_err_t bmp280_get_press_x_reg(bmp280_t const* bmp280, bmp280_press_x_reg_t* reg);

bmp280_err_t bmp280_get_temp_reg(bmp280_t const* bmp280, bmp280_temp_reg_t* reg);

bmp280_err_t bmp280_get_temp_x_reg(bmp280_t const* bmp280, bmp280_temp_x_reg_t* reg);

#endif // BMP280_BMP280_H