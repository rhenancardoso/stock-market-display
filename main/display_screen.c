#include <stdio.h>
#include <time.h>
#include "lvgl.h"
#include "config.h"
#include "esp_log.h"

#define bg_color lv_color_make(2,2,2)
#define txt_color lv_color_make(250,250,250)

#define OFFSET_SIZE 10
#define OFFSET_OBJ (int8_t)OFFSET_SIZE/2
#define BG_PANEL_SIZE_W EXAMPLE_LCD_H_RES+OFFSET_SIZE
#define BG_PANEL_SIZE_H EXAMPLE_LCD_V_RES+OFFSET_SIZE


static lv_style_t time_style;
static lv_style_t no_border_style;

lv_obj_t *home_page;
lv_obj_t *home_page_bg;
lv_obj_t *emergency_button;
lv_obj_t *time_lbl;

void display_ui(lv_disp_t*disp)
{ 
    //Get time
	time_t now;
    char str_clock[8];
    struct tm timeinfo;

    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(str_clock, sizeof(str_clock), "  %H:%M ", &timeinfo);
    ESP_LOGI("DISPLAY_UI", "Time is: %s", str_clock);

    // Create home page
    home_page = lv_obj_create(NULL);
    lv_obj_set_scrollbar_mode(home_page, LV_SCROLLBAR_MODE_OFF);
    // Create Background object and style
    lv_style_init(&no_border_style);
    lv_style_set_bg_color(&no_border_style, bg_color);
    home_page_bg = lv_obj_create(home_page);
    lv_obj_set_size(home_page_bg, BG_PANEL_SIZE_W, BG_PANEL_SIZE_H);
    lv_obj_align(home_page_bg, LV_ALIGN_TOP_LEFT, -OFFSET_OBJ,-OFFSET_OBJ);                                       // this is to ensure that there is no other background colors peek through
    lv_obj_add_style(home_page_bg, &no_border_style, 0);
    // Create hour label object and configure
    lv_style_init(&time_style);
    lv_style_set_text_color(&time_style,txt_color);
    time_lbl = lv_label_create(home_page);
    lv_obj_add_style(time_lbl, &time_style, 0);
    lv_label_set_text(time_lbl, strcat(LV_SYMBOL_WIFI,str_clock));
    lv_obj_align(time_lbl, LV_ALIGN_TOP_RIGHT, -OFFSET_OBJ, OFFSET_OBJ);

    // Update screen
    lv_scr_load(home_page);
}
