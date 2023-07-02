#include "http_client.h"

static const char *TAG = "http_client";

char *http_request(char *url)
{
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0}; // Buffer to store response of http request
    char *response_data = NULL;

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // GET Request
    ESP_LOGI(TAG, "GET %s", url);
    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
    }
    else
    {
        int header_length = esp_http_client_fetch_headers(client);
        if (header_length < 0)
        {
            ESP_LOGE(TAG, "HTTP client fetch headers failed");
        }
        else
        {
            ESP_LOGI(TAG, "Read http response");
            int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0)
            {
                int content_length = esp_http_client_get_content_length(client);
                ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                         esp_http_client_get_status_code(client),
                         content_length);
                response_data = output_buffer;
            }
            else
            {
                ESP_LOGE(TAG, "Failed to read response");
            }
        }
    }
    esp_http_client_close(client);
    ESP_LOGI(TAG, "HTTP output_buffer: %s", response_data);
    return response_data;
}

cJSON *http_get_JSON_request(char *url)
{
    ESP_LOGI(TAG, "HTTP JSON request");
    char *http_response = http_request(url);
    ESP_LOGI(TAG, "HTTP response is NULL? %s", (http_response == NULL) ? "true" : "false");
    if (http_response)
    {
        ESP_LOGI(TAG, "Parsing JSON");
        cJSON *json = cJSON_Parse(http_response);
        if (json == NULL)
        {
            ESP_LOGE(TAG, "JSON error: %s", cJSON_GetErrorPtr());
        }
        char *out = cJSON_Print(json);
        ESP_LOGI(TAG, "JSON response: %s", out);
        free(out);
        return json;
    }
    return NULL;
}