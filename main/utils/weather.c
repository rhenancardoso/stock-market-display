#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"
#include "weather.h"
#include "esp_log.h"

static const char *TAG = "weather";
extern void http_get_request(char *url);
extern cJSON *parse_JSON(void);

void pullDataFromJSON(weather *w, cJSON *root)
{
    cJSON *obj = cJSON_GetObjectItemCaseSensitive(root, "main");
    float temp = cJSON_GetObjectItemCaseSensitive(obj, "temp")->valuedouble;
    int humidity = cJSON_GetObjectItemCaseSensitive(obj, "humidity")->valueint;
    ESP_LOGI(TAG, "TEMP = %f", temp);
    ESP_LOGI(TAG, "HUM = %d", humidity);
    cJSON_Delete(root);
}

void getTemperature(weather *w)
{
    char weather_url[120];
    sprintf(weather_url, "http://api.openweathermap.org/data/2.5/weather?lat=%s&lon=%s&appid=%s", MELB_LAT, MELB_LON, API_KEY);
    ESP_LOGI(TAG, "weather url=%s", weather_url);
    http_get_request(weather_url);
    cJSON *weather_response = NULL;

    while (weather_response == NULL)
    {
        weather_response = parse_JSON();
        ESP_LOGI(TAG, "waiting ...");
        // wait for response
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "response ready");
    pullDataFromJSON(w, weather_response);
}
