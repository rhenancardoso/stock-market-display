#include "driver/gpio.h"
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_oneshot.h"

// Battery
#define VOLTAGE_MAX_mV 2100
#define VOLTAGE_MIN_mV 1850
#define VOLTAGE_REF_mV 1100
#define ADC_SAMPLES 50
#define BATTERY_FULL 4200
#define BATTERY_3 4000
#define BATTERY_2 3900
#define BATTERY_1 3700
#define BATTERY_EMPTY 3500

// Battery calc -> voltage divider
// Vout = ([3.70V - 4.20V]] * 100K) / (100K + 100K)
// Vout = 1.85V - 2.10V
// Battery ADC reading GPIO4 -> ADC2_CH0 -> 'ADC2_GPIO4_CHANNEL'

extern uint16_t battery_pct;
extern adc_oneshot_unit_handle_t adc2_handle;
extern adc_cali_handle_t adc_cali_handle;

/**
 * @brief Read ADC value for battery level and set `battery_pct`.
 */
extern void battery_reading(void);