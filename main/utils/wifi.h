#include "esp_event.h"
#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event_loop.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_system.h"
#include "../key_config.h"

#ifndef _WIFI_H_
#define _WIFI_H_
#define MAX_NUMBER_AP 20
#define ESP_WIFI_CHANNEL 4
#define MAX_STA_CONN 1
/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define MAX_RETRY 10
#endif

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data);
void wifi_init_sta(void);
void initialize_wifi(void);
void connectWifi(void);

typedef struct wifi
{
  bool is_connected;
} Wifi;

extern Wifi wifi_conn;