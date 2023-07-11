#include "lvgl.h"
#include "display_config.h"

#define TIMER_PERIOD (uint8_t)50
#define FORECAST_TICK_CNT (uint32_t)3600000 / TIMER_PERIOD
#define ZERO_TICK 0b0
#define FIRST_TICK 0b1

// Containers size def - - - - - - - - - - - - - - - - - - -
#define HEADING_W EXAMPLE_LCD_H_RES
#define HEADING_H 22
#define WEATHER_CONTAINER_MARGIN 5
#define WC_MARGIN_OFFSET (uint8_t)(WEATHER_CONTAINER_MARGIN / 2)
#define WEATHER_CONTAINER_W (uint8_t)(BG_PANEL_SIZE_W / 3) - WEATHER_CONTAINER_MARGIN
#define WEATHER_CONTAINER_H BG_PANEL_SIZE_H - (HEADING_H + WEATHER_CONTAINER_MARGIN)
#define DAY_FORECAST_OUT_BOX_PAD WEATHER_CONTAINER_MARGIN
#define DAY_FORECAST_OUT_BOX_W (uint8_t)(BG_PANEL_SIZE_W * 2 / 3)
#define DAY_FORECAST_OUT_BOX_H WEATHER_CONTAINER_H
#define DAY_FORECAST_BOX_W DAY_FORECAST_OUT_BOX_W
#define DAY_FORECAST_BOX_H 24
// #define DAY_FORECAST_BOX_H (uint8_t)((DAY_FORECAST_OUT_BOX_H / 5) - (WC_MARGIN_OFFSET * 5))

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
/**
 * Update weather 5 days forecast information to the main screen page.
 */
void _update5DaysForecast(void);