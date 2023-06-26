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
static lv_style_t battery_style;
static lv_style_t weather_frame_style;
static lv_style_t day_forecast_style;
lv_obj_t *home_page;
lv_obj_t *time_lbl;
lv_obj_t *battery_lbl;
lv_obj_t *weather_lbl;
lv_obj_t *weather_icon_img;
lv_obj_t *weather_icon_img_rpt;
lv_obj_t *day1_day_wk;
lv_obj_t *day1_min;
lv_obj_t *day1_max;
lv_obj_t *day2_day_wk;
lv_obj_t *day2_min;
lv_obj_t *day2_max;
lv_obj_t *day3_day_wk;
lv_obj_t *day3_min;
lv_obj_t *day3_max;
lv_obj_t *day4_day_wk;
lv_obj_t *day4_min;
lv_obj_t *day4_max;
lv_obj_t *day5_day_wk;
lv_obj_t *day5_min;
lv_obj_t *day5_max;

lv_timer_t *timer_update;
char str_clock[9];
char str_weather[5];
char str_battery[3];
uint8_t icon_move_x = 0;
uint32_t w_icon_width = 0;

void main_screen_ui(void)
{
    static lv_style_t screen_bg_style;
    lv_obj_t *home_page_bg;

    home_page = lv_obj_create(NULL);
    ESP_LOGI(TAG, "setup screen");
    // Create home page
    lv_obj_set_scrollbar_mode(home_page, LV_SCROLLBAR_MODE_OFF);
    // Create Background object and style
    lv_style_init(&screen_bg_style);
    lv_style_set_bg_color(&screen_bg_style, bckg_color);
    lv_style_set_radius(&screen_bg_style, 0);
    lv_style_set_border_width(&screen_bg_style, 0);
    home_page_bg = lv_obj_create(home_page);
    lv_obj_set_size(home_page_bg, BG_PANEL_SIZE_W, BG_PANEL_SIZE_H);
    lv_obj_align(home_page_bg, LV_ALIGN_TOP_LEFT, 0, 0); // this is to ensure that there is no other background colors peek through
    lv_obj_add_style(home_page_bg, &screen_bg_style, 0);

    // Add heading
    _setHeadingBox();
    // Add weather container
    _setWeatherBox();
    // Add weather forecast boxes
    _set5DaysForecastBox();

    lv_scr_load(home_page);
    ESP_LOGI(TAG, "Create timer");
    lv_timer_t *timer = lv_timer_create(_updateMainPage, 25, NULL);
    timer->repeat_count = -1;
}

void _updateMainPage(lv_timer_t *timer)
{
    // Get time
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(str_clock, sizeof(str_clock), "  %H:%M", &timeinfo);
    char full_str_CLK[11] = "";
    if (wifi_conn.is_connected)
    {
        strcpy(full_str_CLK, LV_SYMBOL_WIFI);
        strcat(full_str_CLK, str_clock);
    }
    else
    {
        strcpy(full_str_CLK, str_clock);
    }
    lv_label_set_text(time_lbl, full_str_CLK);
    // Battery
    lv_label_set_text(battery_lbl, LV_SYMBOL_BATTERY_FULL);

    // Weather
    if (w_melb.is_data_collected)
    {
        sprintf(str_weather, "%.0f°C", w_melb.temp);
        lv_label_set_text(weather_lbl, str_weather);
        // Set weather icon
        icon_move_x++;
        w_icon_width = _setWeatherIconImg(w_melb.icon, weather_icon_img);
        w_icon_width = _setWeatherIconImg(w_melb.icon, weather_icon_img_rpt);
        lv_obj_align(weather_icon_img, LV_ALIGN_LEFT_MID, icon_move_x - w_icon_width, 2);
        lv_obj_align(weather_icon_img_rpt, LV_ALIGN_LEFT_MID, icon_move_x, 2);
        if (icon_move_x > w_icon_width)
        {
            icon_move_x = 0;
        }
    }
    else
    {
        lv_label_set_text(weather_lbl, "--°C");
    }
    // Weather Forecast
    lv_label_set_text(day1_day_wk, "MON");
    lv_label_set_text(day1_min, "--°C");
    lv_label_set_text(day1_max, "--°C");
    lv_label_set_text(day2_day_wk, "TUE");
    lv_label_set_text(day2_min, "--°C");
    lv_label_set_text(day2_max, "--°C");
    lv_label_set_text(day3_day_wk, "WED");
    lv_label_set_text(day3_min, "--°C");
    lv_label_set_text(day3_max, "--°C");
    lv_label_set_text(day4_day_wk, "THU");
    lv_label_set_text(day4_min, "--°C");
    lv_label_set_text(day4_max, "--°C");
    lv_label_set_text(day5_day_wk, "FRI");
    lv_label_set_text(day5_min, "--°C");
    lv_label_set_text(day5_max, "--°C");
}

