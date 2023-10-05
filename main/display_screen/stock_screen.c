#include "stock_screen.h"
#include <time.h>
#include <math.h>
#include "../utils/stock_api.h"
#include "../utils/wifi.h"
#include "shared/heading_view.h"

#define INITIAL_STOCK_TIMER_MS (TIMER_PERIOD + 10)

static const char *TAG = "stock_page";
static const int NUM_SHARES = 21; // TODO: read from the stock struct
Stock *stock_portfolio = NULL;

lv_obj_t *stock_page;
lv_obj_t *stock_outer_box;
lv_timer_t *timer_stock_data;
static lv_style_t stock_pct_red_style;
static lv_style_t stock_pct_blue_style;
int active_page;

void stock_screen_ui(void)
{
    ESP_LOGI(TAG, "setup screen");
    static lv_style_t screen_bg_style;
    lv_obj_t *stock_page_bg;

    ESP_LOGI(TAG, "Create stock_page LVGL object");
    stock_page = lv_obj_create(NULL);
    // Create home page
    lv_obj_set_scrollbar_mode(stock_page, LV_SCROLLBAR_MODE_OFF);
    // Create Background object and style
    ESP_LOGI(TAG, "Initiliaze screen_bg_style");
    lv_style_init(&screen_bg_style);
    lv_style_set_bg_color(&screen_bg_style, black_color);
    lv_style_set_radius(&screen_bg_style, 0);
    lv_style_set_border_width(&screen_bg_style, 0);
    ESP_LOGI(TAG, "Create stock_page_bg LVGL object");
    stock_page_bg = lv_obj_create(stock_page);
    lv_obj_set_size(stock_page_bg, BG_PANEL_SIZE_W, BG_PANEL_SIZE_H);
    lv_obj_align(stock_page_bg, LV_ALIGN_TOP_LEFT, 0, 0); // this is to ensure that there is no other background colors peek through
    lv_obj_add_style(stock_page_bg, &screen_bg_style, 0);

    // Add heading
    ESP_LOGI(TAG, "Home Screen calling setHeadingBox function");
    setHeadingBox(stock_page);
    // Add Stock Boxes
    _createStockBoxes();

    ESP_LOGD(TAG, "Create 'timer_update_weather_box' timer with %dms period", 100);
    timer_stock_data = lv_timer_create(_updateStockData, 100, NULL);
    timer_stock_data->repeat_count = -1;
}

void _updateStockData(lv_timer_t *timer)
{
    if (stock_portfolio != NULL)
    {
        if (timer_stock_data->period != PAGE_TIMER_UPDATE_TICK)
        {
            ESP_LOGI(TAG, "changing data period for timer");
            timer_stock_data->period = PAGE_TIMER_UPDATE_TICK;
        }

        uint32_t stk_box_cnt = lv_obj_get_child_cnt(stock_outer_box);
        for (uint8_t ind = 0; ind < stk_box_cnt; ind++)
        {
            lv_obj_t *stk_box = lv_obj_get_child(stock_outer_box, ind);
            uint8_t stock_index = ind + active_page * MAX_BOXES_PER_PAGE;
            // Update stock data lables
            char stock_name[25] = "--";
            char stock_price[15] = "A$ --.--";
            char stock_change[10] = "-.-- %%";
            if (stock_index <= NUM_SHARES)
            {
                ESP_LOGI(TAG, "Row: %d -> %s\t\t| A$ %.2f\t| %.2f|", stock_index,
                         stock_portfolio[stock_index].stk_name,
                         stock_portfolio[stock_index].stk_price,
                         stock_portfolio[stock_index].stck_pct_change);
                sprintf(stock_name, "%s", stock_portfolio[stock_index].stk_name);
                sprintf(stock_price, "A$ %.2f", stock_portfolio[stock_index].stk_price);
                sprintf(stock_change, "%.2f %%", stock_portfolio[stock_index].stck_pct_change);
            }
            else
            {
                sprintf(stock_name, stock_name);
                sprintf(stock_price, stock_price);
                sprintf(stock_change, stock_change);
            }

            uint32_t lbl_cnt = lv_obj_get_child_cnt(stk_box);
            for (uint8_t lbl_ind = 0; lbl_ind < lbl_cnt; lbl_ind++)
            {
                lv_obj_t *box_lbl = lv_obj_get_child(stk_box, lbl_ind);
                switch (lbl_ind)
                {
                case 0:
                    lv_label_set_text(box_lbl, stock_name);
                    break;

                case 1:
                    lv_label_set_text(box_lbl, stock_price);
                    break;

                case 2:
                    if (stock_portfolio[stock_index].stck_pct_change < 0)
                    {
                        lv_obj_add_style(box_lbl, &stock_pct_red_style, 0);
                    }
                    else
                    {
                        lv_obj_add_style(box_lbl, &stock_pct_blue_style, 0);
                    }
                    lv_label_set_text(box_lbl, stock_change);
                    break;

                default:
                    break;
                }
            }
        }

        active_page++;
        if (active_page > (int)(NUM_SHARES / MAX_BOXES_PER_PAGE))
            active_page = 0;
    }
}

