#ifndef USER_H
#define USER_H

#define USER_SETUP_LOADED
#define WOKWI false

#if WOKWI != true // If not Wokwi

#undef TFT_MOSI
#undef TFT_MISO
#undef TFT_SCLK
#undef TFT_CS
#undef TFT_DC


#undef ILI9341_DRIVER
#define GC9A01_DRIVER

#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASS "WIFI_PASS"

#define BUTTON_OK 39
#define BUTTON_LEFT 25
#define BUTTON_RIGHT 34

#define TFT_MOSI 23
#define TFT_MISO -1
#define TFT_SCLK 18
#define TFT_CS 1
#define TFT_DC 22
#define TFT_RST 21

#define SCREEN_1_CS 15
#define SCREEN_2_CS 2
#define SCREEN_3_CS 27
#define SCREEN_4_CS 4
#define SCREEN_5_CS 5

#define DISPLAY_ROTATION 0

#else

#undef GC9A01_DRIVER
#define ILI9341_DRIVER
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASS ""

#define BUTTON_OK 39
#define BUTTON_LEFT 25
#define BUTTON_RIGHT 34

#define TFT_MOSI 17
// #define TFT_MISO -1
#define TFT_SCLK 23
#define TFT_CS 1
#define TFT_DC 19
#define TFT_RST 18

#define SCREEN_1_CS 13
#define SCREEN_2_CS 33
#define SCREEN_3_CS 32
#define SCREEN_4_CS 25
#define SCREEN_5_CS 21

#define DISPLAY_ROTATION 2

#endif

#define NTP_SERVER "pool.ntp.org"


#define SHADOWING 1

#define SCREEN_SIZE 240

#define TIME_ZONE_OFFSET 2
#define FORMAT_24_HOUR true


#define BG_COLOR 0x20a1         // clock shadow colour(Light brown)
#define LIGHT_ORANGE 0xfc80  // orange for clock

#endif