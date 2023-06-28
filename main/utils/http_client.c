#include "http_client.h"

static const char *TAG = "http_client";

char *response_data;
size_t response_len = 0;
bool data_received = false;

esp_err_t _http_event_handler(esp_http_client_event_t *event)
{
    switch (event->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "Getting data...");
        // Resize the buffer to fit the new chunk of data
        response_data = realloc(response_data, response_len + event->data_len);
        memcpy(response_data + response_len, event->data, event->data_len);
        response_len += event->data_len;
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "Data received!");
        data_received = true;
        break;
    default:
        break;
    }
    return ESP_OK;
}

char *http_request(char *url)
{
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0}; // Buffer to store response of http request
    uint8_t itr_timeout = 0;
    data_received = false;

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .event_handler = _http_event_handler,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // GET Request
    ESP_LOGI(TAG, "GET %s", url);
    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        return NULL;
    }
    else
    {
        int status_code = esp_http_client_get_status_code(client);
        if (status_code == 200)
        {
            ESP_LOGI(TAG, "Message sent Successfully to %s", url);
            while (true)
            {
                vTaskDelay(50 / portTICK_PERIOD_MS);
                if (data_received)
                {
                    ESP_LOGD(TAG, "Returning HTTP response");
                    esp_http_client_cleanup(client);
                    return response_data;
                }
                else
                {
                    itr_timeout++;
                    if (itr_timeout > _TIMEOUT)
                    {
                        ESP_LOGE(TAG, "GET request timeout!");
                        return NULL;
                    }
                }
            }
        }
        else
        {
            ESP_LOGE(TAG, "Message sent Failed");
            return NULL;
        }
    }
}

cJSON *http_get_JSON_request(char *url)
{
    ESP_LOGD(TAG, "HTTP JSON request");
    char *http_response = http_request(url);
    ESP_LOGD(TAG, "HTTP response is NULL? %s", (http_response == NULL) ? "true" : "false");
    if (http_response)
    {
        ESP_LOGD(TAG, "Parsing JSON");
        cJSON *json = cJSON_Parse(http_response);
        if (json == NULL)
        {
            ESP_LOGE(TAG, "JSON error: %s", cJSON_GetErrorPtr());
        }
        return json;
    }
    return NULL;
}