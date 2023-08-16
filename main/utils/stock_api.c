#include "stock_api.h"

static const char *TAG = "stock_api";

void getStockData(void)
{
    http_request(STOCK_WEB_API);
}
