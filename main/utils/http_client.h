#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "cJSON.h"

#define MAX_HTTP_OUTPUT_BUFFER 4500
#define _TIMEOUT 10

/**
 * Extract JSON data object from HTTP request
 * @param url   url end point
 * @return cJSON object
 */
cJSON *http_get_JSON_request(char *url);
/**
 * Raw HTTP request
 */
void http_request(char *url);