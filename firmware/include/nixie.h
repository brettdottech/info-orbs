#ifndef NIXIE_H
#define NIXIE_H

#include <Arduino.h>

// These symbols are generated from the files specified in platformio.ini under 'board_build.embed_files'
// See https://docs.platformio.org/en/latest/platforms/espressif32.html#embedding-binary-data for more info

// Choose either with or without holes here and remember to also adjust platformio.ini
// *************************************************************************************

// extern const byte nixie_0_start[] asm("_binary_images_ClockWidget_nixie_holes_0_jpg_start");
// extern const byte nixie_0_end[] asm("_binary_images_ClockWidget_nixie_holes_0_jpg_end");
// extern const byte nixie_1_start[] asm("_binary_images_ClockWidget_nixie_holes_1_jpg_start");
// extern const byte nixie_1_end[] asm("_binary_images_ClockWidget_nixie_holes_1_jpg_end");
// extern const byte nixie_2_start[] asm("_binary_images_ClockWidget_nixie_holes_2_jpg_start");
// extern const byte nixie_2_end[] asm("_binary_images_ClockWidget_nixie_holes_2_jpg_end");
// extern const byte nixie_3_start[] asm("_binary_images_ClockWidget_nixie_holes_3_jpg_start");
// extern const byte nixie_3_end[] asm("_binary_images_ClockWidget_nixie_holes_3_jpg_end");
// extern const byte nixie_4_start[] asm("_binary_images_ClockWidget_nixie_holes_4_jpg_start");
// extern const byte nixie_4_end[] asm("_binary_images_ClockWidget_nixie_holes_4_jpg_end");
// extern const byte nixie_5_start[] asm("_binary_images_ClockWidget_nixie_holes_5_jpg_start");
// extern const byte nixie_5_end[] asm("_binary_images_ClockWidget_nixie_holes_5_jpg_end");
// extern const byte nixie_6_start[] asm("_binary_images_ClockWidget_nixie_holes_6_jpg_start");
// extern const byte nixie_6_end[] asm("_binary_images_ClockWidget_nixie_holes_6_jpg_end");
// extern const byte nixie_7_start[] asm("_binary_images_ClockWidget_nixie_holes_7_jpg_start");
// extern const byte nixie_7_end[] asm("_binary_images_ClockWidget_nixie_holes_7_jpg_end");
// extern const byte nixie_8_start[] asm("_binary_images_ClockWidget_nixie_holes_8_jpg_start");
// extern const byte nixie_8_end[] asm("_binary_images_ClockWidget_nixie_holes_8_jpg_end");
// extern const byte nixie_9_start[] asm("_binary_images_ClockWidget_nixie_holes_9_jpg_start");
// extern const byte nixie_9_end[] asm("_binary_images_ClockWidget_nixie_holes_9_jpg_end");
// extern const byte nixie_colon_off_start[] asm("_binary_images_ClockWidget_nixie_holes_colon_off_jpg_start");
// extern const byte nixie_colon_off_end[] asm("_binary_images_ClockWidget_nixie_holes_colon_off_jpg_end");
// extern const byte nixie_colon_on_start[] asm("_binary_images_ClockWidget_nixie_holes_colon_on_jpg_start");
// extern const byte nixie_colon_on_end[] asm("_binary_images_ClockWidget_nixie_holes_colon_on_jpg_end");

extern const byte nixie_0_start[] asm("_binary_images_ClockWidget_nixie_no_holes_0_jpg_start");
extern const byte nixie_0_end[] asm("_binary_images_ClockWidget_nixie_no_holes_0_jpg_end");
extern const byte nixie_1_start[] asm("_binary_images_ClockWidget_nixie_no_holes_1_jpg_start");
extern const byte nixie_1_end[] asm("_binary_images_ClockWidget_nixie_no_holes_1_jpg_end");
extern const byte nixie_2_start[] asm("_binary_images_ClockWidget_nixie_no_holes_2_jpg_start");
extern const byte nixie_2_end[] asm("_binary_images_ClockWidget_nixie_no_holes_2_jpg_end");
extern const byte nixie_3_start[] asm("_binary_images_ClockWidget_nixie_no_holes_3_jpg_start");
extern const byte nixie_3_end[] asm("_binary_images_ClockWidget_nixie_no_holes_3_jpg_end");
extern const byte nixie_4_start[] asm("_binary_images_ClockWidget_nixie_no_holes_4_jpg_start");
extern const byte nixie_4_end[] asm("_binary_images_ClockWidget_nixie_no_holes_4_jpg_end");
extern const byte nixie_5_start[] asm("_binary_images_ClockWidget_nixie_no_holes_5_jpg_start");
extern const byte nixie_5_end[] asm("_binary_images_ClockWidget_nixie_no_holes_5_jpg_end");
extern const byte nixie_6_start[] asm("_binary_images_ClockWidget_nixie_no_holes_6_jpg_start");
extern const byte nixie_6_end[] asm("_binary_images_ClockWidget_nixie_no_holes_6_jpg_end");
extern const byte nixie_7_start[] asm("_binary_images_ClockWidget_nixie_no_holes_7_jpg_start");
extern const byte nixie_7_end[] asm("_binary_images_ClockWidget_nixie_no_holes_7_jpg_end");
extern const byte nixie_8_start[] asm("_binary_images_ClockWidget_nixie_no_holes_8_jpg_start");
extern const byte nixie_8_end[] asm("_binary_images_ClockWidget_nixie_no_holes_8_jpg_end");
extern const byte nixie_9_start[] asm("_binary_images_ClockWidget_nixie_no_holes_9_jpg_start");
extern const byte nixie_9_end[] asm("_binary_images_ClockWidget_nixie_no_holes_9_jpg_end");
extern const byte nixie_colon_off_start[] asm("_binary_images_ClockWidget_nixie_no_holes_colon_off_jpg_start");
extern const byte nixie_colon_off_end[] asm("_binary_images_ClockWidget_nixie_no_holes_colon_off_jpg_end");
extern const byte nixie_colon_on_start[] asm("_binary_images_ClockWidget_nixie_no_holes_colon_on_jpg_start");
extern const byte nixie_colon_on_end[] asm("_binary_images_ClockWidget_nixie_no_holes_colon_on_jpg_end");

#endif
