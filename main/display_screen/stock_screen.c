#include "stock_screen.h"
#include "../utils/stock_api.h"
#include "../utils/wifi.h"
#include "shared/heading_view.h"

#define INITIAL_STOCK_TIMER_MS (TIMER_PERIOD + 10)

static const char *TAG = "stock_page";

Stock *stock_portfolio = NULL;

lv_obj_t *stock_page;
lv_obj_t *stock_lbl;
lv_obj_t *stock_box;
lv_obj_t *stk_name;
lv_obj_t *stk_price;
lv_obj_t *stk_pct_change;
lv_obj_t *stk_name_1;
lv_obj_t *stk_price_1;
lv_obj_t *stk_pct_change_1;
lv_obj_t *stk_name_2;
lv_obj_t *stk_price_2;
lv_obj_t *stk_pct_change_2;
lv_obj_t *stk_name_3;
lv_obj_t *stk_price_3;
lv_obj_t *stk_pct_change_3;
lv_obj_t *stk_name_4;
lv_obj_t *stk_price_4;
lv_obj_t *stk_pct_change_4;
lv_obj_t *stk_name_5;
lv_obj_t *stk_price_5;
lv_obj_t *stk_pct_change_5;

lv_timer_t *timer_stock_containers;

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
    ESP_LOGI(TAG, "Calling setHeadingBox function");
    setHeadingBox(stock_page);

    _set5StockBox();

    ESP_LOGD(TAG, "Create 'timer_update_weather_box' timer with %dms period", TIMER_PERIOD_MS);
    timer_stock_containers = lv_timer_create(_updateStockPage, 50, NULL);
    timer_stock_containers->repeat_count = -1;
}

void _updateStockPage(lv_timer_t *timer)
{
    static lv_style_t stock_pct_style;
    // Update Heading
    updateHeading(timer);
    ESP_LOGI(TAG, "Updating Stock Data");
    if (stock_portfolio != NULL)
    {
        ESP_LOGI(TAG, "Stock data has been collected, updating stock box display");
        for (int index = 0; index < 5; index++)
        {
            char stock_name[40] = "";
            char stock_price[20] = "";
            char stock_change[10] = "";
            // Update Todays weather temperature
            ESP_LOGI(TAG, "Row: %d -> %s\t| A$ %.2f\t| %.2f|", index,
                     stock_portfolio[index].stk_name,
                     stock_portfolio[index].stk_price,
                     stock_portfolio[index].stck_pct_change);
            sprintf(stock_name, "%s", stock_portfolio[index].stk_name);
            sprintf(stock_price, "A$ %.2f", stock_portfolio[index].stk_price);
            sprintf(stock_change, "%.2f", stock_portfolio[index].stck_pct_change);

            lv_style_init(&stock_pct_style);
            if (stock_portfolio[index].stck_pct_change < 0)
            {
                lv_style_set_text_color(&stock_pct_style, lv_color_make(255, 90, 25));
            }
            else
            {
                lv_style_set_text_color(&stock_pct_style, lv_color_make(25, 90, 255));
            }

            switch (index)
            {
            case (0):
                lv_obj_add_style(stk_pct_change, &stock_pct_style, 0);
                lv_label_set_text(stk_name_1, stock_name);
                lv_label_set_text(stk_price_1, stock_price);
                lv_label_set_text(stk_pct_change_1, stock_change);
            case (1):
                lv_obj_add_style(stk_pct_change, &stock_pct_style, 0);
                lv_label_set_text(stk_name_2, stock_name);
                lv_label_set_text(stk_price_2, stock_price);
                lv_label_set_text(stk_pct_change_2, stock_change);
            case (2):
                lv_obj_add_style(stk_pct_change, &stock_pct_style, 0);
                lv_label_set_text(stk_name_3, stock_name);
                lv_label_set_text(stk_price_3, stock_price);
                lv_label_set_text(stk_pct_change_3, stock_change);
            case (3):
                lv_obj_add_style(stk_pct_change, &stock_pct_style, 0);
                lv_label_set_text(stk_name_4, stock_name);
                lv_label_set_text(stk_price_4, stock_price);
                lv_label_set_text(stk_pct_change_4, stock_change);
            case (4):
                lv_obj_add_style(stk_pct_change, &stock_pct_style, 0);
                lv_label_set_text(stk_name_5, stock_name);
                lv_label_set_text(stk_price_5, stock_price);
                lv_label_set_text(stk_pct_change_5, stock_change);
            }
        }
        timer_stock_containers->period = TIMER_PERIOD_MS;
    }
}

