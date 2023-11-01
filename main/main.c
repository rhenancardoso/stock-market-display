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
#include "utils/battery.h"
#include "display_screen/home_screen.h"
#include "display_screen/stock_screen.h"
#include "display_screen/wifi_conn_scr.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_oneshot.h"

#define BTN_DIV 5
#define BRIGHTNESS_8BITS_5DIV (uint8_t)(255 / BTN_DIV)
#define BTN_BRIGHT 14
#define BTN_KEY 0
#define PWR_ENABLE_IO 15
#define GPIO_HIGH 1
#define BTN_PRESSED 0
#define LONG_PRESS_MS 500

static const char *TAG = "main.c";

extern ledc_channel_config_t lcd_bright;
uint8_t lcd_bright_btn = 2;
adc_oneshot_unit_handle_t adc2_handle;
adc_cali_handle_t adc_cali_handle;

bool stock_screen_change = 0;

long int time_weather_screen;
long int time_stock_screen;

bool first_request_complete = false;
extern void InitialiseLCD(lv_disp_t *disp);
extern void SetTime(void);
static void displayTask(void);
static void mainAppTask(void);
void ReadBrightBtn(void);
void SetDisplayBrightness(void);
void ChangeStockScreen(void);
void ToggleStockWeatherScreen(void);

void app_main()
{
    ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_2,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config, &adc_cali_handle));

    // ADC initialisation
    adc_oneshot_unit_init_cfg_t init_config2 = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config2, &adc2_handle));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, ADC_CHANNEL_0, &config));

    // Enable system to be battery powered
    gpio_set_direction(PWR_ENABLE_IO, GPIO_MODE_OUTPUT);
    gpio_set_level(PWR_ENABLE_IO, GPIO_HIGH);
    // Configure brigthness button
    gpio_set_direction(BTN_BRIGHT, GPIO_MODE_INPUT);
    // Configure key button
    gpio_set_direction(BTN_KEY, GPIO_MODE_INPUT);

    xTaskCreatePinnedToCore(displayTask, "lvglDisplay", 20000, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(mainAppTask, "StockMarket", 50000, NULL, 1, NULL, 0);
}

static void mainAppTask(void)
{
    // Display brightness init
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (lcd_bright_btn * BRIGHTNESS_8BITS_5DIV));

    // - - INITIALISATION - - - - -
    ESP_LOGI(TAG, "Initialising WIFI");
    initialize_wifi();
    SetTime();
    long int last_time_weekly = clock();
    long int last_time_daily = clock();
    long int last_time_heap_size = clock();
    long int last_time_stock = clock();
    long int time_now;
    weeklyForecast[0].is_data_collected = false; // this is used ot check if the weekly data has been retrieved for the first time, before changing its timer period.

    ESP_LOGI(TAG, "Getting into main loop");
    while (1)
    {
        // The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`

        SetDisplayBrightness();
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

                if (time_now == 0 || time_now >= time_weather_screen)
                {
                    if (time_stock_screen < clock())
                    {
                        time_stock_screen = clock() + WEATHER_SCREEN_MS;
                        ESP_LOGI(TAG, "Setting Home Page");
                    }
                    lv_scr_load(home_page);
                }

                if (time_now >= time_stock_screen)
                {
                    if (time_weather_screen < clock())
                    {
                        time_weather_screen = clock() + STOCK_SCREEN_MS;
                        ESP_LOGI(TAG, "Setting Stock Page");
                    }
                    lv_scr_load(stock_page);
                }
                time_now = clock();
            }
            else
            {
                ESP_LOGI(TAG, "Setting next weather time via 'clock()'");
                getStockData();
                vTaskDelay(20 / portTICK_PERIOD_MS);

                ESP_LOGI(TAG, "Performing first request for Todays Forecast");
                getTodaysForecast();
                vTaskDelay(20 / portTICK_PERIOD_MS);

                ESP_LOGI(TAG, "Performing first request for Weekly Forecast");
                getWeeklyForecast();

                first_request_complete = true;
            }
        }
        if (gpio_get_level(BTN_BRIGHT) == BTN_PRESSED)
        {
            ReadBrightBtn();
        }
        if (gpio_get_level(BTN_KEY) == BTN_PRESSED)
        {
            ChangeStockScreen();
        }
        if ((time_now - last_time_heap_size) > 1000)
        {
            // ESP_LOGI(TAG, "Free Heap: %u bytes", xPortGetFreeHeapSize());
            last_time_heap_size = clock();
            ReadBattery();
        }
    }
}

static void displayTask(void)
{
    static const char *TAG = "displayTask";
    // - - INITIALISATION - - - - -
    // Display initialisation
    lv_disp_t *disp;
    InitialiseLCD(&disp);

    // - - - ACTION - - - - - - - - - - - - - - - - - - - - - - /
    ESP_LOGI(TAG, "Initialise displayTask");
    wifi_conn_screen_ui();
    vTaskDelay(250 / portTICK_PERIOD_MS);
    main_screen_ui();
    vTaskDelay(250 / portTICK_PERIOD_MS);
    stock_screen_ui();
    lv_scr_load(wifi_conn_page);

    while (1)
    {
        // The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
        lv_tick_inc(25);
        lv_timer_handler();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Read when the BRIGHT button (KEY) is pressed and update lcd_bright_btn variable.
 */
void ReadBrightBtn(void)
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
void SetDisplayBrightness(void)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (lcd_bright_btn * BRIGHTNESS_8BITS_5DIV));
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

/**
 * @brief Read when the KEY button (GPIO0) is pressed and update lcd_bright_btn variable.
 */
void ChangeStockScreen(void)
{
    uint16_t time_pressed = clock();
    while (gpio_get_level(BTN_KEY) == BTN_PRESSED)
    {
        // Filter btn noise, in case it is long pressed or flaky signal
    }

    uint16_t time_held_pressed = clock() - time_pressed;
    ESP_LOGI(TAG, "Time held pressed: %d", time_held_pressed);
    if (time_held_pressed < LONG_PRESS_MS)
    {
        stock_screen_change = true;
        ESP_LOGI(TAG, "GPIO0 button pressed");
    }
    else
    {
        ESP_LOGI(TAG, "Long press");
        ToggleStockWeatherScreen();
    }
}

/**
 * @brief Check the current page and toggle to the other one
 */
void ToggleStockWeatherScreen(void)
{
    if (clock() >= time_weather_screen)
    {
        // Current screen is the weather screen
        time_stock_screen = clock();
        time_weather_screen = clock() + STOCK_SCREEN_MS;
        ESP_LOGI(TAG, "Current screen: weather");
    }
    else
    {
        // Current screen is the stock screen
        time_weather_screen = clock();
        time_stock_screen = clock() + WEATHER_SCREEN_MS;
        ESP_LOGI(TAG, "Current screen: stock");
    }
    ESP_LOGI(TAG, "=>time_now: %ld, =>time_weather: %ld =>time_stock: %ld", clock(), time_weather_screen, time_stock_screen);
}