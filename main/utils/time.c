#include "time.h"
#include "wifi.h"

static const char *TAG = "set-time";

char current_time[100];

void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}

void GetSntpTime(char *date_time)
{
    ESP_LOGI(TAG, "Get SNTP time");
    char strftime_buf[64];
    char today[3];
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    // Set timezone
    setenv("TZ", CITY_UTC_TZ, 1);
    tzset();
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    strftime(today, sizeof(today), "%a", &timeinfo);
    ESP_LOGI(TAG, "The current date/time in %s is: %s", CITY_TZ, strftime_buf);
    strcpy(date_time, strftime_buf);
}

static void InitialiseSNTP(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_init();
}

static void GetSystemTime(void)
{
    InitialiseSNTP();
    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count)
    {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    time(&now);
    localtime_r(&now, &timeinfo);
}

void SetTime(void)
{
    ESP_LOGI(TAG, "Set time from SNTP server");
    GetSystemTime();
    GetSntpTime(current_time);
}
