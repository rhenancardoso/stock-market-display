#include "home_screen.h"
#include "../utils/wifi.h"
#include "../utils/weather.h"
#include "shared/heading_view.h"

static const char *TAG = "home_page";

extern char *convertInttoStr(uint8_t week_day);

static lv_style_t weather_frame_style;
static lv_style_t day_forecast_style;
static lv_style_t weather_style;
lv_obj_t *home_page;
lv_obj_t *weather_lbl;
lv_obj_t *weather_icon_img;
lv_obj_t *weather_icon_img_rpt;
lv_obj_t *day1_day_wk;
lv_obj_t *day1_icon;
lv_obj_t *day1_min;
lv_obj_t *day1_max;
lv_obj_t *day2_day_wk;
lv_obj_t *day2_icon;
lv_obj_t *day2_min;
lv_obj_t *day2_max;
lv_obj_t *day3_day_wk;
lv_obj_t *day3_icon;
lv_obj_t *day3_min;
lv_obj_t *day3_max;
lv_obj_t *day4_day_wk;
lv_obj_t *day4_icon;
lv_obj_t *day4_min;
lv_obj_t *day4_max;
lv_obj_t *day5_day_wk;
lv_obj_t *day5_icon;
lv_obj_t *day5_min;
lv_obj_t *day5_max;
lv_obj_t *day_wk;
lv_obj_t *day_min;
lv_obj_t *day_max;
lv_obj_t *day_icon;
lv_obj_t *forecast_box;

lv_timer_t *timer_update_weather_box;
lv_timer_t *timer_forecast_containers;
char str_weather[5];
uint8_t icon_move_x = 0;
uint32_t tick_forecast_cnt = 0;
uint32_t w_icon_width = 0;
uint32_t f_icon_width = 0;

void main_screen_ui(void)
{
    ESP_LOGI(TAG, "setup screen");
    static lv_style_t screen_bg_style;
    lv_obj_t *home_page_bg;

    ESP_LOGD(TAG, "Create home_page LVGL object");
    home_page = lv_obj_create(NULL);
    // Create home page
    lv_obj_set_scrollbar_mode(home_page, LV_SCROLLBAR_MODE_OFF);
    // Create Background object and style
    ESP_LOGD(TAG, "Initiliaze screen_bg_style");
    lv_style_init(&screen_bg_style);
    lv_style_set_bg_color(&screen_bg_style, black_color);
    lv_style_set_radius(&screen_bg_style, 0);
    lv_style_set_border_width(&screen_bg_style, 0);
    ESP_LOGD(TAG, "Create home_page_bg LVGL object");
    home_page_bg = lv_obj_create(home_page);
    lv_obj_set_size(home_page_bg, BG_PANEL_SIZE_W, BG_PANEL_SIZE_H);
    lv_obj_align(home_page_bg, LV_ALIGN_TOP_LEFT, 0, 0); // this is to ensure that there is no other background colors peek through
    lv_obj_add_style(home_page_bg, &screen_bg_style, 0);

    // Add heading
    ESP_LOGI(TAG, "Home Screen calling setHeadingBox function");
    setHeadingBox(home_page);
    // Add weather container
    _setWeatherBox();
    // Add weather forecast boxes
    _set5DaysForecastBox();

    ESP_LOGD(TAG, "Create 'timer_update_weather_box' timer with %dms period", TIMER_PERIOD);
    timer_update_weather_box = lv_timer_create(_updateMainPage, TIMER_PERIOD, NULL);
    timer_update_weather_box->repeat_count = -1;
    ESP_LOGD(TAG, "Create 'timer_forecast_containers' timer with %dms period", INITIAL_5DAYS_TIMER_MS);
    timer_forecast_containers = lv_timer_create(_update5DaysForecast, INITIAL_5DAYS_TIMER_MS, NULL);
    timer_forecast_containers->repeat_count = -1;
}

void _updateMainPage(lv_timer_t *timer)
{
    updateHeading(timer);
    // Weather
    if (w_melb.is_data_collected)
    {
        ESP_LOGD(TAG, "Weather data has been collected, updating weather box display");
        // Update Todays weather temperature
        sprintf(str_weather, "%.0f°C", w_melb.temp);
        lv_label_set_text(weather_lbl, str_weather);
        // Set weather icon
        icon_move_x++;
        w_icon_width = getWeatherIconWidth(w_melb.icon);
        setWeatherIconImg(w_melb.icon, weather_icon_img);
        lv_obj_align(weather_icon_img, LV_ALIGN_LEFT_MID, icon_move_x - w_icon_width, 2);
        w_icon_width = getWeatherIconWidth(w_melb.icon);
        setWeatherIconImg(w_melb.icon, weather_icon_img_rpt);
        lv_obj_align(weather_icon_img_rpt, LV_ALIGN_LEFT_MID, icon_move_x, 2);
        if (icon_move_x > w_icon_width)
        {
            icon_move_x = 0;
        }
    }
    else
    {
        ESP_LOGD(TAG, "Weather data not collected, diplaying --°C");
        lv_label_set_text(weather_lbl, "--°C");
    }
}

