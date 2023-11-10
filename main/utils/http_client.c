#include "http_client.h"

static const char *TAG = "http_client";

static cJSON *json;
static cJSON *stock_json;

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
        uint16_t status_code = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %" PRId64,
                 status_code,
                 esp_http_client_get_content_length(client));
        esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
        if (strlen(output_buffer) >= 0)
        {
            ESP_LOGI(TAG, "Data read response: %d", strlen(output_buffer));
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

void ReadHttpResponse(char *output_buffer)
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

// ============== STOCK MARKET CALL =================================================== //

static char *buffer_output; // Buffer to store full response from GET request
char *response_output;      // Buffer to store JSON data after cleaning

esp_err_t _http_stock_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        // Append received data to the buffer_output
        if ((evt->data_len + strlen(buffer_output)) < MAX_HTTP_OUTPUT_BUFFER)
        {
            strncat(buffer_output, (char *)evt->data, evt->data_len);
        }
        else
        {
            ESP_LOGE(TAG, "JSON data too large for buffer");
        }
        break;
    default:
        break;
    }
    return ESP_OK;
}

cJSON *http_get_portfolio_request(char *url)
{
    ESP_LOGI(TAG, "HTTP JSON request");
    StockHttpRequest(url);
    return stock_json;
}

void StockHttpRequest(char *url)
{
    buffer_output = (char *)malloc(MAX_HTTP_OUTPUT_BUFFER);
    ESP_LOGI(TAG, "HTTP Stock request URL: %s", url);
    // Configure the HTTP client
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_stock_event_handler,
        .method = HTTP_METHOD_GET,
        .max_redirection_count = 3,
    };
    // Create an HTTP client
    esp_http_client_handle_t client = esp_http_client_init(&config);
    //  Perform the HTTP GET request
    esp_err_t err = esp_http_client_perform(client);

    if (err != ESP_OK)
    {
        stock_json = NULL;
        ESP_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
    }
    else
    {
        esp_http_client_fetch_headers(client);
        uint16_t status_code = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %" PRId64,
                 status_code,
                 esp_http_client_get_content_length(client));
        RemoveString(FIND_STRING_RESPONSE);
        ESP_LOGI(TAG, "cJSON response: %s", response_output);
        stock_json = cJSON_Parse(response_output);
    }
    if (buffer_output != NULL)
    {
        ESP_LOGI(TAG, "Release buffer_output memory allocation via `free`");
        // Free memory used for the response buffer
        free(buffer_output);
    }
    // Clean up the client
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
}

void RemoveString(char *findString)
{
    ESP_LOGI(TAG, "Removing heading string from stock response");
    int strLen = strlen(buffer_output);

    if (strLen == 0)
    {
        // Invalid input, do nothing.
        ESP_LOGE(TAG, "Invalid input / strLen: %d", strLen);
        return;
    }

    // Get giving string within the buffer output
    response_output = strstr(buffer_output, findString);
    ESP_LOGD(TAG, "response buffer: %s", response_output);
}