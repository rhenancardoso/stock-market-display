// SNTP

#include <time.h>
#include <sys/time.h>
#include "esp_attr.h"
#include "esp_sleep.h"
#include "esp_sntp.h"

// wifi
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#define CITY_TZ "Melbourne"
#define CITY_UTC_TZ "UTC-10:00"

void time_sync_notification_cb(struct timeval *tv);
void get_SNTP_time(char *date_time);
static void initialize_sntp(void);
static void get_system_time(void);
void set_time(void);