void _update5DaysForecast(void)
{
    // Weather
    if (weeklyForecast[0].is_data_collected)
    {
        ESP_LOGD(TAG, "Update weather info in forecast containers");
        for (uint8_t i = 1; i < DAYS_FORECAST; i++)
        {
            char temp_min[6] = "";
            char temp_max[6] = "";
            char week_day[4] = "";
            sprintf(temp_min, "%.0f°C", weeklyForecast[i].min_temp);
            sprintf(temp_max, "%.0f°C", weeklyForecast[i].max_temp);
            sprintf(week_day, "%s", convertInttoStr(weeklyForecast[i].week_day));
            int icon_code = weeklyForecast[i].icon_code;
            ESP_LOGI(TAG, "Update %d container: Week Day: %s/ Icon-code: %d / Max Temp: %s/ Min Temp: %s", i, week_day, icon_code, temp_min, temp_max);
            switch (i)
            {
            case (1):
                setWeeklyForecastIconImg(icon_code, day1_icon);
                lv_label_set_text(day1_day_wk, week_day);
                lv_label_set_text(day1_min, temp_min);
                lv_label_set_text(day1_max, temp_max);
            case (2):
                setWeeklyForecastIconImg(icon_code, day2_icon);
                lv_label_set_text(day2_day_wk, week_day);
                lv_label_set_text(day2_min, temp_min);
                lv_label_set_text(day2_max, temp_max);
            case (3):
                setWeeklyForecastIconImg(icon_code, day3_icon);
                lv_label_set_text(day3_day_wk, week_day);
                lv_label_set_text(day3_min, temp_min);
                lv_label_set_text(day3_max, temp_max);
            case (4):
                setWeeklyForecastIconImg(icon_code, day4_icon);
                lv_label_set_text(day4_day_wk, week_day);
                lv_label_set_text(day4_min, temp_min);
                lv_label_set_text(day4_max, temp_max);
            case (5):
                setWeeklyForecastIconImg(icon_code, day5_icon);
                lv_label_set_text(day5_day_wk, week_day);
                lv_label_set_text(day5_min, temp_min);
                lv_label_set_text(day5_max, temp_max);
            }
        }
        // Update period after first screen initialisation
        if (timer_forecast_containers->period == TIMER_PERIOD + 10)
        {
            timer_forecast_containers->period = WEEKLY_UPDATE_MS;
        }
    }
    else
    {
        ESP_LOGD(TAG, "Data not collected");
        lv_label_set_text(day1_day_wk, convertInttoStr(weeklyForecast[1].week_day));
        lv_label_set_text(day1_min, "--°C");
        lv_label_set_text(day1_max, "--°C");
        lv_label_set_text(day2_day_wk, convertInttoStr(weeklyForecast[2].week_day));
        lv_label_set_text(day2_min, "--°C");
        lv_label_set_text(day2_max, "--°C");
        lv_label_set_text(day3_day_wk, convertInttoStr(weeklyForecast[3].week_day));
        lv_label_set_text(day3_min, "--°C");
        lv_label_set_text(day3_max, "--°C");
        lv_label_set_text(day4_day_wk, convertInttoStr(weeklyForecast[4].week_day));
        lv_label_set_text(day4_min, "--°C");
        lv_label_set_text(day4_max, "--°C");
        lv_label_set_text(day5_day_wk, convertInttoStr(weeklyForecast[5].week_day));
        lv_label_set_text(day5_min, "--°C");
        lv_label_set_text(day5_max, "--°C");
    }
}

