#include "lvgl.h"
#include "display_config.h"
#include "esp_log.h"

#ifndef _STOCK_SCREEN_H_
#define _STOCK_SCREEN_H_
#define PAGE_TIMER_UPDATE_TICK 500

// Containers size def - - - - - - - - - - - - - - - - - - -
#define MAX_BOXES_PER_PAGE 5
#define HEADING_W EXAMPLE_LCD_H_RES
#define HEADING_H 22
#define STOCK_CONTAINER_MARGIN 5
#define WC_MARGIN_OFFSET (uint8_t)(STOCK_CONTAINER_MARGIN / 2)
#define STOCK_CONTAINER_W HEADING_W - STOCK_CONTAINER_MARGIN
#define STOCK_CONTAINER_H BG_PANEL_SIZE_H - (HEADING_H + STOCK_CONTAINER_MARGIN)
#define STOCK_BOX_PAD WEATHER_CONTAINER_MARGIN
#define STOCK_BOX_H 24

#endif

extern lv_obj_t *stock_page;

/**
 * @brief
 * Configure objects for the stock screen page.
 */
void stock_screen_ui(void);
/**
 * @brief
 * Update data to labels in the stock screen page
 * - Header: time label, battery icon
 * - Body: stock portfolio data
 * @param timer       pointer to an LVGL timer object
 */
void _updateStockData(lv_timer_t *timer);
/**
 * @brief
 * Create 5 stock box containers and add to the stock screen page.
 */
void _createStockBoxes(void);