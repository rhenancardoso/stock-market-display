#include <stdio.h>
#include <time.h>
#include "lvgl.h"
#include "config.h"
#include "esp_log.h"
#include "wifi.h"
#include "weather.h"

#define bg_color lv_color_make(2, 2, 2)
#define txt_color lv_color_make(250, 250, 250)

#define OFFSET_SIZE 10
#define OFFSET_OBJ (int8_t) OFFSET_SIZE / 2
#define BG_PANEL_SIZE_W EXAMPLE_LCD_H_RES + OFFSET_SIZE
#define BG_PANEL_SIZE_H EXAMPLE_LCD_V_RES + OFFSET_SIZE

static const char *TAG = "home_page";
extern struct Wifi wifi_conn;
extern struct Weather w_melb;

void update_time(lv_timer_t *timer);

static lv_style_t time_style;
static lv_style_t weather_style;
static lv_style_t no_border_style;
lv_obj_t *home_page;
lv_obj_t *home_page_bg;
lv_obj_t *emergency_button;
lv_obj_t *time_lbl;
lv_obj_t *weather_lbl;
lv_timer_t *timer_update;
char str_clock[9];
char str_weather[6];

void update_header(lv_timer_t *timer)
{
    // Get time
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(str_clock, sizeof(str_clock), "  %H:%M", &timeinfo);
    char full_str[11] = "";
    if (wifi_conn.is_connected)
    {
        strcpy(full_str, LV_SYMBOL_WIFI);
        strcat(full_str, str_clock);
    }
    else
    {
        strcpy(full_str, str_clock);
    }
    lv_label_set_text(time_lbl, full_str);
    if (w_melb.is_data_collected)
    {
        sprintf(str_weather, "%.0f °C", w_melb.temp);
        lv_label_set_text(weather_lbl, str_weather);
    }
    else
    {
        lv_label_set_text(weather_lbl, "- °C");
    }
}

void main_screen_ui(void)
{
    char str_clock[10] = "";
    home_page = lv_obj_create(NULL);
    ESP_LOGI(TAG, "setup screen");
    // Create home page
    lv_obj_set_scrollbar_mode(home_page, LV_SCROLLBAR_MODE_OFF);
    // Create Background object and style
    lv_style_init(&no_border_style);
    lv_style_set_bg_color(&no_border_style, bg_color);
    home_page_bg = lv_obj_create(home_page);
    lv_obj_set_size(home_page_bg, BG_PANEL_SIZE_W, BG_PANEL_SIZE_H);
    lv_obj_align(home_page_bg, LV_ALIGN_TOP_LEFT, -OFFSET_OBJ, -OFFSET_OBJ); // this is to ensure that there is no other background colors peek through
    lv_obj_add_style(home_page_bg, &no_border_style, 0);
    // Create hour label object and configure
    lv_style_init(&time_style);
    lv_style_set_text_color(&time_style, txt_color);
    lv_style_set_text_font(&time_style, &lv_font_montserrat_16);
    time_lbl = lv_label_create(home_page);
    lv_obj_add_style(time_lbl, &time_style, 0);
    lv_label_set_text(time_lbl, str_clock);
    lv_obj_align(time_lbl, LV_ALIGN_TOP_RIGHT, -OFFSET_OBJ, OFFSET_OBJ);
    // Create temp label object and configure
    lv_style_init(&weather_style);
    lv_style_set_text_color(&weather_style, txt_color);
    lv_style_set_text_font(&weather_style, &lv_font_montserrat_16);
    weather_lbl = lv_label_create(home_page);
    lv_obj_add_style(weather_lbl, &weather_style, 0);
    lv_label_set_text(weather_lbl, str_clock);
    lv_obj_align(weather_lbl, LV_ALIGN_TOP_LEFT, OFFSET_OBJ, OFFSET_OBJ);
    lv_scr_load(home_page);

    ESP_LOGI(TAG, "Create timer");
    lv_timer_t *timer = lv_timer_create(update_header, 50, NULL);
    timer->repeat_count = -1;
}