#include "../key_config.h"

#define API_URL "http://api.openweathermap.org/data/2.5/weather?lat=%s&lon=%s&appid=%s&units=metric"
#define CITY_LAT MELB_LAT
#define CITY_LON MELB_LON
#define API_KEY WEATHER_API_KEY

void getTemperature(void);
typedef struct Weather
{
    float temp;
    uint8_t humidity;
    uint8_t cloud_pct;
    float min_temp;
    float max_temp;
    bool is_data_collected;
};