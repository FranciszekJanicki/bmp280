#include "bmp280.h"
#include <assert.h>
#include <math.h>
#include <string.h>

static bmp280_err_t bmp280_bus_init(bmp280_t const* bmp280)
{
    return bmp280->interface.bus_init ? bmp280->interface.bus_init(bmp280->interface.bus_user)
                                      : BMP280_ERR_NULL;
}

static bmp280_err_t bmp280_bus_deinit(bmp280_t const* bmp280)
{
    return bmp280->interface.bus_deinit ? bmp280->interface.bus_deinit(bmp280->interface.bus_user)
                                        : BMP280_ERR_NULL;
}

static bmp280_err_t bmp280_bus_write(bmp280_t const* bmp280,
                                     uint8_t address,
                                     uint8_t const* data,
                                     size_t data_size)
{
    return bmp280->interface.bus_write
               ? bmp280->interface.bus_write(bmp280->interface.bus_user, address, data, data_size)
               : BMP280_ERR_NULL;
}

static bmp280_err_t bmp280_bus_read(bmp280_t const* bmp280,
                                    uint8_t address,
                                    uint8_t* data,
                                    size_t data_size)
{
    return bmp280->interface.bus_read
               ? bmp280->interface.bus_read(bmp280->interface.bus_user, address, data, data_size)
               : BMP280_ERR_NULL;
}

bmp280_err_t bmp280_initialize(bmp280_t* bmp280,
                               bmp280_config_t const* config,
                               bmp280_interface_t const* interface)
{
    assert(bmp280 && config && interface);

    memset(bmp280, 0, sizeof(*bmp280));
    memcpy(&bmp280->config, config, sizeof(*config));
    memcpy(&bmp280->interface, interface, sizeof(*interface));

    return bmp280_bus_init(bmp280);
}

bmp280_err_t bmp280_deinitialize(bmp280_t* bmp280)
{
    assert(bmp280);

    bmp280_err_t err = bmp280_bus_deinit(bmp280);

    memset(bmp280, 0, sizeof(*bmp280));

    return err;
}

bmp280_err_t bmp280_get_temp_data(bmp280_t* bmp280, float32_t* data)
{
    assert(bmp280 && data);

    bmp280_mode_t mode = BMP280_MODE_FORCED;

    bmp280_err_t err = bmp280_set_mode(bmp280, mode);

    do {
        err |= bmp280_get_mode(bmp280, &mode);
    } while (mode == BMP280_MODE_SLEEP);

    bmp280_temp_reg_t reg = {};

    int32_t adc_T = reg.temp >> 4;
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)bmp280->trim_data.t1 << 1))) *
                    ((int32_t)bmp280->trim_data.t2)) >>
                   11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)bmp280->trim_data.t1)) *
                      ((adc_T >> 4) - ((int32_t)bmp280->trim_data.t1))) >>
                     12) *
                    ((int32_t)bmp280->trim_data.t3)) >>
                   14;
    bmp280->trim_data.t_fine = var1 + var2;

    *data = (float32_t)((bmp280->trim_data.t_fine * 5 + 128) >> 8) / 100;

    return err;
}

bmp280_err_t bmp280_get_press_data(bmp280_t* bmp280, int32_t* data)
{
    assert(bmp280 && data);

    float32_t temp = {};
    bmp280_press_reg_t reg = {};

    bmp280_err_t err = bmp280_get_temp_data(bmp280, &temp);
    err |= bmp280_get_press_reg(bmp280, &reg);

    int32_t adc_P = reg.press >> 4;
    int64_t var1 = ((int64_t)bmp280->trim_data.t_fine) - 128000;
    int64_t var2 = var1 * var1 * (int64_t)bmp280->trim_data.p6;
    var2 += ((var1 * (int64_t)bmp280->trim_data.p5) << 17);
    var2 += (((int64_t)bmp280->trim_data.p4) << 35);
    var1 = ((var1 * var1 * (int64_t)bmp280->trim_data.p3) >> 8) +
           ((var1 * (int64_t)bmp280->trim_data.p2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)bmp280->trim_data.p1) >> 33;

    assert(var1 != 0);

    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)bmp280->trim_data.p9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)bmp280->trim_data.p8) * p) >> 19;

    ((p + var1 + var2) >> 8) + (((int64_t)bmp280->trim_data.p7) << 4) / 256;

    return err;
}

