#include <stdio.h>
#include <time.h>
#include <string.h>
#include "esp_log.h"
#include "../../utils/wifi.h"
#include "../../utils/battery.h"
#include "heading_view.h"

static const char *TAG = "heading_view";

extern void UpdateHeading(lv_timer_t *timer);

static lv_style_t time_style;
static lv_style_t weather_style;
static lv_style_t battery_style;
lv_obj_t *time_lbl;
lv_obj_t *battery_lbl;
char str_clock[9];
char str_battery[15];
uint16_t battery_pct;

void UpdateHeading(lv_timer_t *timer)
{
    ESP_LOGD(TAG, "Update main page: Heading");

    // Get time
    time_t now;
    struct tm timeinfo;
    ESP_LOGD(TAG, "Get system time");
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(str_clock, sizeof(str_clock), "  %H:%M", &timeinfo);
    ESP_LOGD(TAG, "Local time is: %s", str_clock);
    char full_str_CLK[11] = "";
    if (wifi_conn.is_connected)
    {
        ESP_LOGD(TAG, "Wifi connected, copy wifi symbol to clock string");
        strcpy(full_str_CLK, LV_SYMBOL_WIFI);
        strcat(full_str_CLK, str_clock);
    }
    else
    {
        ESP_LOGD(TAG, "Wifi not connected, clock string without wifi icon");
        strcpy(full_str_CLK, str_clock);
    }
    ESP_LOGD(TAG, "Set time label in heading");
    lv_label_set_text(time_lbl, full_str_CLK);

    // Battery
    ESP_LOGD(TAG, "Set battery icon in heading");
    if (battery_pct > BATTERY_FULL)
        sprintf(str_battery, "%s %d", LV_SYMBOL_BATTERY_FULL, battery_pct);
    else if (battery_pct > BATTERY_3)
        sprintf(str_battery, "%s %d", LV_SYMBOL_BATTERY_3, battery_pct);
    else if (battery_pct > BATTERY_2)
        sprintf(str_battery, "%s %d", LV_SYMBOL_BATTERY_2, battery_pct);
    else if (battery_pct > BATTERY_1)
        sprintf(str_battery, "%s %d", LV_SYMBOL_BATTERY_1, battery_pct);
    else
        sprintf(str_battery, "%s %d", LV_SYMBOL_BATTERY_EMPTY, battery_pct);

    lv_label_set_text(battery_lbl, str_battery);
}

void SetHeadingBox(lv_obj_t *screen)
{

    ESP_LOGI(TAG, "Setting heading container");
    lv_obj_t *heading_container;
    static lv_style_t heading_container_style;

    // Create Heading container
    heading_container = lv_obj_create(screen);
    lv_obj_set_scrollbar_mode(heading_container, LV_SCROLLBAR_MODE_OFF);
    lv_style_init(&heading_container_style);
    lv_style_set_radius(&heading_container_style, 0);
    lv_style_set_border_width(&heading_container_style, 0);
    lv_style_set_bg_color(&heading_container_style, CL_STEEL_BLUE);
    lv_obj_set_size(heading_container, HEADING_W, HEADING_H);
    lv_obj_add_style(heading_container, &heading_container_style, 0);
    // Create hour label object and configure
    lv_style_init(&time_style);
    lv_style_set_text_color(&time_style, white_color);
    lv_style_set_text_font(&time_style, &lv_font_montserrat_16);
    time_lbl = lv_label_create(heading_container);
    lv_obj_add_style(time_lbl, &time_style, 0);
    lv_label_set_text(time_lbl, str_clock);
    lv_obj_align(time_lbl, LV_ALIGN_RIGHT_MID, 0, 0);
    // Create temp label object and configure
    lv_style_init(&battery_style);
    lv_style_set_text_color(&battery_style, white_color);
    lv_style_set_text_font(&battery_style, &lv_font_montserrat_16);
    battery_lbl = lv_label_create(heading_container);
    lv_obj_add_style(battery_lbl, &battery_style, 0);
    lv_label_set_text(battery_lbl, str_clock);
    lv_obj_align(battery_lbl, LV_ALIGN_LEFT_MID, 0, 0);

    ESP_LOGI(TAG, "Heading container initialised!");
}