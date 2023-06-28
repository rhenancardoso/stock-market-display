#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "esp_log.h"

static const char *TAG = "utils";

/**
 * From 3 leter acronym for day of the week, return matching integer
 * @param week_day       day of the week string (3 leters, eg. MON)
 * @return              (uint8_t) matching integer for day of the week
 * 1-MON, 2-TUE, 3-WED, 4-THU, 5-FRI, 6-SAT, 7-SUN
 */
uint8_t convertDaytoInt(char *week_day)
{
    ESP_LOGI(TAG, "Convert str Day into Int");
    char *upper_case_str = week_day;
    for (int x = 0; x < strlen(upper_case_str); x++)
    {
        upper_case_str[x] = toupper(week_day[x]);
    }
    ESP_LOGI(TAG, "Day of week uppercase: %s", upper_case_str);

    if (strcmp(upper_case_str, "MON") == 0)
    {
        return 1;
    }
    else if (strcmp(upper_case_str, "TUE") == 0)
    {
        return 2;
    }
    else if (strcmp(upper_case_str, "WED") == 0)
    {
        return 3;
    }
    else if (strcmp(upper_case_str, "THU") == 0)
    {
        return 4;
    }
    else if (strcmp(upper_case_str, "FRI") == 0)
    {
        return 5;
    }
    else if (strcmp(upper_case_str, "SAT") == 0)
    {
        return 6;
    }
    else if (strcmp(upper_case_str, "SUN") == 0)
    {
        return 7;
    }

    return 0;
}

/**
 * From day of the week number return matching 3 leter day of the week
 * @param week_day       day of the week integer (eg. MON -> 1)
 * @return              (char[3]) day of week string
 * 1-MON, 2-TUE, 3-WED, 4-THU, 5-FRI, 6-SAT, 7-SUN
 */
char *convertInttoStr(uint8_t week_day)
{
    switch (week_day)
    {
    case (1):
        return "MON";
    case (2):
        return "TUE";
    case (3):
        return "WED";
    case (4):
        return "THU";
    case (5):
        return "FRI";
    case (6):
        return "SAT";
    case (7):
        return "SUN";
    }
    return NULL;
}