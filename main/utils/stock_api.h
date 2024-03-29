#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "esp_log.h"
#include "http_client.h"
#include "../key_config.h"

#ifndef _STOCK_API_H_
#define _STOCK_API_H_
#define STOCK_REQUEST_URL "https://script.google.com/macros/s/%s/exec?read=null"
#define STOCK_REQUEST_URL_EODHD "https://eodhd.com/api/real-time/%s?fmt=json&&api_token=%s"
#define STOCK_UPDATE_MS 2 * 60 * 1000 // Stock's updated every 5 minutes
#define STOCK_SCREEN_MS 30 * 1000     // Stock's screen displays every 30 seconds

#endif

/**
 * Extract JSON data object from HTTP Request
 */
void getStockData(void);

/**
 * Extract JSON response to Struct Stock object
 */
void pullPortfolioFromJSON(cJSON *root);

typedef struct stock
{
    char stk_type[1];
    char stk_location[2];
    char stk_name[25];
    uint16_t stk_qtd;
    float stk_price;
    float stk_value;
    float stck_pct_change;
    uint16_t stk_pe;
    uint16_t stk_eps;
} Stock;

extern Stock *stock_portfolio;