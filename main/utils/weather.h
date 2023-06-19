#include "../key_config.h"

#define MELB_LAT "-37.8142176"
#define MELB_LON "144.9631608"
#define API_KEY WEATHER_API_KEY

typedef struct Weather
{
    float temp;
    uint8_t humidity;
} weather;

void getTemperature(weather *w);