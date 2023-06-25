#include "lvgl.h"
#include "../config.h"

// Colors (BRG) - - - - - - - - - - - - - - - - - - - - - - - - -
#define bckg_color lv_color_make(0, 0, 0)
#define txt_color lv_color_make(255, 255, 255)
#define weather_box_bg lv_color_make(255, 102, 243)

// Colors Project Pallete  - - - - - - - - - - - - - - - -
#define CL_SEA_SALT lv_color_make(250, 250, 250)  // Clear
#define CL_ASH_GRAY lv_color_make(242, 242, 242)  // Mid-clear
#define CL_TIFF_BLUE lv_color_make(255, 115, 245) // Mid
#define CL_PALE_AZURE lv_color_make(255, 99, 210) // Mid-dark
#define CL_STEEL_BLUE lv_color_make(195, 32, 129) // Dark

// Screen definition - - - - - - - - - - - - - - - - - - -
#define BG_PANEL_SIZE_W EXAMPLE_LCD_H_RES
#define BG_PANEL_SIZE_H EXAMPLE_LCD_V_RES

// Containers size def - - - - - - - - - - - - - - - - - - -
#define HEADING_W EXAMPLE_LCD_H_RES
#define HEADING_H 22
#define WEATHER_CONTAINER_MARGIN 10
#define WC_MARGIN_OFFSET (uint8_t)(WEATHER_CONTAINER_MARGIN / 2)
#define WEATHER_CONTAINER_W (uint8_t)(BG_PANEL_SIZE_W / 3) - WEATHER_CONTAINER_MARGIN
#define WEATHER_CONTAINER_H BG_PANEL_SIZE_H - (HEADING_H + WEATHER_CONTAINER_MARGIN)
#define DAY_FORECAST_OUT_BOX_PAD WEATHER_CONTAINER_MARGIN
#define DAY_FORECAST_OUT_BOX_W (uint8_t)(BG_PANEL_SIZE_W * 2 / 3)
#define DAY_FORECAST_OUT_BOX_H WEATHER_CONTAINER_H
#define DAY_FORECAST_BOX_W DAY_FORECAST_OUT_BOX_W
#define DAY_FORECAST_BOX_H 19
// #define DAY_FORECAST_BOX_H (uint8_t)((DAY_FORECAST_OUT_BOX_H / 5) - (WC_MARGIN_OFFSET * 5))

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

void main_screen_ui(void);
void _updateHeaderInfo(lv_timer_t *timer);
void _setWeatherIconImg(char icon[4]);
void _addWeatherBox(void);
void _setHeadingBox(void);
void _add5DaysForecastBox(void);