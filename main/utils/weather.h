#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"
#include "esp_log.h"
#include "../key_config.h"
#include "lvgl.h"
#include "time.h"
#include "../display_screen/display_screen.h"

#define API_URL "http://api.openweathermap.org/data/2.5/weather?lat=%s&lon=%s&appid=%s&units=metric"
#define BIT_API_URL "http://api.weatherbit.io/v2.0/forecast/daily?city=%s&country=%s&key=%s&days=%d"
#define CITY "Melbourne"
#define COUNTRY "Australia"
#define CITY_LAT "-37.8142176"
#define CITY_LON "144.9631608"
#define API_KEY WEATHER_API_KEY
#define BIT_API_KEY WEATHERBIT_API_KEY
#define DAYS_FORECAST 6 // Includes 'today'
#define WEATHER_UPDATE_MS 30 * 60 * 1000

LV_IMG_DECLARE(w01n);
LV_IMG_DECLARE(w01d);
LV_IMG_DECLARE(w02n);
LV_IMG_DECLARE(w02d);
LV_IMG_DECLARE(w03n);
LV_IMG_DECLARE(w04n);
LV_IMG_DECLARE(w09n);
LV_IMG_DECLARE(w10n);
LV_IMG_DECLARE(w10d);
LV_IMG_DECLARE(w11n);

/**
 * Get current weather forecast via HTTP rest to API.
 */
void getTodaysForecast(void);
/**
 * Get weekly weather forecast via HTTP rest to API.
 */
void getWeeklyForecast(void);
/**
 * Get the weather icon and set the icon_img object.
 * @param icon       icon string name to be searched
 * @param icon_img   pointer to the image object to be set
 */
void setWeatherIconImg(char icon[4], lv_obj_t *icon_img);
/**
 * Get the weather icon and set the icon_img object.
 * @param icon_code  icon code integer to be searched
 * @param icon_img   pointer to the image object to be set
 */
void setWeeklyForecastIconImg(int icon_code, lv_obj_t *icon_img);
/**
 * Get weather icon width.
 * @param icon      icon string to be searched
 * @return          (uint32_t) width of the given icon
 */
uint32_t getWeatherIconWidth(char icon[4]);
/**
 * Set WeeklyForecast struct days of the week integers.
 */
void setWeeklyDaysStruct(void);
/**
 * @brief Create Mock data for 5Days Forecast
 *
 */
void FiveDaysMockData(void);

typedef struct Weather
{
    float temp;
    uint8_t humidity;
    float min_temp;
    float max_temp;
    bool is_data_collected;
    char icon[4];
};

typedef struct WeeklyForecast
{
    uint8_t week_day;
    float min_temp;
    float max_temp;
    int icon_code;
};
