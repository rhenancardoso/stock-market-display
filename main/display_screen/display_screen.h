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

/**
 * Configure objects for the main screen page.
 */
void main_screen_ui(void);
/**
 * Update objects in the main screen page
 * - Header: time label, battery icon
 * - Body: Today's weather forecast, and icon moving
 * - Body: 5 days forecast info
 * @param timer       pointer to an LVGL timer object
 */
void _updateMainPage(lv_timer_t *timer);
/**
 * Get the weather icon and set the icon_img object.
 * @param icon       icon string name to be searched
 * @param icon_img       pointer to the image object to be set
 * @return          icon width
 */
uint32_t _setWeatherIconImg(char icon[4], lv_obj_t *icon_img);
/**
 * Create weather box container and add to the main screen page.
 */
void _setWeatherBox(void);
/**
 * Create heading box container and add to the main screen page.
 */
void _setHeadingBox(void);
/**
 * Create 5 days box containers and add to the main screen page.
 */
void _set5DaysForecastBox(void);