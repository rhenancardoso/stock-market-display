#include "http_client.h"

static const char *TAG = "http_client";

static cJSON *json;

void http_request(char *url)
{
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0}; // Buffer to store response of http request

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,  // Specify transport type
        .crt_bundle_attach = esp_crt_bundle_attach, // Attach the certificate bundle
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
        esp_http_client_fetch_headers(client);
        int data_read = 0;
        int status_code = esp_http_client_get_status_code(client);

        ESP_LOGI(TAG, "Http response: %d, content-length: %lld", status_code, esp_http_client_get_content_length(client));
        if (status_code == 200)
        {
            ESP_LOGI(TAG, "Reading response");
            data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
        }
        else if (status_code >= 300)
        {
            char *location = "";
            esp_http_client_get_header(client, "location", location);
            ESP_LOGI(TAG, "Location from header: %s", location);
            ESP_LOGI(TAG, "Setting redirection");
            esp_http_client_set_redirection(client);
            ESP_LOGI(TAG, "Http client perform");
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
            }
            else
            {
                esp_http_client_fetch_headers(client);
                status_code = esp_http_client_get_status_code(client);
                ESP_LOGI(TAG, "Http response: %d, content-length: %lld", status_code, esp_http_client_get_content_length(client));
                if (status_code == 200)
                {
                    ESP_LOGI(TAG, "Reading response");
                    data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
                }
            }
        }

        if (data_read >= 0)
        {
            ESP_LOGI(TAG, "Data read response: %d", data_read);
            ESP_LOGI(TAG, "Parsing JSON");
            json = cJSON_Parse(output_buffer);
            if (json == NULL)
            {
                ESP_LOGE(TAG, "JSON error: %s", cJSON_GetErrorPtr());
            }
            else
            {
                ESP_LOGI(TAG, "JSON response parsed");
            }
        }
        else
        {
            json = NULL;
            ESP_LOGE(TAG, "Failed to read response");
        }
    }
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
}

void read_http_response(char *output_buffer)
{
    ESP_LOGI(TAG, "Parsing JSON");
    ESP_LOGI(TAG, "HTTP http_response buffer cJSON Parse: %s", output_buffer);
    json = cJSON_Parse(output_buffer);
    if (json == NULL)
    {
        ESP_LOGE(TAG, "JSON error: %s", cJSON_GetErrorPtr());
    }
    else
    {
        ESP_LOGI(TAG, "JSON response parsed");
    }
}

cJSON *http_get_JSON_request(char *url)
{
    ESP_LOGI(TAG, "HTTP JSON request");
    http_request(url);
    return json;
}