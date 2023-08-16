#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "esp_log.h"
#include "http_client.h"
#include "../key_config.h"

#ifndef _STOCK_API_H_
#define _STOCK_API_H_
#define STOCK_WEB_API "https://script.google.com/macros/s/AKfycbxCoJvtC2REGZ9nJr7A0dHZtbSCQ4jetiDvbBjMzxPbBzAcBgp09cd82EpxHdq2fISn/exec?read"
#endif

void getStockData(void);

typedef struct stock
{
    char stk_type[1];
    char stk_location[2];
    char stk_name[30];
    float stk_qtd;
    float stk_price;
    float stk_value;
    uint16_t stk_pe;
};