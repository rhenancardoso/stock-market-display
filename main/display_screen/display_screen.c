#include <stdio.h>
#include <time.h>
#include <string.h>
#include "esp_log.h"
#include "display_screen.h"
#include "../utils/wifi.h"
#include "../utils/weather.h"

static const char *TAG = "home_page";

extern struct Wifi wifi_conn;
extern struct Weather w_melb;

static lv_style_t time_style;
static lv_style_t weather_style;
static lv_style_t no_border_style;
static lv_style_t weather_frame_style;
lv_obj_t *home_page;
lv_obj_t *home_page_bg;
lv_obj_t *emergency_button;
lv_obj_t *time_lbl;
lv_obj_t *weather_lbl;
lv_obj_t *weather_icon_img;
lv_obj_t *weather_icon_frame;
lv_timer_t *timer_update;
char str_clock[9];
char str_weather[6];
uint8_t icon_move_x = 0;

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
        // Set weather icon
        if (icon_move_x > 150)
        {
            icon_move_x = 0;
        }
        icon_move_x++;
        setWeatherIconImg(w_melb.icon);
    }
    else
    {
        lv_label_set_text(weather_lbl, "- °C");
    }
}

void main_screen_ui(void)
{
    home_page = lv_obj_create(NULL);
    ESP_LOGI(TAG, "setup screen");
    // Create home page
    lv_obj_set_scrollbar_mode(home_page, LV_SCROLLBAR_MODE_OFF);
    // Create Background object and style
    lv_style_init(&no_border_style);
    lv_style_set_bg_color(&no_border_style, bckg_color);
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
    // Add weather container
    add_weather_container();

    lv_scr_load(home_page);
    ESP_LOGI(TAG, "Create timer");
    lv_timer_t *timer = lv_timer_create(update_header, 50, NULL);
    timer->repeat_count = -1;
}

void setWeatherIconImg(char icon[4])
{
    lv_obj_align(weather_icon_img, LV_ALIGN_LEFT_MID, icon_move_x - 100, 0);
    ESP_LOGI(TAG, "set weather icon: %s", icon);
    if (strcmp(icon, "01d") == 0)
    {
        lv_img_set_src(weather_icon_img, &w01d);
    }
    else if (strcmp(icon, "01n") == 0)
    {
        lv_img_set_src(weather_icon_img, &w01n);
    }
    else if (strcmp(icon, "02d") == 0)
    {
        lv_img_set_src(weather_icon_img, &w02d);
    }
    else if (strcmp(icon, "02n") == 0)
    {
        lv_img_set_src(weather_icon_img, &w02n);
    }
    else if (strcmp(icon, "03n") == 0)
    {
        lv_img_set_src(weather_icon_img, &w03n);
    }
    else if (strcmp(icon, "04n") == 0 || true)
    {
        lv_img_set_src(weather_icon_img, &w04n);
    }
    else if (strcmp(icon, "09n") == 0)
    {
        lv_img_set_src(weather_icon_img, &w09n);
    }
    else if (strcmp(icon, "10d") == 0)
    {
        lv_img_set_src(weather_icon_img, &w10d);
    }
    else if (strcmp(icon, "10n") == 0)
    {
        lv_img_set_src(weather_icon_img, &w10n);
    }
    else if (strcmp(icon, "11n") == 0)
    {
        lv_img_set_src(weather_icon_img, &w11n);
    }
}

void add_weather_container(void)
{
    // Weather icon frame
    weather_icon_frame = lv_obj_create(home_page);
    lv_obj_set_scrollbar_mode(weather_icon_frame, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(weather_icon_frame, 90, 80);
    lv_obj_align(weather_icon_frame, LV_ALIGN_LEFT_MID, 20, 0);
    lv_style_init(&weather_frame_style);
    lv_style_set_radius(&weather_frame_style, 20);
    lv_style_set_bg_color(&weather_frame_style, weather_box_bg);
    lv_style_set_border_color(&weather_frame_style, lv_color_make(255, 255, 255));
    lv_obj_add_style(weather_icon_frame, &weather_frame_style, 0);
    // create wether img
    weather_icon_img = lv_img_create(weather_icon_frame);
}