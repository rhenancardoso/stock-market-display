#include "esp_event.h"
#include "wifi_config.h"

#define ESP_WIFI_SSID      HOME_WIFI_SSID
#define ESP_WIFI_PASS      HOME_WIFI_PASSWORD
#define ESP_WIFI_CHANNEL   4
#define MAX_STA_CONN       1

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);
void wifi_init_sta(void);
void initialize_wifi(void);