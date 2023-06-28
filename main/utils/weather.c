#include "weather.h"

static const char *TAG = "weather";

extern uint8_t convertDaytoInt(char *week_day);
extern cJSON *http_get_JSON_request(char *url);
cJSON *weather_http_response;
struct Weather w_melb;
struct WeeklyForecast weeklyForecast[DAYS_FORECAST];
char weather_request[131];
char weekly_weather_request[123];

void pullTodaysForecastJSON(cJSON *root)
{
    cJSON *elem;
    cJSON *name;

    ESP_LOGI(TAG, "pull 'today' weather JSON");
    cJSON *obj_main = cJSON_GetObjectItemCaseSensitive(root, "main");
    w_melb.temp = cJSON_GetObjectItemCaseSensitive(obj_main, "temp")->valuedouble;
    w_melb.min_temp = cJSON_GetObjectItemCaseSensitive(obj_main, "temp_min")->valuedouble;
    w_melb.max_temp = cJSON_GetObjectItemCaseSensitive(obj_main, "temp_max")->valuedouble;
    w_melb.humidity = cJSON_GetObjectItemCaseSensitive(obj_main, "humidity")->valueint;
    cJSON *obj_clouds = cJSON_GetObjectItemCaseSensitive(root, "clouds");
    cJSON *obj_weather = cJSON_GetObjectItemCaseSensitive(root, "weather");
    uint8_t n = cJSON_GetArraySize(obj_weather);
    for (uint8_t i = 0; i < n; i++)
    {
        elem = cJSON_GetArrayItem(obj_weather, i);
        name = cJSON_GetObjectItem(elem, "icon");
        sprintf(w_melb.icon, "%s", name->valuestring);
        if (strcmp(w_melb.icon, "03d") == 0 || strcmp(w_melb.icon, "04d") == 0 || strcmp(w_melb.icon, "09d") == 0 || strcmp(w_melb.icon, "11d") == 0)
        {
            w_melb.icon[2] = 'n';
        }
    }
    ESP_LOGD(TAG, "Todays forecast JSON -> temp=%.2f; min_t=%.2f; max_t=%.2f; icon=%s", w_melb.temp, w_melb.min_temp, w_melb.max_temp, w_melb.icon);
    w_melb.is_data_collected = true;
    cJSON_Delete(root);
}

void pullWeeklyForecastFromJSON(cJSON *root)
{
    ESP_LOGD(TAG, "pull weekly forecast JSON");
    cJSON *data = cJSON_GetObjectItemCaseSensitive(root, "data");
    uint8_t n = cJSON_GetArraySize(data);
    for (uint8_t i = 0; i < n; i++)
    {
        cJSON *elem;
        elem = cJSON_GetArrayItem(data, i);
        weeklyForecast[i].min_temp = cJSON_GetObjectItemCaseSensitive(elem, "temp_min")->valuedouble;
        weeklyForecast[i].max_temp = cJSON_GetObjectItemCaseSensitive(elem, "temp_max")->valuedouble;
        cJSON *weather = cJSON_GetObjectItemCaseSensitive(elem, "weather");
        weeklyForecast[i].icon_code = cJSON_GetObjectItemCaseSensitive(weather, "code")->valueint;
        ESP_LOGD(TAG, "weekly forecast JSON -> ind=%d; min_t=%.0f; max_t=%.0f, ; icon=%d", i, weeklyForecast[i].min_temp, weeklyForecast[i].max_temp, weeklyForecast[i].icon_code);
    }

    ESP_LOGD(TAG, "weekly forecast JSON extracted");
    cJSON_Delete(root);
}

void getTodaysForecast(void)
{
    ESP_LOGI(TAG, "Get Today's weather forecast");
    setWeeklyDaysStruct();
    sprintf(weather_request, API_URL, CITY_LAT, CITY_LON, API_KEY);
    weather_http_response = http_get_JSON_request(weather_request);
    if (weather_http_response != NULL)
    {
        ESP_LOGI(TAG, "weather response ready");
        pullTodaysForecastJSON(weather_http_response);
    }
    else
    {
        w_melb.is_data_collected = false;
        ESP_LOGE(TAG, "null weather API response");
    }
}

void getWeeklyForecast(void)
{
    ESP_LOGI(TAG, "Get weekly's weather forecast");
    sprintf(weekly_weather_request, BIT_API_URL, CITY, COUNTRY, BIT_API_KEY, DAYS_FORECAST);
    weather_http_response = http_get_JSON_request(weekly_weather_request);
    if (weather_http_response != NULL)
    {
        ESP_LOGI(TAG, "weekly forecast response ready");
        pullWeeklyForecastFromJSON(weather_http_response);
    }
    else
    {
        ESP_LOGE(TAG, "null weekly forecast API response");
    }
}

void setWeeklyDaysStruct(void)
{
    ESP_LOGI(TAG, "Get Today's day of the week");
    char week_day[4];
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(week_day, sizeof(week_day), "%a", &timeinfo);
    ESP_LOGI(TAG, "Getting Today's day of the week. System week day: %s", week_day);
    uint8_t today = convertDaytoInt(week_day);
    ESP_LOGD(TAG, "System week day integer: %d", today);
    weeklyForecast[0].week_day = today;
    for (uint8_t ind = 1; ind < DAYS_FORECAST; ind++)
    {
        weeklyForecast[ind].week_day = (today + ind) > 7 ? (today + ind - 7) : (today + ind);
    }
}

