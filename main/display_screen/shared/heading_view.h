#include "lvgl.h"
#include "../display_config.h"

// Containers size def - - - - - - - - - - - - - - - - - - -
#define HEADING_W EXAMPLE_LCD_H_RES
#define HEADING_H 22

/**
 * @brief
 * Update time model
 */
void updateHeading(lv_timer_t *timer);

/**
 * @brief
 * Set heading container
 */
void setHeadingBox(lv_obj_t *screen);
