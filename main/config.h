/* WIFI CONFIG*/
#define ESP_WIFI_SSID               HOME_WIFI_SSID
#define ESP_WIFI_PASS               HOME_WIFI_PASS

/* LCD CONFIG */
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ   (6528000) //(10 * 1000 * 1000)
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL  1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL

// The pixel number in horizontal and vertical
#define EXAMPLE_LCD_H_RES            320
#define EXAMPLE_LCD_V_RES            170
#define LVGL_LCD_BUF_SIZE            (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES)

// Bit number used to represent command and parameter
#define LCD_CMD_BITS                 8
#define LCD_PARAM_BITS               8
#define EXAMPLE_LVGL_TICK_PERIOD_MS  2

/*ESP32S3*/
#define PIN_LCD_BL                   GPIO_NUM_38

#define PIN_LCD_D0                   39
#define PIN_LCD_D1                   40
#define PIN_LCD_D2                   41
#define PIN_LCD_D3                   42
#define PIN_LCD_D4                   45
#define PIN_LCD_D5                   46
#define PIN_LCD_D6                   47
#define PIN_LCD_D7                   48

#define PIN_POWER_ON                 15

#define PIN_LCD_RES                  5
#define PIN_LCD_CS                   6
#define PIN_LCD_DC                   7
#define PIN_LCD_WR                   GPIO_NUM_8
#define PIN_LCD_RD                   GPIO_NUM_9

#define PIN_BUTTON_1                 0
#define PIN_BUTTON_2                 14
#define PIN_BAT_VOLT                 4

#define PIN_IIC_SCL                  17
#define PIN_IIC_SDA                  18