void _setHeadingBox(void)
{
    lv_obj_t *heading_container;
    static lv_style_t heading_container_style;

    // Create Heading container
    heading_container = lv_obj_create(home_page);
    lv_obj_set_scrollbar_mode(heading_container, LV_SCROLLBAR_MODE_OFF);
    lv_style_init(&heading_container_style);
    lv_style_set_radius(&heading_container_style, 0);
    lv_style_set_border_width(&heading_container_style, 0);
    lv_style_set_bg_color(&heading_container_style, CL_STEEL_BLUE);
    lv_obj_set_size(heading_container, HEADING_W, HEADING_H);
    lv_obj_add_style(heading_container, &heading_container_style, 0);
    // Create hour label object and configure
    lv_style_init(&time_style);
    lv_style_set_text_color(&time_style, txt_color);
    lv_style_set_text_font(&time_style, &lv_font_montserrat_16);
    time_lbl = lv_label_create(heading_container);
    lv_obj_add_style(time_lbl, &time_style, 0);
    lv_label_set_text(time_lbl, str_clock);
    lv_obj_align(time_lbl, LV_ALIGN_RIGHT_MID, 0, 0);
    // Create temp label object and configure
    lv_style_init(&battery_style);
    lv_style_set_text_color(&battery_style, txt_color);
    lv_style_set_text_font(&battery_style, &lv_font_montserrat_16);
    battery_lbl = lv_label_create(heading_container);
    lv_obj_add_style(battery_lbl, &battery_style, 0);
    lv_label_set_text(battery_lbl, str_clock);
    lv_obj_align(battery_lbl, LV_ALIGN_LEFT_MID, 0, 0);
}

uint32_t _setWeatherIconImg(char icon[4], lv_obj_t *icon_img)
{
    uint32_t icon_width = 0;
    if (strcmp(icon, "01d") == 0)
    {
        lv_img_set_src(icon_img, &w01d);
        icon_width = w01d.header.w;
    }
    else if (strcmp(icon, "01n") == 0)
    {
        lv_img_set_src(icon_img, &w01n);
        icon_width = w01n.header.w;
    }
    else if (strcmp(icon, "02d") == 0)
    {
        lv_img_set_src(icon_img, &w02d);
        icon_width = w02d.header.w;
    }
    else if (strcmp(icon, "02n") == 0)
    {
        lv_img_set_src(icon_img, &w02n);
        icon_width = w02n.header.w;
    }
    else if (strcmp(icon, "03n") == 0)
    {
        lv_img_set_src(icon_img, &w03n);
        icon_width = w03n.header.w;
    }
    else if (strcmp(icon, "04n") == 0)
    {
        lv_img_set_src(icon_img, &w04n);
        icon_width = w04n.header.w;
    }
    else if (strcmp(icon, "09n") == 0)
    {
        lv_img_set_src(icon_img, &w09n);
        icon_width = w09n.header.w;
    }
    else if (strcmp(icon, "10d") == 0)
    {
        lv_img_set_src(icon_img, &w10d);
        icon_width = w10d.header.w;
    }
    else if (strcmp(icon, "10n") == 0)
    {
        lv_img_set_src(icon_img, &w10n);
        icon_width = w10n.header.w;
    }
    else if (strcmp(icon, "11n") == 0)
    {
        lv_img_set_src(&icon_img, &w11n);
        icon_width = w11n.header.w;
    }
    return icon_width;
}