bmp280_err_t bmp280_get_trim_data(bmp280_t const* bmp280, bmp280_trim_data_t* trim_data)
{
    assert(bmp280 && trim_data);

    uint8_t data[24] = {};

    bmp280_err_t err = bmp280_bus_read(bmp280, BMP280_REG_ADDRESS_CALIB00, data, sizeof(data));

    trim_data->t1 = (uint16_t)(((data[1] & 0xFFU) << 8U) | (data[0] & 0xFFU));
    trim_data->t2 = (int16_t)(((data[3] & 0xFF) << 8) | (data[2] & 0xFF));
    trim_data->t3 = (uint16_t)(((data[5] & 0xFFU) << 8U) | (data[4] & 0xFFU));
    trim_data->p1 = (uint16_t)(((data[7] & 0xFFU) << 8U) | (data[6] & 0xFFU));
    trim_data->p2 = (int16_t)(((data[9] & 0xFF) << 8) | (data[8] & 0xFF));
    trim_data->p3 = (int16_t)(((data[11] & 0xFF) << 8) | (data[10] & 0xFF));
    trim_data->p4 = (int16_t)(((data[13] & 0xFF) << 8) | (data[12] & 0xFF));
    trim_data->p5 = (int16_t)(((data[15] & 0xFF) << 8) | (data[14] & 0xFF));
    trim_data->p6 = (int16_t)(((data[17] & 0xFF) << 8) | (data[16] & 0xFF));
    trim_data->p7 = (int16_t)(((data[19] & 0xFF) << 8) | (data[18] & 0xFF));
    trim_data->p8 = (int16_t)(((data[21] & 0xFF) << 8) | (data[20] & 0xFF));
    trim_data->p9 = (int16_t)(((data[23] & 0xFF) << 8) | (data[22] & 0xFF));

    return err;
}

bmp280_err_t bmp280_set_mode(bmp280_t const* bmp280, bmp280_mode_t mode)
{
    assert(bmp280);

    bmp280_ctrl_meas_reg_t reg = {};

    bmp280_err_t err = bmp280_get_ctrl_meas_reg(bmp280, &reg);
    reg.mode = mode;
    err |= bmp280_set_ctrl_meas_reg(bmp280, &reg);

    return err;
}

bmp280_err_t bmp280_get_mode(bmp280_t const* bmp280, bmp280_mode_t* mode)
{
    assert(bmp280 && mode);

    bmp280_ctrl_meas_reg_t reg = {};

    bmp280_err_t err = bmp280_get_ctrl_meas_reg(bmp280, &reg);
    *mode = reg.mode;

    return err;
}

bmp280_err_t bmp280_get_calib_reg(bmp280_t const* bmp280, uint8_t num, bmp280_calib_reg_t* reg)
{
    assert(bmp280 && reg);

    uint8_t data[2] = {};

    bmp280_err_t err =
        bmp280_bus_read(bmp280, BMP280_REG_ADDRESS_CALIB00 + 2 * num, data, sizeof(data));

    reg->calibration_data = (uint16_t)(((data[1] & 0xFFU) << 8U) | (data[0] & 0xFFU));

    return err;
}

bmp280_err_t bmp280_set_calib_reg(bmp280_t const* bmp280,
                                  uint8_t num,
                                  bmp280_calib_reg_t const* reg)
{
    assert(bmp280 && reg);

    uint8_t data[2] = {};

    data[0] = (uint8_t)((reg->calibration_data >> 8U) & 0xFFU);
    data[1] = (uint8_t)((reg->calibration_data & 0xFFU));

    return bmp280_bus_write(bmp280, BMP280_REG_ADDRESS_CALIB00 + num * 2, data, sizeof(data));
}

bmp280_err_t bmp280_get_reset_reg(bmp280_t const* bmp280, bmp280_reset_reg_t* reg)
{
    assert(bmp280 && reg);

    uint8_t data = {};

    bmp280_err_t err = bmp280_bus_read(bmp280, BMP280_REG_ADDRESS_RESET, &data, sizeof(data));

    reg->reset = data & 0xFFU;

    return err;
}

bmp280_err_t bmp280_set_reset_reg(bmp280_t const* bmp280, bmp280_reset_reg_t const* reg)
{
    assert(bmp280 && reg);

    uint8_t data = {};

    data = reg->reset & 0xFFU;

    return bmp280_bus_write(bmp280, BMP280_REG_ADDRESS_RESET, &data, sizeof(data));
}

bmp280_err_t bmp280_get_config_reg(bmp280_t const* bmp280, bmp280_config_reg_t* reg)
{
    assert(bmp280 && reg);

    uint8_t data = {};

    bmp280_err_t err = bmp280_bus_read(bmp280, BMP280_REG_ADDRESS_CONFIG, &data, sizeof(data));

    reg->t_sb = (data >> 5U) & 0x07U;
    reg->filter = (data >> 2U) & 0x07U;
    reg->spi3w_en = data & 0x01U;

    return err;
}

