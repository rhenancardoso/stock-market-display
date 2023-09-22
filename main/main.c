#include <stdio.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "utils/weather.h"
#include "utils/stock_api.h"
#include "utils/wifi.h"
#include "display_screen/home_screen.h"
#include "display_screen/stock_screen.h"
#include "display_screen/wifi_conn_scr.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

#define BTN_DIV 5
#define BRIGHTNESS_8BITS_5DIV (uint8_t)(255 / BTN_DIV)
#define BTN_BRIGHT 14
#define BTN_PRESSED 0

static const char *TAG = "main.c";

extern ledc_channel_config_t lcd_bright;
uint8_t lcd_bright_btn = 2;

long int next_time_weather_screen;
long int next_time_stock_screen;

bool first_request_complete = false;
extern void initialise_lcd(lv_disp_t *disp);
extern void set_time(void);
static void displayTask(void);
static void extConnTask(void);
void read_bright_btn(void);
void set_display_brigthness(void);

void app_main()
{
    xTaskCreatePinnedToCore(displayTask, "lvglDisplay", 50000, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(extConnTask, "extConnection", 20000, NULL, 1, NULL, 0);
}

static void displayTask(void)
{
    static const char *TAG = "displayTask";
    // - - INITIALISATION - - - - -
    // Display initialisation
    lv_disp_t *disp;
    initialise_lcd(&disp);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (lcd_bright_btn * BRIGHTNESS_8BITS_5DIV));
    // - - - ACTION - - - - - - - - - - - - - - - - - - - - - - /
    ESP_LOGI(TAG, "Display main UI");
    ESP_LOGI(TAG, "While loop");
    main_screen_ui();
    stock_screen_ui();
    wifi_conn_screen_ui();
    lv_scr_load(wifi_conn_page);

    long int time_now = 0;
    while (1)
    {
        set_display_brigthness();
        if (first_request_complete)
        {
            if (time_now == 0 || time_now >= next_time_stock_screen)
            {
                ESP_LOGI(TAG, "Setting Home Page");
                lv_scr_load(home_page);
                next_time_weather_screen = clock() + WEATHER_SCREEN_MS;
                next_time_stock_screen = clock() + WEATHER_SCREEN_MS + STOCK_SCREEN_MS;
            }
            if (time_now >= next_time_weather_screen)
            {
                ESP_LOGI(TAG, "Setting Stock Page");
                lv_scr_load(stock_page);
                next_time_stock_screen = clock() + STOCK_SCREEN_MS;
                next_time_weather_screen = clock() + WEATHER_SCREEN_MS + STOCK_SCREEN_MS;
            }
            time_now = clock();
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
    long int last_time_heap_size = clock();
    long int last_time_stock = clock();
    long int time_now;
    weeklyForecast[0].is_data_collected = false; // this is used ot check if the weekly data has been retrieved for the first time, before changing its timer period.
    gpio_set_direction(BTN_BRIGHT, GPIO_MODE_INPUT);
    while (1)
    {
        time_now = clock();
        if (wifi_conn.is_connected)
        {
            if (first_request_complete)
            {
                if ((time_now - last_time_daily) >= TODAYS_UPDATE_MS)
                {
                    // Daily forecast API call to daily forecast object
                    ESP_LOGI(TAG, "Todays tick count = %ld", (time_now - last_time_daily));
                    getTodaysForecast();
                    last_time_daily = clock();
                }
                if ((time_now - last_time_weekly) >= WEEKLY_UPDATE_MS)
                {
                    // Weekly forecast API call to weekly forecast object
                    ESP_LOGI(TAG, "Weekly tick count = %ld", (time_now - last_time_weekly));
                    getWeeklyForecast();
                    last_time_weekly = clock();
                }
                if ((time_now - last_time_stock) >= STOCK_UPDATE_MS)
                {
                    // Stock spreadhsheet API call to update stock object
                    ESP_LOGI(TAG, "Stock tick count = %ld", (time_now - last_time_stock));
                    getStockData();
                    last_time_stock = clock();
                }
            }
            else
            {
                ESP_LOGI(TAG, "Setting next weather time via 'clock()'");
                getStockData();
                vTaskDelay(250 / portTICK_PERIOD_MS);

                ESP_LOGI(TAG, "Performing first request for Todays Forecast");
                getTodaysForecast();
                vTaskDelay(250 / portTICK_PERIOD_MS);

                ESP_LOGI(TAG, "Performing first request for Weekly Forecast");
                getWeeklyForecast();

                first_request_complete = true;

                next_time_weather_screen = clock();
            }
        }
        if (gpio_get_level(BTN_BRIGHT) == BTN_PRESSED)
        {
            read_bright_btn();
        }
        if ((time_now - last_time_heap_size) > 2000)
        {
            ESP_LOGI(TAG, "Free Heap: %u bytes", xPortGetFreeHeapSize());
            last_time_heap_size = clock();
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Read when the BRIGHT button (KEY) is pressed and update lcd_bright_btn variable.
 */
void read_bright_btn(void)
{
    while (gpio_get_level(BTN_BRIGHT) == BTN_PRESSED)
    {
        // Filter btn noise, in case it is long pressed or flaky signal
    }
    lcd_bright_btn++;
    if (lcd_bright_btn > BTN_DIV)
        lcd_bright_btn = 1;
    ESP_LOGI(TAG, "Key button pressed. lcd_bright: %d", lcd_bright_btn);
    ESP_LOGI(TAG, "brightnnes 8-bit value: %d", lcd_bright_btn * BRIGHTNESS_8BITS_5DIV);
}

/**
 * @brief Set the display brigthness based on lcd_bright_btn variable.
 */
void set_display_brigthness(void)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (lcd_bright_btn * BRIGHTNESS_8BITS_5DIV));
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}