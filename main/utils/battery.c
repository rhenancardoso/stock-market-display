#include "battery.h"

static const char *TAG = "battery";

void ReadBattery(void)
{

    uint16_t read_raw;
    int voltage;

    ESP_ERROR_CHECK(adc_oneshot_read(adc2_handle, ADC_CHANNEL_0, &read_raw));
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc_cali_handle, read_raw, &voltage));

    battery_pct = 10 * read_raw;
    // ESP_LOGI(TAG, "Channel[%d] Cali Voltage: %d mV / raw_value: %d", ADC_CHANNEL_0, voltage, read_raw);
}