void _setWeatherBox(void)
{
    lv_obj_t *weather_icon_frame;
    // Weather icon frame
    weather_icon_frame = lv_obj_create(home_page);
    lv_obj_set_scrollbar_mode(weather_icon_frame, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(weather_icon_frame, WEATHER_CONTAINER_W, WEATHER_CONTAINER_H);
    lv_obj_align(weather_icon_frame, LV_ALIGN_BOTTOM_LEFT, 0, -WC_MARGIN_OFFSET);
    lv_style_init(&weather_frame_style);
    lv_style_set_pad_all(&weather_frame_style, 5);
    lv_style_set_radius(&weather_frame_style, 7);
    lv_style_set_bg_color(&weather_frame_style, CL_SEA_SALT);
    lv_style_set_border_width(&weather_frame_style, 0);
    lv_obj_add_style(weather_icon_frame, &weather_frame_style, 0);
    // create wether img
    weather_icon_img = lv_img_create(weather_icon_frame);
    weather_icon_img_rpt = lv_img_create(weather_icon_frame);
    // Temp label
    lv_style_init(&weather_style);
    lv_style_set_text_color(&weather_style, CL_STEEL_BLUE);
    lv_style_set_text_font(&weather_style, &lv_font_montserrat_18);
    weather_lbl = lv_label_create(weather_icon_frame);
    lv_obj_add_style(weather_lbl, &weather_style, 0);
    lv_obj_align(weather_lbl, LV_ALIGN_TOP_MID, 0, 0);
    // TODAY label
    lv_obj_t *today_lbl = lv_label_create(weather_icon_frame);
    lv_obj_add_style(today_lbl, &weather_style, 0);
    lv_obj_align(today_lbl, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_label_set_text(today_lbl, "Today");
}

void _set5DaysForecastBox(void)
{
    // Create Outer Box element
    static lv_style_t outer_box_bg_style;
    lv_obj_t *forecast_outer_box = lv_obj_create(home_page);
    lv_obj_set_scrollbar_mode(forecast_outer_box, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(forecast_outer_box, DAY_FORECAST_OUT_BOX_W, DAY_FORECAST_OUT_BOX_H);
    lv_obj_align(forecast_outer_box, LV_ALIGN_BOTTOM_RIGHT, 0, -WC_MARGIN_OFFSET);
    lv_style_init(&outer_box_bg_style);
    lv_style_set_radius(&outer_box_bg_style, 0);
    lv_style_set_border_width(&outer_box_bg_style, 0);
    lv_style_set_bg_color(&outer_box_bg_style, bckg_color);
    lv_style_set_pad_all(&outer_box_bg_style, WC_MARGIN_OFFSET);
    lv_obj_add_style(forecast_outer_box, &outer_box_bg_style, 0);

    // Create 5 weather forecast boxes
    static lv_style_t box_day_style;
    lv_style_init(&box_day_style);
    lv_style_set_radius(&box_day_style, 7);
    lv_style_set_border_width(&box_day_style, 0);
    lv_style_set_bg_color(&box_day_style, CL_SEA_SALT);
    // create text style
    static lv_style_t box_day_txt_style;
    lv_style_init(&box_day_txt_style);
    lv_style_set_text_color(&box_day_txt_style, CL_STEEL_BLUE);
    lv_style_set_text_font(&box_day_txt_style, &lv_font_montserrat_14);
    int box_pos_y;
    for (int i = 0; i < 5; i++)
    {
        // lv_obj_t *day_wk = NULL;
        box_pos_y = i * (DAY_FORECAST_BOX_H + WEATHER_CONTAINER_MARGIN) - WC_MARGIN_OFFSET;
        lv_obj_t *forecast_box = lv_obj_create(forecast_outer_box);
        lv_obj_set_scrollbar_mode(forecast_box, LV_SCROLLBAR_MODE_OFF);
        lv_obj_align(forecast_box, LV_ALIGN_TOP_MID, 0, box_pos_y);
        lv_obj_set_size(forecast_box, DAY_FORECAST_BOX_W, DAY_FORECAST_BOX_H);
        lv_style_set_pad_all(&outer_box_bg_style, WC_MARGIN_OFFSET);
        lv_obj_add_style(forecast_box, &box_day_style, 0);

        lv_obj_t *day_wk = lv_label_create(forecast_box);
        lv_obj_t *day_min = lv_label_create(forecast_box);
        lv_obj_t *day_max = lv_label_create(forecast_box);
        // Day of the week
        lv_obj_add_style(day_wk, &box_day_txt_style, 0);
        lv_obj_align(day_wk, LV_ALIGN_LEFT_MID, 0, 0);
        // min temp
        lv_obj_align(day_min, LV_ALIGN_LEFT_MID, 100, 0);
        lv_obj_add_style(day_min, &box_day_txt_style, 0);
        // max temp
        lv_obj_add_style(day_max, &box_day_txt_style, 0);
        lv_obj_align(day_max, LV_ALIGN_LEFT_MID, 150, 0);
        switch (i)
        {
        case (0):
            day1_day_wk = day_wk;
            day1_min = day_min;
            day1_max = day_max;

        case (1):
            day2_day_wk = day_wk;
            day2_min = day_min;
            day2_max = day_max;

        case (2):
            day3_day_wk = day_wk;
            day3_min = day_min;
            day3_max = day_max;

        case (3):
            day4_day_wk = day_wk;
            day4_min = day_min;
            day4_max = day_max;

        case (4):
            day5_day_wk = day_wk;
            day5_min = day_min;
            day5_max = day_max;
        }
    }
}