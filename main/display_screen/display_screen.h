#include "lvgl.h"
#include "../config.h"

#define bckg_color lv_color_make(0, 0, 0)
#define txt_color lv_color_make(255, 255, 255)
#define weather_box_bg lv_color_make(255, 102, 243) // BRG

#define OFFSET_SIZE 10
#define OFFSET_OBJ (int8_t) OFFSET_SIZE / 2
#define BG_PANEL_SIZE_W EXAMPLE_LCD_H_RES + OFFSET_SIZE
#define BG_PANEL_SIZE_H EXAMPLE_LCD_V_RES + OFFSET_SIZE

LV_IMG_DECLARE(w01n);
LV_IMG_DECLARE(w01d);
LV_IMG_DECLARE(w02n);
LV_IMG_DECLARE(w02d);
LV_IMG_DECLARE(w03n);
LV_IMG_DECLARE(w04n);
LV_IMG_DECLARE(w09n);
LV_IMG_DECLARE(w10n);
LV_IMG_DECLARE(w10d);
LV_IMG_DECLARE(w11n);

void update_time(lv_timer_t *timer);
void main_screen_ui(void);
void setWeatherIconImg(char icon[4]);
void add_weather_container(void);