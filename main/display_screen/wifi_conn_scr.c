#include "wifi_conn_scr.h"
#include "esp_log.h"

static const char *TAG = "wifi_conn_page";

lv_obj_t *wifi_conn_page;
lv_obj_t *wifi_conn_lbl;
lv_obj_t *loading_arc;
static lv_style_t wifi_lbl_style;
static lv_style_t wifi_arc_style;
lv_timer_t *wifi_conn_update_screen;
uint8_t tick_count = 0;
bool reverse = false;

void wifi_conn_screen_ui(void)
{
    ESP_LOGI(TAG, "setup screen");
    static lv_style_t screen_bg_style;
    lv_obj_t *wifi_conn_scr_bg;

    ESP_LOGD(TAG, "Create wifi_conn_screen LVGL object");
    wifi_conn_page = lv_obj_create(NULL);
    // Create home page
    lv_obj_set_scrollbar_mode(wifi_conn_page, LV_SCROLLBAR_MODE_OFF);
    // Create Background object and style
    ESP_LOGD(TAG, "Initiliaze screen_bg_style");
    lv_style_init(&screen_bg_style);
    lv_style_set_bg_color(&screen_bg_style, black_color);
    lv_style_set_radius(&screen_bg_style, 0);
    lv_style_set_border_width(&screen_bg_style, 0);
    ESP_LOGD(TAG, "Create wifi_conn_scr_bg LVGL object");
    wifi_conn_scr_bg = lv_obj_create(wifi_conn_page);
    lv_obj_set_size(wifi_conn_scr_bg, BG_PANEL_SIZE_W, BG_PANEL_SIZE_H);
    lv_obj_align(wifi_conn_scr_bg, LV_ALIGN_TOP_LEFT, 0, 0); // this is to ensure that there is no other background colors peek through
    lv_obj_add_style(wifi_conn_scr_bg, &screen_bg_style, 0);
    // Create hour label object and configure
    lv_style_init(&wifi_lbl_style);
    lv_style_set_text_color(&wifi_lbl_style, white_color);
    lv_style_set_text_font(&wifi_lbl_style, &lv_font_montserrat_32);
    wifi_conn_lbl = lv_label_create(wifi_conn_page);
    lv_obj_add_style(wifi_conn_lbl, &wifi_lbl_style, 0);
    lv_label_set_text(wifi_conn_lbl, LV_SYMBOL_WIFI);
    lv_obj_align(wifi_conn_lbl, LV_ALIGN_CENTER, 0, 0);

    // Create Arc style
    lv_style_init(&wifi_arc_style);
    lv_style_set_arc_color(&wifi_arc_style, CL_SEA_SALT);
    lv_style_set_arc_width(&wifi_arc_style, 5);
    // Create Arc style
    static lv_style_t wifi_bg_arc_style;
    lv_style_init(&wifi_bg_arc_style);
    lv_style_set_arc_color(&wifi_bg_arc_style, black_color);
    // Create arc
    loading_arc = lv_arc_create(wifi_conn_page);
    lv_obj_add_style(loading_arc, &wifi_bg_arc_style, LV_PART_MAIN);
    lv_obj_add_style(loading_arc, &wifi_arc_style, LV_PART_INDICATOR);
    lv_obj_set_size(loading_arc, 125, 125);
    lv_arc_set_bg_angles(loading_arc, 0, 360);
    lv_arc_set_angles(loading_arc, 270, 270);
    lv_obj_align(loading_arc, LV_ALIGN_CENTER, 0, 0);
    lv_obj_remove_style(loading_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(loading_arc, LV_OBJ_FLAG_CLICKABLE);

    ESP_LOGD(TAG, "Create 'wifi_conn_timer' timer with %dms period", TIMER_PERIOD);
    wifi_conn_update_screen = lv_timer_create(_updateWifiPage, TIMER_PERIOD, NULL);
    wifi_conn_update_screen->repeat_count = -1;
}

void _updateWifiPage(lv_timer_t *timer)
{
    // Set angle
    if (tick_count * 5 >= 360 && !reverse)
    {
        tick_count = 0;
        reverse = true;
    }
    else if (tick_count * 5 >= 360 && reverse)
    {
        tick_count = 0;
        reverse = false;
    }
    // Reverse
    if (reverse)
    {
        lv_arc_set_angles(loading_arc, 90, tick_count * 5 + 90);
    }
    else
    {
        lv_arc_set_angles(loading_arc, tick_count * 5 + 90, 90);
    }

    tick_count++;
}