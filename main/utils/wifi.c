// WIFI Settings taken from
// https://github.com/vinothkannan369/ESP32/blob/main/SNTP/set_clk.c

#include "wifi.h"

Wifi wifi_conn;

static const char *TAG = "wifi-sta";
static EventGroupHandle_t s_wifi_event_group;
uint8_t retries;
void wifi_init_sta(void);
void initialize_wifi(void);

wifi_config_t wifi_config;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "wifi event handler, event_id: %ld", event_id);
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(TAG, "Connecting to Wifi");
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (retries > MAX_RETRY)
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            ESP_LOGI(TAG, "Fail to connect to Wifi");
            wifi_conn.is_connected = false;
        }
        else
        {
            esp_wifi_connect();
            ESP_LOGI(TAG, "retry to connect to Wifi");
            retries++;
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        wifi_conn.is_connected = true;
    }
}

void initialize_wifi(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "START Wifi Scanning...");
    uint16_t number = MAX_NUMBER_AP;
    wifi_ap_record_t ap_info[MAX_NUMBER_AP];
    uint16_t ap_count = 0;
    memset(ap_info, 0, sizeof(ap_info));

    ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true)); // The true parameter cause the function to block until
                                                      // the scan is done.
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);
    for (int i = 0; (i < MAX_NUMBER_AP) && (i < ap_count); i++)
    {
        ESP_LOGI(TAG, "SSID: %15s | RSSI: %5d", (char *)ap_info[i].ssid, ap_info[i].rssi);

        if (strcmp(WORK_WIFI_SSID, (char *)ap_info[i].ssid) == 0)
        {
            memcpy(wifi_config.sta.ssid, WORK_WIFI_SSID, sizeof(WORK_WIFI_SSID) / sizeof(uint8_t));
            memcpy(wifi_config.sta.password, WORK_WIFI_PASSWORD, sizeof(WORK_WIFI_PASSWORD) / sizeof(uint8_t));
        }
        else if (strcmp(HOME_WIFI_SSID, (char *)ap_info[i].ssid) == 0)
        {
            memcpy(wifi_config.sta.ssid, HOME_WIFI_SSID, sizeof(HOME_WIFI_SSID) / sizeof(uint8_t));
            memcpy(wifi_config.sta.password, HOME_WIFI_PASSWORD, sizeof(HOME_WIFI_PASSWORD) / sizeof(uint8_t));
        }
    };
    esp_wifi_stop();
    ESP_LOGI(TAG, "Define ESP_WIFI_SSID = %s", HOME_WIFI_SSID);
    connectWifi();
}

void connectWifi(void)
{
    s_wifi_event_group = xEventGroupCreate();

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        WIFI_EVENT_STA_DISCONNECTED,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    ESP_LOGI(TAG, "creating wifi config. ssid: %s", wifi_config.sta.ssid);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    /* Setting a password implies station will connect to all security modes including WEP/WPA.
     * However these modes are deprecated and not advisable to be used. Incase your Access point
     * doesn't support WPA2, these mode can be enabled by commenting below line */
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s",
                 wifi_config.sta.ssid);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s",
                 wifi_config.sta.ssid);
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);
}
