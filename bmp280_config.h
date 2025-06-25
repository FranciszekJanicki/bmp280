#ifndef BMP280_BMP280_CONFIG_H
#define BMP280_BMP280_CONFIG_H

#include <stddef.h>
#include <stdint.h>

#define BMP280_MEASURING (1 << 3)
#define BMP280_DEVICE_ID 0x75U

typedef float float32_t;

typedef enum {
  BMP280_ERR_OK = 0,
  BMP280_ERR_FAIL = 1 << 0,
  BMP280_ERR_NULL = 1 << 1,
} bmp280_err_t;

typedef enum {
  BMP280_REG_ADDRESS_CALIB00 = 0x88,
  BMP280_REG_ADDRESS_ID = 0xD0,
  BMP280_REG_ADDRESS_RESET = 0xE0,
  BMP280_REG_ADDRESS_STATUS = 0xF3,
  BMP280_REG_ADDRESS_CTRL_MEAS = 0xF4,
  BMP280_REG_ADDRESS_CONFIG = 0xF5,
  BMP280_REG_ADDRESS_PRESS_MSB = 0xF7,
  BMP280_REG_ADDRESS_PRESS_LSB = 0xF8,
  BMP280_REG_ADDRESS_PRESS_XLSB = 0xF9,
  BMP280_REG_ADDRESS_TEMP_MSB = 0xFA,
  BMP280_REG_ADDRESS_TEMP_LSB = 0xFB,
  BMP280_REG_ADDRESS_TEMP_XLSB = 0xFC,
} bmp280_reg_address_t;

typedef enum {
  BMP280_RESOLUTION_ULTRALOWPOWER = 1,
  BMP280_RESOLUTION_LOWPOWER = 2,
  BMP280_RESOLUTION_STANDARD = 3,
  BMP280_RESOLUTION_HIGHRES = 4,
  BMP280_RESOLUTION_ULTRAHIGHRES = 5,
} bmp280_resolution_t;

typedef enum {
  BMP280_TEMPERATURE_16BIT = 1,
  BMP280_TEMPERATURE_17BIT = 2,
  BMP280_TEMPERATURE_18BIT = 3,
  BMP280_TEMPERATURE_19BIT = 4,
  BMP280_TEMPERATURE_20BIT = 5,
} bmp280_temperature_t;

typedef enum {
  BMP280_MODE_SLEEP = 0,
  BMP280_MODE_FORCED = 1,
  BMP280_MODE_NORMAL = 3,
} bmp280_mode_t;

typedef enum {
  BMP280_STANDBY_MS_0_5 = 0,
  BMP280_STANDBY_MS_10 = 6,
  BMP280_STANDBY_MS_20 = 7,
  BMP280_STANDBY_MS_62_5 = 1,
  BMP280_STANDBY_MS_125 = 2,
  BMP280_STANDBY_MS_250 = 3,
  BMP280_STANDBY_MS_500 = 4,
  BMP280_STANDBY_MS_1000 = 5,
} bmp280_standby_t;

typedef enum {
  BMP280_FILTER_OFF = 0,
  BMP280_FILTER_X2 = 1,
  BMP280_FILTER_X4 = 2,
  BMP280_FILTER_X8 = 3,
  BMP280_FILTER_X16 = 4,
} bmp280_filter_t;

typedef enum {
  BMP280_DIGIT_T1 = 0x88,
  BMP280_DIGIT_T2 = 0x8A,
  BMP280_DIGIT_T3 = 0x8C,
  BMP280_DIGIT_P1 = 0x8E,
  BMP280_DIGIT_P2 = 0x90,
  BMP280_DIGIT_P3 = 0x92,
  BMP280_DIGIT_P4 = 0x94,
  BMP280_DIGIT_P5 = 0x96,
  BMP280_DIGIT_P6 = 0x98,
  BMP280_DIGIT_P7 = 0x9A,
  BMP280_DIGIT_P8 = 0x9C,
  BMP280_DIGIT_P9 = 0x9E,
} bmp280_digit_t;

typedef struct {
  uint16_t t1;
  int16_t t2;
  int16_t t3;
  uint16_t p1;
  int16_t p2;
  int16_t p3;
  int16_t p4;
  int16_t p5;
  int16_t p6;
  int16_t p7;
  int16_t p8;
  int16_t p9;
  uint32_t t_fine;
} bmp280_trim_data_t;

typedef struct {
  float32_t scale;
} bmp280_config_t;

typedef struct {
  void *bus_user;
  bmp280_err_t (*bus_initialize)(void *);
  bmp280_err_t (*bus_deinitialize)(void *);
  bmp280_err_t (*bus_write_data)(void *, uint8_t, uint8_t const *, size_t);
  bmp280_err_t (*bus_read_data)(void *, uint8_t, uint8_t *, size_t);
} bmp280_interface_t;

#endif // BMP280_BMP280_CONFIG_H