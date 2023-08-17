#include "stock_api.h"

static const char *TAG = "stock_api";

char stock_url_request[150];

void getStockData(void)
{
    sprintf(stock_url_request, STOCK_REQUEST_URL, GOOGLE_API_DEPLOY_ID);
    http_request_stock(stock_url_request);
}
