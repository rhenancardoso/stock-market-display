#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_log.h"
#include "cJSON.h"

#define MAX_HTTP_OUTPUT_BUFFER 4500
#define _TIMEOUT 10
#define FIND_STRING_RESPONSE "[["

/**
 * Extract JSON data object from Weather API HTTP request
 * @param url   url end point
 * @return cJSON object
 */
extern cJSON *http_get_JSON_request(char *url);

/**
 * Extract JSON data object from Stock-Market API HTTP request
 * @param url   url end point
 * @return cJSON object
 */
extern cJSON *http_get_portfolio_request(char *url);

/**
 * Raw HTTP request
 */
void http_request(char *url);
/**
 * Raw HTTP request for the Google Sheets web app
 */
void StockHttpRequest(char *url);
/**
 * @brief Event handler for the HTTP requests
 *
 */
esp_err_t _http_event_handler(esp_http_client_event_t *evt);
/**
 * @brief Search giving string and assign the rest to response_output
 *
 */
void RemoveString(char *findString);