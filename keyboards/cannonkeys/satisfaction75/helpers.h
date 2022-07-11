#pragma once

#include "satisfaction75.h"
#include <stdio.h>

#define min(x, y) (((x) >= (y)) ? (y) : (x))
#define max(x, y) (((x) >= (y)) ? (x) : (y))
#define wpm() get_current_wpm()
#define PRE_SLEEP_TIMEOUT (get_timeout() - 30000)

bool is_24hr_time(void) { return date_time_format_mode == 1 || date_time_format_mode == 3; }
bool is_month_first_date(void) { return date_time_format_mode == 2 || date_time_format_mode == 3; }
uint8_t get_frame_duration(void) { return max(200 - (wpm() * 0.65), 75); }

uint32_t get_timeout(void) {
    switch (timeout_mode) {
        default:
        case 0:
            return 90000;  // 1m30s
        case 1:
            return 120000;  // 2m
        case 2:
            return 300000;  // 5m
        case 3:
            return 60000;  // 1m
    }
}

char* get_enc_mode(void) {
    switch (encoder_mode) {
        default:
        case ENC_MODE_VOLUME:
            return "VOL";
        case ENC_MODE_MEDIA:
            return "MED";
        case ENC_MODE_SCROLL:
            return "SCR";
        case ENC_MODE_BRIGHTNESS:
            return "BRT";
        case ENC_MODE_BACKLIGHT:
            return "BKL";
        case ENC_MODE_CLOCK_SET:
            return "CLK";
        case ENC_MODE_CUSTOM0:
            return "CS0";
        case ENC_MODE_CUSTOM1:
            return "CS1";
        case ENC_MODE_CUSTOM2:
            return "CS2";
    }
}

char* get_time(void) {
    uint8_t  hour   = last_minute / 60;
    uint16_t minute = last_minute % 60;

    if (encoder_mode == ENC_MODE_CLOCK_SET) {
        hour   = hour_config;
        minute = minute_config;
    }

    bool is_pm = (hour / 12) > 0;
    if (!is_24hr_time()) {
        hour = hour % 12;
        if (hour == 0) {
            hour = 12;
        }
    }

    static char time_str[11] = "";
    if (!is_24hr_time()) {
        sprintf(time_str, "%02d:%02d%s", hour, minute, is_pm ? "pm" : "am");
    } else {
        sprintf(time_str, "%02d:%02d", hour, minute);
    }

    return time_str;
}

char* get_date(bool show_full) {
    int16_t year  = last_timespec.year + 1980;
    int8_t  month = last_timespec.month;
    int8_t  day   = last_timespec.day;

    if (encoder_mode == ENC_MODE_CLOCK_SET) {
        year  = year_config + 1980;
        month = month_config;
        day   = day_config;
    }

    static char date_str[15] = "";
    if (show_full) {
        if (!is_month_first_date()) {
            sprintf(date_str, "%02d/%02d/%04d", day, month, year);
        } else {
            sprintf(date_str, "%02d/%02d/%04d", month, day, year);
        }
    } else {
        if (!is_month_first_date()) {
            sprintf(date_str, "%02d/%02d/%02d", day, month, year % 100);
        } else {
            sprintf(date_str, "%02d/%02d/%02d", month, day, year % 100);
        }
    }

    return date_str;
}