void _createStockBoxes(void)
{
    ESP_LOGI(TAG, "Create stock_page_bg LVGL object");
    // Create Outer Box element
    ESP_LOGI(TAG, "Create outer box element");
    static lv_style_t outer_box_bg_style;
    stock_outer_box = lv_obj_create(stock_page);
    lv_obj_set_scrollbar_mode(stock_outer_box, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(stock_outer_box, STOCK_CONTAINER_W, STOCK_CONTAINER_H);
    lv_obj_align(stock_outer_box, LV_ALIGN_BOTTOM_RIGHT, 0, -WC_MARGIN_OFFSET);
    lv_obj_set_scrollbar_mode(stock_outer_box, LV_SCROLLBAR_MODE_OFF);

    lv_style_init(&outer_box_bg_style);
    lv_style_set_radius(&outer_box_bg_style, 0);
    lv_style_set_border_width(&outer_box_bg_style, 0);
    lv_style_set_bg_color(&outer_box_bg_style, black_color);
    lv_style_set_pad_all(&outer_box_bg_style, WC_MARGIN_OFFSET);
    lv_obj_add_style(stock_outer_box, &outer_box_bg_style, 0);

    // Create stock boxes styles
    ESP_LOGI(TAG, "Create boxes styles");
    static lv_style_t box_stock_style;
    lv_style_init(&box_stock_style);
    lv_style_set_radius(&box_stock_style, 7);
    lv_style_set_border_width(&box_stock_style, 0);
    lv_style_set_bg_color(&box_stock_style, CL_SEA_SALT);

    // create text style
    static lv_style_t box_stock_txt_style;
    ESP_LOGI(TAG, "Create text styles");
    lv_style_init(&box_stock_txt_style);
    lv_style_set_text_opa(&box_stock_txt_style, LV_OPA_100);
    lv_style_set_text_color(&box_stock_txt_style, CL_STEEL_BLUE);
    lv_style_set_text_font(&box_stock_txt_style, &lv_font_montserrat_16);

    lv_style_init(&stock_pct_red_style);
    lv_style_set_text_color(&stock_pct_red_style, RED_STOCK);
    lv_style_init(&stock_pct_blue_style);
    lv_style_set_text_color(&stock_pct_blue_style, BLUE_STOCK);

    int box_pos_y;
    for (uint8_t i = 0; i < MAX_BOXES_PER_PAGE; i++)
    {
        box_pos_y = i * (STOCK_BOX_H + STOCK_CONTAINER_MARGIN) - WC_MARGIN_OFFSET;
        lv_obj_t *stock_box = lv_obj_create(stock_outer_box);
        lv_obj_set_scrollbar_mode(stock_box, LV_SCROLLBAR_MODE_OFF);
        lv_obj_align(stock_box, LV_ALIGN_TOP_MID, 0, box_pos_y);
        lv_obj_set_size(stock_box, STOCK_CONTAINER_W, STOCK_BOX_H);
        lv_style_set_pad_all(&outer_box_bg_style, WC_MARGIN_OFFSET);
        lv_obj_add_style(stock_box, &box_stock_style, 0);

        // Stock Name
        lv_obj_t *stk_name = lv_label_create(stock_box);
        lv_obj_add_style(stk_name, &box_stock_txt_style, 0);
        lv_obj_align(stk_name, LV_ALIGN_LEFT_MID, -5, 0);
        lv_label_set_text(stk_name, "--");
        // Stock Price
        lv_obj_t *stk_price = lv_label_create(stock_box);
        lv_obj_add_style(stk_price, &box_stock_txt_style, 0);
        lv_obj_align(stk_price, LV_ALIGN_LEFT_MID, 140, 0);
        lv_label_set_text(stk_price, "A$ --.--");
        // Stock pct Change
        lv_obj_t *stk_pct_change = lv_label_create(stock_box);
        lv_obj_add_style(stk_pct_change, &box_stock_txt_style, 0);
        lv_obj_align(stk_pct_change, LV_ALIGN_LEFT_MID, 240, 0);
        lv_label_set_text(stk_pct_change, "-.-- %%");
    }
}