void setWeatherIconImg(char icon[4], lv_obj_t *icon_img)
{
    ESP_LOGD(TAG, "Set weather icon image. icon: %s", icon);
    if (strcmp(icon, "01d") == 0)
    {
        lv_img_set_src(icon_img, &w01d);
    }
    else if (strcmp(icon, "01n") == 0)
    {
        lv_img_set_src(icon_img, &w01n);
    }
    else if (strcmp(icon, "02d") == 0)
    {
        lv_img_set_src(icon_img, &w02d);
    }
    else if (strcmp(icon, "02n") == 0)
    {
        lv_img_set_src(icon_img, &w02n);
    }
    else if (strcmp(icon, "03n") == 0)
    {
        lv_img_set_src(icon_img, &w03n);
    }
    else if (strcmp(icon, "04n") == 0)
    {
        lv_img_set_src(icon_img, &w04n);
    }
    else if (strcmp(icon, "09n") == 0)
    {
        lv_img_set_src(icon_img, &w09n);
    }
    else if (strcmp(icon, "10d") == 0)
    {
        lv_img_set_src(icon_img, &w10d);
    }
    else if (strcmp(icon, "10n") == 0)
    {
        lv_img_set_src(icon_img, &w10n);
    }
    else if (strcmp(icon, "11n") == 0)
    {
        lv_img_set_src(&icon_img, &w11n);
    }
    else
    {
        ESP_LOGW(TAG, "Icon %s not in the icon's list.", icon);
    }
}

uint32_t getWeatherIconWidth(char icon[4])
{
    uint32_t icon_width = 0;
    if (strcmp(icon, "01d") == 0)
    {
        icon_width = w01d.header.w;
    }
    else if (strcmp(icon, "01n") == 0)
    {
        icon_width = w01n.header.w;
    }
    else if (strcmp(icon, "02d") == 0)
    {
        icon_width = w02d.header.w;
    }
    else if (strcmp(icon, "02n") == 0)
    {
        icon_width = w02n.header.w;
    }
    else if (strcmp(icon, "03n") == 0)
    {
        icon_width = w03n.header.w;
    }
    else if (strcmp(icon, "04n") == 0)
    {
        icon_width = w04n.header.w;
    }
    else if (strcmp(icon, "09n") == 0)
    {
        icon_width = w09n.header.w;
    }
    else if (strcmp(icon, "10d") == 0)
    {
        icon_width = w10d.header.w;
    }
    else if (strcmp(icon, "10n") == 0)
    {
        icon_width = w10n.header.w;
    }
    else if (strcmp(icon, "11n") == 0)
    {
        icon_width = w11n.header.w;
    }
    return icon_width;
}

void setWeeklyForecastIconImg(int icon_code, lv_obj_t *icon_img)
{
    if (icon_code == 800)
    {
        lv_img_set_src(icon_img, &w01d);
    }
    else if ((icon_code >= 801 && icon_code <= 802) || (icon_code >= 700 && icon_code <= 751))
    {
        lv_img_set_src(icon_img, &w02d);
    }
    else if (icon_code == 803)
    {
        lv_img_set_src(icon_img, &w03n);
    }
    else if (icon_code == 804)
    {
        lv_img_set_src(icon_img, &w04n);
    }
    else if (icon_code == 502 || icon_code == 522)
    {
        lv_img_set_src(icon_img, &w09n);
    }
    else if (icon_code == 500 || icon_code == 501 || icon_code == 520 || icon_code == 521 || icon_code == 900)
    {
        lv_img_set_src(icon_img, &w10d);
    }
    else if ((icon_code >= 200 && icon_code <= 233))
    {
        lv_img_set_src(icon_img, &w11n);
    }
}

void FiveDaysMockData(void)
{
    ESP_LOGD(TAG, "Create mock data for 5Days forecast.");

    // case (1):
    weeklyForecast[1].min_temp = 9.2;
    weeklyForecast[1].max_temp = 27.7;
    weeklyForecast[1].icon_code = 741;
    // case (2):
    weeklyForecast[2].min_temp = 7.5;
    weeklyForecast[2].max_temp = 12.9;
    weeklyForecast[2].icon_code = 520;
    // case (3):
    weeklyForecast[3].min_temp = 14;
    weeklyForecast[3].max_temp = 22;
    weeklyForecast[3].icon_code = 202;
    // case (4):
    weeklyForecast[4].min_temp = 17;
    weeklyForecast[4].max_temp = 32;
    weeklyForecast[4].icon_code = 803;
    // case (5):
    weeklyForecast[5].min_temp = 2;
    weeklyForecast[5].max_temp = 9;
    weeklyForecast[5].icon_code = 804;

    for (uint8_t i = 0; i < DAYS_FORECAST; i++)
    {
        ESP_LOGI(TAG, "Data index %d-> min_temp: %.2f, max_temp: %.2f, icon_code: %d",
                 i,
                 weeklyForecast[i].min_temp,
                 weeklyForecast[i].max_temp,
                 weeklyForecast[i].icon_code);
    }
}