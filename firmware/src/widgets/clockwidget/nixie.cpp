#include "nixie.h"

#if defined(USE_CLOCK_NIXIE) && (USE_CLOCK_NIXIE == NIXIE_HOLES || USE_CLOCK_NIXIE == NIXIE_NOHOLES)
    #if USE_CLOCK_NIXIE == NIXIE_HOLES
extern const byte nixie_0_start[] asm("_binary_images_clock_nixie_holes_0_jpg_start");
extern const byte nixie_0_end[] asm("_binary_images_clock_nixie_holes_0_jpg_end");
extern const byte nixie_1_start[] asm("_binary_images_clock_nixie_holes_1_jpg_start");
extern const byte nixie_1_end[] asm("_binary_images_clock_nixie_holes_1_jpg_end");
extern const byte nixie_2_start[] asm("_binary_images_clock_nixie_holes_2_jpg_start");
extern const byte nixie_2_end[] asm("_binary_images_clock_nixie_holes_2_jpg_end");
extern const byte nixie_3_start[] asm("_binary_images_clock_nixie_holes_3_jpg_start");
extern const byte nixie_3_end[] asm("_binary_images_clock_nixie_holes_3_jpg_end");
extern const byte nixie_4_start[] asm("_binary_images_clock_nixie_holes_4_jpg_start");
extern const byte nixie_4_end[] asm("_binary_images_clock_nixie_holes_4_jpg_end");
extern const byte nixie_5_start[] asm("_binary_images_clock_nixie_holes_5_jpg_start");
extern const byte nixie_5_end[] asm("_binary_images_clock_nixie_holes_5_jpg_end");
extern const byte nixie_6_start[] asm("_binary_images_clock_nixie_holes_6_jpg_start");
extern const byte nixie_6_end[] asm("_binary_images_clock_nixie_holes_6_jpg_end");
extern const byte nixie_7_start[] asm("_binary_images_clock_nixie_holes_7_jpg_start");
extern const byte nixie_7_end[] asm("_binary_images_clock_nixie_holes_7_jpg_end");
extern const byte nixie_8_start[] asm("_binary_images_clock_nixie_holes_8_jpg_start");
extern const byte nixie_8_end[] asm("_binary_images_clock_nixie_holes_8_jpg_end");
extern const byte nixie_9_start[] asm("_binary_images_clock_nixie_holes_9_jpg_start");
extern const byte nixie_9_end[] asm("_binary_images_clock_nixie_holes_9_jpg_end");
extern const byte nixie_colon_off_start[] asm("_binary_images_clock_nixie_holes_10_jpg_start");
extern const byte nixie_colon_off_end[] asm("_binary_images_clock_nixie_holes_10_jpg_end");
extern const byte nixie_colon_on_start[] asm("_binary_images_clock_nixie_holes_11_jpg_start");
extern const byte nixie_colon_on_end[] asm("_binary_images_clock_nixie_holes_11_jpg_end");
    #elif USE_CLOCK_NIXIE == NIXIE_NOHOLES
extern const byte nixie_0_start[] asm("_binary_images_clock_nixie_no_holes_0_jpg_start");
extern const byte nixie_0_end[] asm("_binary_images_clock_nixie_no_holes_0_jpg_end");
extern const byte nixie_1_start[] asm("_binary_images_clock_nixie_no_holes_1_jpg_start");
extern const byte nixie_1_end[] asm("_binary_images_clock_nixie_no_holes_1_jpg_end");
extern const byte nixie_2_start[] asm("_binary_images_clock_nixie_no_holes_2_jpg_start");
extern const byte nixie_2_end[] asm("_binary_images_clock_nixie_no_holes_2_jpg_end");
extern const byte nixie_3_start[] asm("_binary_images_clock_nixie_no_holes_3_jpg_start");
extern const byte nixie_3_end[] asm("_binary_images_clock_nixie_no_holes_3_jpg_end");
extern const byte nixie_4_start[] asm("_binary_images_clock_nixie_no_holes_4_jpg_start");
extern const byte nixie_4_end[] asm("_binary_images_clock_nixie_no_holes_4_jpg_end");
extern const byte nixie_5_start[] asm("_binary_images_clock_nixie_no_holes_5_jpg_start");
extern const byte nixie_5_end[] asm("_binary_images_clock_nixie_no_holes_5_jpg_end");
extern const byte nixie_6_start[] asm("_binary_images_clock_nixie_no_holes_6_jpg_start");
extern const byte nixie_6_end[] asm("_binary_images_clock_nixie_no_holes_6_jpg_end");
extern const byte nixie_7_start[] asm("_binary_images_clock_nixie_no_holes_7_jpg_start");
extern const byte nixie_7_end[] asm("_binary_images_clock_nixie_no_holes_7_jpg_end");
extern const byte nixie_8_start[] asm("_binary_images_clock_nixie_no_holes_8_jpg_start");
extern const byte nixie_8_end[] asm("_binary_images_clock_nixie_no_holes_8_jpg_end");
extern const byte nixie_9_start[] asm("_binary_images_clock_nixie_no_holes_9_jpg_start");
extern const byte nixie_9_end[] asm("_binary_images_clock_nixie_no_holes_9_jpg_end");
extern const byte nixie_colon_off_start[] asm("_binary_images_clock_nixie_no_holes_10_jpg_start");
extern const byte nixie_colon_off_end[] asm("_binary_images_clock_nixie_no_holes_10_jpg_end");
extern const byte nixie_colon_on_start[] asm("_binary_images_clock_nixie_no_holes_11_jpg_start");
extern const byte nixie_colon_on_end[] asm("_binary_images_clock_nixie_no_holes_11_jpg_end");
    #endif

const byte *clock_nixie[12][2] = {
    {nixie_0_start, nixie_0_end},
    {nixie_1_start, nixie_1_end},
    {nixie_2_start, nixie_2_end},
    {nixie_3_start, nixie_3_end},
    {nixie_4_start, nixie_4_end},
    {nixie_5_start, nixie_5_end},
    {nixie_6_start, nixie_6_end},
    {nixie_7_start, nixie_7_end},
    {nixie_8_start, nixie_8_end},
    {nixie_9_start, nixie_9_end},
    {nixie_colon_off_start, nixie_colon_off_end},
    {nixie_colon_on_start, nixie_colon_on_end}};

#endif