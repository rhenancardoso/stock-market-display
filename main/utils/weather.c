#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"
#include "weather.h"
#include "esp_log.h"

static const char *TAG = "weather";
extern cJSON *http_get_JSON_request(char *url);
extern char *http_request(char *url);
cJSON *weather_response = NULL;
struct Weather w_melb;
struct WeatherIcon w_icon;
char weather_url[131];

void pullWeatherFromJSON(cJSON *root)
{
    int i;
    cJSON *elem;
    cJSON *name;

    ESP_LOGI(TAG, "pull weather JSON");

    cJSON *obj_main = cJSON_GetObjectItemCaseSensitive(root, "main");
    float temp = cJSON_GetObjectItemCaseSensitive(obj_main, "temp")->valuedouble;
    float min_temp = cJSON_GetObjectItemCaseSensitive(obj_main, "temp_min")->valuedouble;
    float max_temp = cJSON_GetObjectItemCaseSensitive(obj_main, "temp_max")->valuedouble;
    uint8_t humidity = cJSON_GetObjectItemCaseSensitive(obj_main, "humidity")->valueint;
    cJSON *obj_clouds = cJSON_GetObjectItemCaseSensitive(root, "clouds");
    uint8_t cloud_pct = cJSON_GetObjectItemCaseSensitive(obj_clouds, "all")->valueint;
    cJSON *obj_weather = cJSON_GetObjectItemCaseSensitive(root, "weather");
    int n = cJSON_GetArraySize(obj_weather);
    for (i = 0; i < n; i++)
    {
        elem = cJSON_GetArrayItem(obj_weather, i);
        name = cJSON_GetObjectItem(elem, "icon");
        sprintf(w_melb.icon, "http://openweathermap.org/img/wn/%s@2x.png", name->valuestring);
    }

    w_melb.humidity = humidity;
    w_melb.temp = temp;
    w_melb.min_temp = min_temp;
    w_melb.max_temp = max_temp;
    w_melb.cloud_pct = cloud_pct;
    w_melb.is_data_collected = true;
    cJSON_Delete(root);

    ESP_LOGI(TAG, "temp = %f", w_melb.temp);
    ESP_LOGI(TAG, "humd = %d", w_melb.humidity);
    ESP_LOGI(TAG, "min_temp = %f", w_melb.min_temp);
    ESP_LOGI(TAG, "max_temp = %f", w_melb.max_temp);
    ESP_LOGI(TAG, "clouds = %d", w_melb.cloud_pct);
    ESP_LOGI(TAG, "icon = %s", w_melb.icon);
}

void getTemperature(void)
{
    sprintf(weather_url, API_URL, CITY_LAT, CITY_LON, API_KEY);
    ESP_LOGI(TAG, "weather url=%s", weather_url);
    weather_response = http_get_JSON_request(weather_url);
    if (weather_response != NULL)
    {
        ESP_LOGI(TAG, "response ready");
        pullWeatherFromJSON(weather_response);
        getWeatherIcon();
    }
    else
    {
        w_melb.is_data_collected = false;
        ESP_LOGE(TAG, "null API response");
    }
}

void getWeatherIcon(void)
{
    ESP_LOGI(TAG, "Weather Icon");
    char *icon_resp = http_request(w_melb.icon);
    w_icon.icon_img = (uint8_t *)icon_resp;
    w_icon.data_size = sizeof(w_icon.icon_img);
    w_icon.width = 10;
    w_icon.height = 10;
    ESP_LOGI(TAG, "%s", w_icon.icon_img);
}
