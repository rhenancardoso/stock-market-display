#include "lvgl.h"
#include "display_config.h"

#define TIMER_PERIOD (uint8_t)10

/**
 * Configure objects for the wifi connection screen page.
 */
void wifi_conn_screen_ui(void);
/**
 * Update objects in the wifi connection screen page
 * @param timer       pointer to an LVGL timer object
 */
void _updateWifiPage(lv_timer_t *timer);