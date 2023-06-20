#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "cJSON.h"

#define MAX_HTTP_OUTPUT_BUFFER 2048
#define _TIMEOUT 10

static const char *TAG = "http_client";
cJSON *http_get_request(char *url);
esp_err_t _http_event_handler(esp_http_client_event_t *event);

cJSON *root = NULL;
char *response_data = NULL;
size_t response_len = 0;
bool all_chunks_received = false;

esp_err_t _http_event_handler(esp_http_client_event_t *event)
{
    switch (event->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        // Resize the buffer to fit the new chunk of data
        response_data = realloc(response_data, response_len + event->data_len);
        memcpy(response_data + response_len, event->data, event->data_len);
        response_len += event->data_len;
        break;
    case HTTP_EVENT_ON_FINISH:
        all_chunks_received = true;
        break;
    default:
        break;
    }
    return ESP_OK;
}

cJSON *http_get_request(char *url)
{
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0}; // Buffer to store response of http request
    uint8_t itr_timeout = 0;
    all_chunks_received = false;

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .event_handler = _http_event_handler,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // GET Request
    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
    }
    else
    {
        int status_code = esp_http_client_get_status_code(client);
        if (status_code == 200)
        {
            ESP_LOGI(TAG, "Message sent Successfully");
            while (true)
            {
                vTaskDelay(100 / portTICK_PERIOD_MS);
                if (all_chunks_received)
                {
                    root = cJSON_Parse(response_data);
                    break;
                }
                else
                {
                    itr_timeout++;
                    if (itr_timeout > _TIMEOUT)
                    {
                        ESP_LOGI(TAG, "GET request timeout!");
                        break;
                    }
                }
            }
        }
        else
        {
            ESP_LOGE(TAG, "Message sent Failed");
        }
    }
    esp_http_client_cleanup(client);
    return root;
}