void _setWeatherBox(void)
{
    lv_obj_t *weather_icon_frame;
    // Weather icon frame
    weather_icon_frame = lv_obj_create(home_page);
    lv_obj_set_scrollbar_mode(weather_icon_frame, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(weather_icon_frame, WEATHER_CONTAINER_W, WEATHER_CONTAINER_H);
    lv_obj_align(weather_icon_frame, LV_ALIGN_BOTTOM_LEFT, 0, -WC_MARGIN_OFFSET);
    lv_style_init(&weather_frame_style);
    lv_style_set_pad_all(&weather_frame_style, 5);
    lv_style_set_radius(&weather_frame_style, 7);
    lv_style_set_bg_color(&weather_frame_style, CL_SEA_SALT);
    lv_style_set_border_width(&weather_frame_style, 0);
    lv_obj_add_style(weather_icon_frame, &weather_frame_style, 0);
    // create wether img
    weather_icon_img = lv_img_create(weather_icon_frame);
    weather_icon_img_rpt = lv_img_create(weather_icon_frame);
    // Temp label
    lv_style_init(&weather_style);
    lv_style_set_text_color(&weather_style, CL_STEEL_BLUE);
    lv_style_set_text_font(&weather_style, &lv_font_montserrat_20);
    weather_lbl = lv_label_create(weather_icon_frame);
    lv_obj_add_style(weather_lbl, &weather_style, 0);
    lv_obj_align(weather_lbl, LV_ALIGN_TOP_MID, 0, 0);
    // TODAY label
    lv_obj_t *today_lbl = lv_label_create(weather_icon_frame);
    lv_obj_add_style(today_lbl, &weather_style, 0);
    lv_obj_align(today_lbl, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_label_set_text(today_lbl, "Today");
}

void _set5DaysForecastBox(void)
{
    // Create Outer Box element
    static lv_style_t outer_box_bg_style;
    lv_obj_t *forecast_outer_box = lv_obj_create(home_page);
    lv_obj_set_scrollbar_mode(forecast_outer_box, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(forecast_outer_box, DAY_FORECAST_OUT_BOX_W, DAY_FORECAST_OUT_BOX_H);
    lv_obj_align(forecast_outer_box, LV_ALIGN_BOTTOM_RIGHT, 0, -WC_MARGIN_OFFSET);
    lv_style_init(&outer_box_bg_style);
    lv_style_set_radius(&outer_box_bg_style, 0);
    lv_style_set_border_width(&outer_box_bg_style, 0);
    lv_style_set_bg_color(&outer_box_bg_style, black_color);
    lv_style_set_pad_all(&outer_box_bg_style, WC_MARGIN_OFFSET);
    lv_obj_add_style(forecast_outer_box, &outer_box_bg_style, 0);

    // Create 5 weather forecast boxes
    static lv_style_t box_day_style;
    lv_style_init(&box_day_style);
    lv_style_set_radius(&box_day_style, 7);
    lv_style_set_border_width(&box_day_style, 0);
    lv_style_set_bg_color(&box_day_style, CL_SEA_SALT);

    // create text style
    static lv_style_t box_day_txt_style;
    lv_style_init(&box_day_txt_style);
    lv_style_set_text_color(&box_day_txt_style, CL_STEEL_BLUE);
    lv_style_set_text_font(&box_day_txt_style, &lv_font_montserrat_16);
    int box_pos_y;

    for (uint8_t i = 0; i < 5; i++)
    {
        box_pos_y = i * (DAY_FORECAST_BOX_H + WEATHER_CONTAINER_MARGIN) - WC_MARGIN_OFFSET;
        forecast_box = lv_obj_create(forecast_outer_box);
        lv_obj_set_scrollbar_mode(forecast_box, LV_SCROLLBAR_MODE_OFF);
        lv_obj_align(forecast_box, LV_ALIGN_TOP_MID, 0, box_pos_y);
        lv_obj_set_size(forecast_box, DAY_FORECAST_BOX_W, DAY_FORECAST_BOX_H);
        lv_style_set_pad_all(&outer_box_bg_style, WC_MARGIN_OFFSET);
        lv_obj_add_style(forecast_box, &box_day_style, 0);
        // local variables
        day_wk = lv_label_create(forecast_box);
        day_min = lv_label_create(forecast_box);
        day_max = lv_label_create(forecast_box);
        day_icon = lv_img_create(forecast_box);

        // Day of the week
        lv_obj_add_style(day_wk, &box_day_txt_style, 0);
        lv_obj_align(day_wk, LV_ALIGN_LEFT_MID, 0, 0);
        // Forecast Icon
        lv_obj_align(day_icon, LV_ALIGN_LEFT_MID, 20, 0);
        lv_img_set_zoom(day_icon, 100);
        //  min temp
        lv_obj_align(day_min, LV_ALIGN_LEFT_MID, 110, 0);
        lv_obj_add_style(day_min, &box_day_txt_style, 0);
        // max temp
        lv_obj_add_style(day_max, &box_day_txt_style, 0);
        lv_obj_align(day_max, LV_ALIGN_LEFT_MID, 160, 0);

        switch (i)
        {
        case (0):
            day1_day_wk = day_wk;
            day1_min = day_min;
            day1_max = day_max;
            day1_icon = day_icon;

        case (1):
            day2_day_wk = day_wk;
            day2_min = day_min;
            day2_max = day_max;
            day2_icon = day_icon;

        case (2):
            day3_day_wk = day_wk;
            day3_min = day_min;
            day3_max = day_max;
            day3_icon = day_icon;

        case (3):
            day4_day_wk = day_wk;
            day4_min = day_min;
            day4_max = day_max;
            day4_icon = day_icon;

        case (4):
            day5_day_wk = day_wk;
            day5_min = day_min;
            day5_max = day_max;
            day5_icon = day_icon;
        }
    }
}