bmp280_err_t bmp280_set_config_reg(bmp280_t const* bmp280, bmp280_config_reg_t const* reg)
{
    assert(bmp280 && reg);

    uint8_t data = {};

    bmp280_err_t err = bmp280_bus_read(bmp280, BMP280_REG_ADDRESS_CONFIG, &data, sizeof(data));

    data &= ~((0x07U << 5U) | (0x07U << 2U) | 0x01);

    data |= (reg->t_sb & 0x07U) << 5U;
    data |= (reg->filter & 0x07U) << 2U;
    data |= reg->spi3w_en & 0x01U;

    err |= bmp280_bus_write(bmp280, BMP280_REG_ADDRESS_CONFIG, &data, sizeof(data));

    return err;
}

bmp280_err_t bmp280_get_ctrl_meas_reg(bmp280_t const* bmp280, bmp280_ctrl_meas_reg_t* reg)
{
    assert(bmp280 && reg);

    uint8_t data = {};

    bmp280_err_t err = bmp280_bus_read(bmp280, BMP280_REG_ADDRESS_CTRL_MEAS, &data, sizeof(data));

    reg->osrs_t = (data >> 5U) & 0x03U;
    reg->osrs_p = (data >> 2U) & 0x03U;
    reg->mode = data & 0x03U;

    return err;
}

bmp280_err_t bmp280_set_ctrl_meas_reg(bmp280_t const* bmp280, bmp280_ctrl_meas_reg_t const* reg)
{
    assert(bmp280 && reg);

    uint8_t data = {};

    bmp280_err_t err = bmp280_bus_read(bmp280, BMP280_REG_ADDRESS_CTRL_MEAS, &data, sizeof(data));

    data &= ~((0x07U << 5U) | (0x07U << 2U) | 0x03U);

    data |= (reg->osrs_t & 0x07U) << 5U;
    data |= (reg->osrs_p & 0x07U) << 2U;
    data |= reg->mode & 0x03U;

    err |= bmp280_bus_write(bmp280, BMP280_REG_ADDRESS_CTRL_MEAS, &data, sizeof(data));

    return err;
}

bmp280_err_t bmp280_get_id_reg(bmp280_t const* bmp280, bmp280_id_reg_t* reg)
{
    assert(bmp280 && reg);

    uint8_t data = {};

    bmp280_err_t err = bmp280_bus_read(bmp280, BMP280_REG_ADDRESS_ID, &data, sizeof(data));

    reg->chip_id = data & 0xFFU;

    return err;
}

bmp280_err_t bmp280_get_status_reg(bmp280_t const* bmp280, bmp280_status_reg_t* reg)
{
    assert(bmp280 && reg);

    uint8_t data = {};

    bmp280_err_t err = bmp280_bus_read(bmp280, BMP280_REG_ADDRESS_STATUS, &data, sizeof(data));

    reg->measuring = (data >> 3U) & 0x01U;
    reg->im_update = data & 0x01U;

    return err;
}

bmp280_err_t bmp280_get_press_reg(bmp280_t const* bmp280, bmp280_press_reg_t* reg)
{
    assert(bmp280 && reg);

    uint8_t data[2] = {};

    bmp280_err_t err = bmp280_bus_read(bmp280, BMP280_REG_ADDRESS_PRESS_MSB, data, sizeof(data));

    reg->press = (int16_t)(((data[0] & 0xFF) << 8) | (data[1] & 0xFF));

    return err;
}

bmp280_err_t bmp280_get_press_x_reg(bmp280_t const* bmp280, bmp280_press_x_reg_t* reg)
{
    assert(bmp280 && reg);

    uint8_t data = {};

    bmp280_err_t err = bmp280_bus_read(bmp280, BMP280_REG_ADDRESS_PRESS_XLSB, &data, sizeof(data));

    reg->press_x = (data >> 4U) & 0x0FU;

    return err;
}

bmp280_err_t bmp280_get_temp_reg(bmp280_t const* bmp280, bmp280_temp_reg_t* reg)
{
    assert(bmp280 && reg);

    uint8_t data[2] = {};

    bmp280_err_t err = bmp280_bus_read(bmp280, BMP280_REG_ADDRESS_TEMP_MSB, data, sizeof(data));

    reg->temp = (int16_t)(((data[0] & 0xFF) << 8) | (data[1] & 0xFF));

    return err;
}

bmp280_err_t bmp280_get_temp_x_reg(bmp280_t const* bmp280, bmp280_temp_x_reg_t* reg)
{
    assert(bmp280 && reg);

    uint8_t data = {};

    bmp280_err_t err = bmp280_bus_read(bmp280, BMP280_REG_ADDRESS_TEMP_XLSB, &data, sizeof(data));

    reg->temp_x = (data >> 4U) & 0x0FU;

    return err;
}