void _set5StockBox(void)
{
    ESP_LOGI(TAG, "Create stock_page_bg LVGL object");
    // Create Outer Box element
    ESP_LOGI(TAG, "Create outer box element");
    static lv_style_t outer_box_bg_style;
    lv_obj_t *stock_outer_box = lv_obj_create(stock_page);
    lv_obj_set_scrollbar_mode(stock_outer_box, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(stock_outer_box, STOCK_CONTAINER_W, STOCK_CONTAINER_H);
    lv_obj_align(stock_outer_box, LV_ALIGN_BOTTOM_RIGHT, 0, -WC_MARGIN_OFFSET);
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
    lv_style_set_text_color(&box_stock_txt_style, CL_STEEL_BLUE);
    lv_style_set_text_font(&box_stock_txt_style, &lv_font_montserrat_16);
    int box_pos_y;

    for (uint8_t i = 0; i < 5; i++)
    {
        ESP_LOGI(TAG, "For loop to create containers");

        box_pos_y = i * (STOCK_BOX_H + STOCK_CONTAINER_MARGIN) - WC_MARGIN_OFFSET;
        stock_box = lv_obj_create(stock_outer_box);
        lv_obj_set_scrollbar_mode(stock_box, LV_SCROLLBAR_MODE_OFF);
        lv_obj_align(stock_box, LV_ALIGN_TOP_MID, 0, box_pos_y);
        lv_obj_set_size(stock_box, STOCK_CONTAINER_W, STOCK_BOX_H);
        lv_style_set_pad_all(&outer_box_bg_style, WC_MARGIN_OFFSET);
        lv_obj_add_style(stock_box, &box_stock_style, 0);
        // local variables
        stk_name = lv_label_create(stock_box);
        stk_price = lv_label_create(stock_box);
        stk_pct_change = lv_label_create(stock_box);

        // Stock Name
        lv_obj_add_style(stk_name, &box_stock_txt_style, 0);
        lv_obj_align(stk_name, LV_ALIGN_LEFT_MID, 0, 0);
        // Stock Price
        lv_obj_add_style(stk_price, &box_stock_txt_style, 0);
        lv_obj_align(stk_price, LV_ALIGN_LEFT_MID, 120, 0);
        // Stock pct Change
        lv_obj_add_style(stk_pct_change, &box_stock_txt_style, 0);
        lv_obj_align(stk_pct_change, LV_ALIGN_LEFT_MID, 240, 0);
        // Stock PE

        switch (i)
        {
        case (0):
            stk_name_1 = stk_name;
            stk_price_1 = stk_price;
            stk_pct_change_1 = stk_pct_change;

        case (1):
            stk_name_2 = stk_name;
            stk_price_2 = stk_price;
            stk_pct_change_2 = stk_pct_change;

        case (2):
            stk_name_3 = stk_name;
            stk_price_3 = stk_price;
            stk_pct_change_3 = stk_pct_change;

        case (3):
            stk_name_4 = stk_name;
            stk_price_4 = stk_price;
            stk_pct_change_4 = stk_pct_change;

        case (4):
            stk_name_5 = stk_name;
            stk_price_5 = stk_price;
            stk_pct_change_5 = stk_pct_change;
        }
    }
}
