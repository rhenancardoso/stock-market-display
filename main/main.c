#include <stdio.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "utils/weather.h"
#include "utils/wifi.h"
#include "display_screen/home_screen.h"
#include "display_screen/wifi_conn_scr.h"

static const char *TAG = "main.c";

extern lv_obj_t *home_page;
extern lv_obj_t *wifi_conn_page;
extern struct Wifi wifi_conn;
extern struct WeeklyForecast weeklyForecast[DAYS_FORECAST];
bool first_request_complete = false;
extern void initialise_lcd(lv_disp_t *disp);
extern void set_time(void);
static void displayTask(void);
static void extConnTask(void);

void app_main()
{
    xTaskCreatePinnedToCore(displayTask, "lvglDisplay", 50000, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(extConnTask, "extConnection", 10000, NULL, 1, NULL, 0);
}

static void displayTask(void)
{
    static const char *TAG = "displayTask";
    // - - INITIALISATION - - - - -
    // Display initialisation
    lv_disp_t *disp;
    initialise_lcd(&disp);

    // - - - ACTION - - - - - - - - - - - - - - - - - - - - - - /
    ESP_LOGI(TAG, "Display main UI");
    ESP_LOGI(TAG, "While loop");
    main_screen_ui();
    wifi_conn_screen_ui();
    lv_scr_load(wifi_conn_page);
    while (1)
    {
        if (first_request_complete)
        {
            lv_scr_load(home_page);
        }
        // The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
        lv_tick_inc(5);
        lv_timer_handler();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void extConnTask(void)
{
    static const char *TAG = "extConnTask";
    // - - INITIALISATION - - - - -
    ESP_LOGI(TAG, "Initialising WIFI");
    initialize_wifi();
    set_time();
    long int last_time_weekly = clock();
    long int last_time_daily = clock();
    long int time_now;
    weeklyForecast[0].is_data_collected = false; // this is used ot check if the weekly data has been retrieved for the first time, before changing its timer period.

    while (1)
    {
        time_now = clock();
        if (wifi_conn.is_connected)
        {
            if (!first_request_complete)
            {
                getTodaysForecast();
                getWeeklyForecast();
                first_request_complete = true;
            }
            else
            {
                if ((time_now - last_time_daily) >= TODAYS_UPDATE_MS)
                {
                    ESP_LOGI(TAG, "Todays tick count = %ld", (time_now - last_time_daily));
                    getTodaysForecast();
                    last_time_daily = clock();
                }
                if ((time_now - last_time_weekly) >= WEEKLY_UPDATE_MS)
                {
                    ESP_LOGI(TAG, "Weekly tick count = %ld", (time_now - last_time_weekly));
                    getWeeklyForecast();
                    last_time_weekly = clock();
                }
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
