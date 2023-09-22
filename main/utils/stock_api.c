#include "stock_api.h"

static const char *TAG = "stock_api";

char stock_url_request[150];
extern cJSON *http_get_portfolio_request(char *url);

void getStockData(void)
{
    ESP_LOGI(TAG, "Get portfolio data from stock-market sheet");
    sprintf(stock_url_request, STOCK_REQUEST_URL, GOOGLE_API_DEPLOY_ID);
    cJSON *http_api_response = http_get_portfolio_request(stock_url_request);
    if (http_api_response != NULL)
    {
        ESP_LOGI(TAG, "Portfolio response ready");
        pullPortfolioFromJSON(http_api_response);
    }
    else
    {
        ESP_LOGE(TAG, "null portfolio API response");
    }
    ESP_LOGI(TAG, "Get portfolio data complete. Deleting JSON");
    cJSON_Delete(http_api_response);
}

void pullPortfolioFromJSON(cJSON *root)
{
    ESP_LOGI(TAG, "pull portfolio JSON");
    uint8_t sheet_row = cJSON_GetArraySize(root);
    ESP_LOGI(TAG, "JSON Array size: %d", sheet_row);
    if (sizeof(stock_portfolio) != ((sheet_row - 1) * sizeof(Stock)))
    {
        ESP_LOGI(TAG, "Resize stock_portfolio array");
        if (sizeof(stock_portfolio) >= sizeof(Stock))
        {
            free(stock_portfolio);
        }
        stock_portfolio = (Stock *)malloc((sheet_row - 1) * sizeof(Stock));
    }

    if (cJSON_IsArray(root))
    {
        uint8_t data_row = 0;
        ESP_LOGI(TAG, "Stock data JSON is an Array");
        cJSON *row;
        cJSON_ArrayForEach(row, root)
        {
            if (data_row > 0)
            {
                uint8_t column = 0;
                cJSON *item = NULL;
                cJSON_ArrayForEach(item, row)
                {
                    switch (column)
                    {
                    case 0:
                        sprintf(stock_portfolio[data_row - 1].stk_type, "%s", item->valuestring);
                        break;
                    case 1:
                        sprintf(stock_portfolio[data_row - 1].stk_location, "%s", item->valuestring);
                        break;
                    case 2:
                        sprintf(stock_portfolio[data_row - 1].stk_name, "%s", item->valuestring);
                        break;
                    case 4:
                        stock_portfolio[data_row - 1].stk_qtd = item->valueint;
                        break;
                    case 5:
                        stock_portfolio[data_row - 1].stk_price = item->valuedouble;
                        break;
                    case 6:
                        stock_portfolio[data_row - 1].stck_pct_change = item->valuedouble;
                        break;
                    case 7:
                        stock_portfolio[data_row - 1].stk_value = item->valuedouble;
                        break;
                    case 8:
                        stock_portfolio[data_row - 1].stk_pe = 100 * item->valueint;
                        break;
                    case 9:
                        stock_portfolio[data_row - 1].stk_eps = 100 * item->valueint;
                        break;
                    }
                    column++;
                }
            }
            data_row++;
        }
    }
}