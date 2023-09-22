#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"
#include "esp_log.h"
#include "../key_config.h"
#include "lvgl.h"
#include "time.h"
// #include "utils.c"

#ifndef _WEATHER_H_
#define _WEATHER_H_

#define API_URL "http://api.openweathermap.org/data/2.5/weather?lat=%s&lon=%s&appid=%s&units=metric"
#define BIT_API_URL "http://api.weatherbit.io/v2.0/forecast/daily?city=%s&country=%s&key=%s&days=%d"
#define CITY "Melbourne"
#define COUNTRY "Australia"
#define CITY_LAT "-37.8142176"
#define CITY_LON "144.9631608"
#define API_KEY WEATHER_API_KEY
#define BIT_API_KEY WEATHERBIT_API_KEY
#define DAYS_FORECAST 6                     // Includes 'today'
#define WEEKLY_UPDATE_MS 2 * 60 * 60 * 1000 // 5 days update every 2 hours, as Weatherbit Api can handle 50 calls/day
#define TODAYS_UPDATE_MS 2 * 60 * 1000      // Today's update every 2 minutes as OpenWeatherApi can handle 1000 calls/day

#endif

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

typedef struct weather
{
    float temp;
    uint8_t humidity;
    float min_temp;
    float max_temp;
    bool is_data_collected;
    char icon[4];
} Weather;

typedef struct weeklyForecast
{
    uint8_t week_day;
    float min_temp;
    float max_temp;
    int icon_code;
    bool is_data_collected;
} WeeklyForecast;

extern Weather w_melb;
extern WeeklyForecast weeklyForecast[DAYS_FORECAST];