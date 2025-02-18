#include "flip.h"

#if defined(USE_CLOCK_FLIP) && (USE_CLOCK_FLIP == FLIPS)
extern const byte flip_0_1_start[] asm("_binary_images_clock_flip_010_jpg_start");
extern const byte flip_0_1_end[] asm("_binary_images_clock_flip_010_jpg_end");
extern const byte flip_1_2_start[] asm("_binary_images_clock_flip_120_jpg_start");
extern const byte flip_1_2_end[] asm("_binary_images_clock_flip_120_jpg_end");
extern const byte flip_2_3_start[] asm("_binary_images_clock_flip_230_jpg_start");
extern const byte flip_2_3_end[] asm("_binary_images_clock_flip_230_jpg_end");
extern const byte flip_3_4_start[] asm("_binary_images_clock_flip_340_jpg_start");
extern const byte flip_3_4_end[] asm("_binary_images_clock_flip_340_jpg_end");
extern const byte flip_4_5_start[] asm("_binary_images_clock_flip_450_jpg_start");
extern const byte flip_4_5_end[] asm("_binary_images_clock_flip_450_jpg_end");
extern const byte flip_5_6_start[] asm("_binary_images_clock_flip_560_jpg_start");
extern const byte flip_5_6_end[] asm("_binary_images_clock_flip_560_jpg_end");
extern const byte flip_6_7_start[] asm("_binary_images_clock_flip_670_jpg_start");
extern const byte flip_6_7_end[] asm("_binary_images_clock_flip_670_jpg_end");
extern const byte flip_7_8_start[] asm("_binary_images_clock_flip_780_jpg_start");
extern const byte flip_7_8_end[] asm("_binary_images_clock_flip_780_jpg_end");
extern const byte flip_8_9_start[] asm("_binary_images_clock_flip_890_jpg_start");
extern const byte flip_8_9_end[] asm("_binary_images_clock_flip_890_jpg_end");
extern const byte flip_9_0_start[] asm("_binary_images_clock_flip_900_jpg_start");
extern const byte flip_9_0_end[] asm("_binary_images_clock_flip_900_jpg_end");
extern const byte flip_colon_off_start[] asm("_binary_images_clock_flip_colonoff_jpg_start");
extern const byte flip_colon_off_end[] asm("_binary_images_clock_flip_colonoff_jpg_end");
extern const byte flip_colon_on_start[] asm("_binary_images_clock_flip_colonon_jpg_start");
extern const byte flip_colon_on_end[] asm("_binary_images_clock_flip_colonon_jpg_end");
extern const byte flip_0_1_1_start[] asm("_binary_images_clock_flip_011_jpg_start");
extern const byte flip_0_1_1_end[] asm("_binary_images_clock_flip_011_jpg_end");
extern const byte flip_0_1_2_start[] asm("_binary_images_clock_flip_012_jpg_start");
extern const byte flip_0_1_2_end[] asm("_binary_images_clock_flip_012_jpg_end");
extern const byte flip_0_1_3_start[] asm("_binary_images_clock_flip_013_jpg_start");
extern const byte flip_0_1_3_end[] asm("_binary_images_clock_flip_013_jpg_end");
extern const byte flip_0_1_4_start[] asm("_binary_images_clock_flip_014_jpg_start");
extern const byte flip_0_1_4_end[] asm("_binary_images_clock_flip_014_jpg_end");
extern const byte flip_1_2_1_start[] asm("_binary_images_clock_flip_121_jpg_start");
extern const byte flip_1_2_1_end[] asm("_binary_images_clock_flip_121_jpg_end");
extern const byte flip_1_2_2_start[] asm("_binary_images_clock_flip_122_jpg_start");
extern const byte flip_1_2_2_end[] asm("_binary_images_clock_flip_122_jpg_end");
extern const byte flip_1_2_3_start[] asm("_binary_images_clock_flip_123_jpg_start");
extern const byte flip_1_2_3_end[] asm("_binary_images_clock_flip_123_jpg_end");
extern const byte flip_1_2_4_start[] asm("_binary_images_clock_flip_124_jpg_start");
extern const byte flip_1_2_4_end[] asm("_binary_images_clock_flip_124_jpg_end");
extern const byte flip_2_3_1_start[] asm("_binary_images_clock_flip_231_jpg_start");
extern const byte flip_2_3_1_end[] asm("_binary_images_clock_flip_231_jpg_end");
extern const byte flip_2_3_2_start[] asm("_binary_images_clock_flip_232_jpg_start");
extern const byte flip_2_3_2_end[] asm("_binary_images_clock_flip_232_jpg_end");
extern const byte flip_2_3_3_start[] asm("_binary_images_clock_flip_233_jpg_start");
extern const byte flip_2_3_3_end[] asm("_binary_images_clock_flip_233_jpg_end");
extern const byte flip_2_3_4_start[] asm("_binary_images_clock_flip_234_jpg_start");
extern const byte flip_2_3_4_end[] asm("_binary_images_clock_flip_234_jpg_end");
extern const byte flip_3_4_1_start[] asm("_binary_images_clock_flip_341_jpg_start");
extern const byte flip_3_4_1_end[] asm("_binary_images_clock_flip_341_jpg_end");
extern const byte flip_3_4_2_start[] asm("_binary_images_clock_flip_342_jpg_start");
extern const byte flip_3_4_2_end[] asm("_binary_images_clock_flip_342_jpg_end");
extern const byte flip_3_4_3_start[] asm("_binary_images_clock_flip_343_jpg_start");
extern const byte flip_3_4_3_end[] asm("_binary_images_clock_flip_343_jpg_end");
extern const byte flip_3_4_4_start[] asm("_binary_images_clock_flip_344_jpg_start");
extern const byte flip_3_4_4_end[] asm("_binary_images_clock_flip_344_jpg_end");
extern const byte flip_4_5_1_start[] asm("_binary_images_clock_flip_451_jpg_start");
extern const byte flip_4_5_1_end[] asm("_binary_images_clock_flip_451_jpg_end");
extern const byte flip_4_5_2_start[] asm("_binary_images_clock_flip_452_jpg_start");
extern const byte flip_4_5_2_end[] asm("_binary_images_clock_flip_452_jpg_end");
extern const byte flip_4_5_3_start[] asm("_binary_images_clock_flip_453_jpg_start");
extern const byte flip_4_5_3_end[] asm("_binary_images_clock_flip_453_jpg_end");
extern const byte flip_4_5_4_start[] asm("_binary_images_clock_flip_454_jpg_start");
extern const byte flip_4_5_4_end[] asm("_binary_images_clock_flip_454_jpg_end");
extern const byte flip_5_6_1_start[] asm("_binary_images_clock_flip_561_jpg_start");
extern const byte flip_5_6_1_end[] asm("_binary_images_clock_flip_561_jpg_end");
extern const byte flip_5_6_2_start[] asm("_binary_images_clock_flip_562_jpg_start");
extern const byte flip_5_6_2_end[] asm("_binary_images_clock_flip_562_jpg_end");
extern const byte flip_5_6_3_start[] asm("_binary_images_clock_flip_563_jpg_start");
extern const byte flip_5_6_3_end[] asm("_binary_images_clock_flip_563_jpg_end");
extern const byte flip_5_6_4_start[] asm("_binary_images_clock_flip_564_jpg_start");
extern const byte flip_5_6_4_end[] asm("_binary_images_clock_flip_564_jpg_end");
extern const byte flip_6_7_1_start[] asm("_binary_images_clock_flip_671_jpg_start");
extern const byte flip_6_7_1_end[] asm("_binary_images_clock_flip_671_jpg_end");
extern const byte flip_6_7_2_start[] asm("_binary_images_clock_flip_672_jpg_start");
extern const byte flip_6_7_2_end[] asm("_binary_images_clock_flip_672_jpg_end");
extern const byte flip_6_7_3_start[] asm("_binary_images_clock_flip_673_jpg_start");
extern const byte flip_6_7_3_end[] asm("_binary_images_clock_flip_673_jpg_end");
extern const byte flip_6_7_4_start[] asm("_binary_images_clock_flip_674_jpg_start");
extern const byte flip_6_7_4_end[] asm("_binary_images_clock_flip_674_jpg_end");
extern const byte flip_7_8_1_start[] asm("_binary_images_clock_flip_781_jpg_start");
extern const byte flip_7_8_1_end[] asm("_binary_images_clock_flip_781_jpg_end");
extern const byte flip_7_8_2_start[] asm("_binary_images_clock_flip_782_jpg_start");
extern const byte flip_7_8_2_end[] asm("_binary_images_clock_flip_782_jpg_end");
extern const byte flip_7_8_3_start[] asm("_binary_images_clock_flip_783_jpg_start");
extern const byte flip_7_8_3_end[] asm("_binary_images_clock_flip_783_jpg_end");
extern const byte flip_7_8_4_start[] asm("_binary_images_clock_flip_784_jpg_start");
extern const byte flip_7_8_4_end[] asm("_binary_images_clock_flip_784_jpg_end");
extern const byte flip_8_9_1_start[] asm("_binary_images_clock_flip_891_jpg_start");
extern const byte flip_8_9_1_end[] asm("_binary_images_clock_flip_891_jpg_end");
extern const byte flip_8_9_2_start[] asm("_binary_images_clock_flip_892_jpg_start");
extern const byte flip_8_9_2_end[] asm("_binary_images_clock_flip_892_jpg_end");
extern const byte flip_8_9_3_start[] asm("_binary_images_clock_flip_893_jpg_start");
extern const byte flip_8_9_3_end[] asm("_binary_images_clock_flip_893_jpg_end");
extern const byte flip_8_9_4_start[] asm("_binary_images_clock_flip_894_jpg_start");
extern const byte flip_8_9_4_end[] asm("_binary_images_clock_flip_894_jpg_end");
extern const byte flip_9_0_1_start[] asm("_binary_images_clock_flip_901_jpg_start");
extern const byte flip_9_0_1_end[] asm("_binary_images_clock_flip_901_jpg_end");
extern const byte flip_9_0_2_start[] asm("_binary_images_clock_flip_902_jpg_start");
extern const byte flip_9_0_2_end[] asm("_binary_images_clock_flip_902_jpg_end");
extern const byte flip_9_0_3_start[] asm("_binary_images_clock_flip_903_jpg_start");
extern const byte flip_9_0_3_end[] asm("_binary_images_clock_flip_903_jpg_end");
extern const byte flip_9_0_4_start[] asm("_binary_images_clock_flip_904_jpg_start");
extern const byte flip_9_0_4_end[] asm("_binary_images_clock_flip_904_jpg_end");
extern const byte flip_1_0_1_start[] asm("_binary_images_clock_flip_101_jpg_start");
extern const byte flip_1_0_1_end[] asm("_binary_images_clock_flip_101_jpg_end");
extern const byte flip_1_0_2_start[] asm("_binary_images_clock_flip_102_jpg_start");
extern const byte flip_1_0_2_end[] asm("_binary_images_clock_flip_102_jpg_end");
extern const byte flip_1_0_3_start[] asm("_binary_images_clock_flip_103_jpg_start");
extern const byte flip_1_0_3_end[] asm("_binary_images_clock_flip_103_jpg_end");
extern const byte flip_1_0_4_start[] asm("_binary_images_clock_flip_104_jpg_start");
extern const byte flip_1_0_4_end[] asm("_binary_images_clock_flip_104_jpg_end");
extern const byte flip_2_0_1_start[] asm("_binary_images_clock_flip_201_jpg_start");
extern const byte flip_2_0_1_end[] asm("_binary_images_clock_flip_201_jpg_end");
extern const byte flip_2_0_2_start[] asm("_binary_images_clock_flip_202_jpg_start");
extern const byte flip_2_0_2_end[] asm("_binary_images_clock_flip_202_jpg_end");
extern const byte flip_2_0_3_start[] asm("_binary_images_clock_flip_203_jpg_start");
extern const byte flip_2_0_3_end[] asm("_binary_images_clock_flip_203_jpg_end");
extern const byte flip_2_0_4_start[] asm("_binary_images_clock_flip_204_jpg_start");
extern const byte flip_2_0_4_end[] asm("_binary_images_clock_flip_204_jpg_end");
extern const byte flip_5_0_1_start[] asm("_binary_images_clock_flip_501_jpg_start");
extern const byte flip_5_0_1_end[] asm("_binary_images_clock_flip_501_jpg_end");
extern const byte flip_5_0_2_start[] asm("_binary_images_clock_flip_502_jpg_start");
extern const byte flip_5_0_2_end[] asm("_binary_images_clock_flip_502_jpg_end");
extern const byte flip_5_0_3_start[] asm("_binary_images_clock_flip_503_jpg_start");
extern const byte flip_5_0_3_end[] asm("_binary_images_clock_flip_503_jpg_end");
extern const byte flip_5_0_4_start[] asm("_binary_images_clock_flip_504_jpg_start");
extern const byte flip_5_0_4_end[] asm("_binary_images_clock_flip_504_jpg_end");
extern const byte flip_2_1_1_start[] asm("_binary_images_clock_flip_211_jpg_start");
extern const byte flip_2_1_1_end[] asm("_binary_images_clock_flip_211_jpg_end");
extern const byte flip_2_1_2_start[] asm("_binary_images_clock_flip_212_jpg_start");
extern const byte flip_2_1_2_end[] asm("_binary_images_clock_flip_212_jpg_end");
extern const byte flip_2_1_3_start[] asm("_binary_images_clock_flip_213_jpg_start");
extern const byte flip_2_1_3_end[] asm("_binary_images_clock_flip_213_jpg_end");
extern const byte flip_2_1_4_start[] asm("_binary_images_clock_flip_214_jpg_start");
extern const byte flip_2_1_4_end[] asm("_binary_images_clock_flip_214_jpg_end");
extern const byte flip_3_0_1_start[] asm("_binary_images_clock_flip_301_jpg_start");
extern const byte flip_3_0_1_end[] asm("_binary_images_clock_flip_301_jpg_end");
extern const byte flip_3_0_2_start[] asm("_binary_images_clock_flip_302_jpg_start");
extern const byte flip_3_0_2_end[] asm("_binary_images_clock_flip_302_jpg_end");
extern const byte flip_3_0_3_start[] asm("_binary_images_clock_flip_303_jpg_start");
extern const byte flip_3_0_3_end[] asm("_binary_images_clock_flip_303_jpg_end");
extern const byte flip_3_0_4_start[] asm("_binary_images_clock_flip_304_jpg_start");
extern const byte flip_3_0_4_end[] asm("_binary_images_clock_flip_304_jpg_end");
extern const byte flip_colon_on_1_start[] asm("_binary_images_clock_flip_colonon1_jpg_start");
extern const byte flip_colon_on_1_end[] asm("_binary_images_clock_flip_colonon1_jpg_end");
extern const byte flip_colon_on_2_start[] asm("_binary_images_clock_flip_colonon2_jpg_start");
extern const byte flip_colon_on_2_end[] asm("_binary_images_clock_flip_colonon2_jpg_end");
extern const byte flip_colon_on_3_start[] asm("_binary_images_clock_flip_colonon3_jpg_start");
extern const byte flip_colon_on_3_end[] asm("_binary_images_clock_flip_colonon3_jpg_end");
extern const byte flip_colon_on_4_start[] asm("_binary_images_clock_flip_colonon4_jpg_start");
extern const byte flip_colon_on_4_end[] asm("_binary_images_clock_flip_colonon4_jpg_end");
extern const byte flip_colon_off_1_start[] asm("_binary_images_clock_flip_colonoff1_jpg_start");
extern const byte flip_colon_off_1_end[] asm("_binary_images_clock_flip_colonoff1_jpg_end");
extern const byte flip_colon_off_2_start[] asm("_binary_images_clock_flip_colonoff2_jpg_start");
extern const byte flip_colon_off_2_end[] asm("_binary_images_clock_flip_colonoff2_jpg_end");
extern const byte flip_colon_off_3_start[] asm("_binary_images_clock_flip_colonoff3_jpg_start");
extern const byte flip_colon_off_3_end[] asm("_binary_images_clock_flip_colonoff3_jpg_end");
extern const byte flip_colon_off_4_start[] asm("_binary_images_clock_flip_colonoff4_jpg_start");
extern const byte flip_colon_off_4_end[] asm("_binary_images_clock_flip_colonoff4_jpg_end");

const byte *clock_flip[80][2] = {
    {flip_0_1_start, flip_0_1_end},
    {flip_1_2_start, flip_1_2_end},
    {flip_2_3_start, flip_2_3_end},
    {flip_3_4_start, flip_3_4_end},
    {flip_4_5_start, flip_4_5_end},
    {flip_5_6_start, flip_5_6_end},
    {flip_6_7_start, flip_6_7_end},
    {flip_7_8_start, flip_7_8_end},
    {flip_8_9_start, flip_8_9_end},
    {flip_9_0_start, flip_9_0_end},
    {flip_colon_off_start, flip_colon_off_end},
    {flip_colon_on_start, flip_colon_on_end},
    {flip_0_1_1_start, flip_0_1_1_end},
    {flip_0_1_2_start, flip_0_1_2_end},
    {flip_0_1_3_start, flip_0_1_3_end},
    {flip_0_1_4_start, flip_0_1_4_end},
    {flip_1_2_1_start, flip_1_2_1_end},
    {flip_1_2_2_start, flip_1_2_2_end},
    {flip_1_2_3_start, flip_1_2_3_end},
    {flip_1_2_4_start, flip_1_2_4_end},
    {flip_2_3_1_start, flip_2_3_1_end},
    {flip_2_3_2_start, flip_2_3_2_end},
    {flip_2_3_3_start, flip_2_3_3_end},
    {flip_2_3_4_start, flip_2_3_4_end},
    {flip_3_4_1_start, flip_3_4_1_end},
    {flip_3_4_2_start, flip_3_4_2_end},
    {flip_3_4_3_start, flip_3_4_3_end},
    {flip_3_4_4_start, flip_3_4_4_end},
    {flip_4_5_1_start, flip_4_5_1_end},
    {flip_4_5_2_start, flip_4_5_2_end},
    {flip_4_5_3_start, flip_4_5_3_end},
    {flip_4_5_4_start, flip_4_5_4_end},
    {flip_5_6_1_start, flip_5_6_1_end},
    {flip_5_6_2_start, flip_5_6_2_end},
    {flip_5_6_3_start, flip_5_6_3_end},
    {flip_5_6_4_start, flip_5_6_4_end},
    {flip_6_7_1_start, flip_6_7_1_end},
    {flip_6_7_2_start, flip_6_7_2_end},
    {flip_6_7_3_start, flip_6_7_3_end},
    {flip_6_7_4_start, flip_6_7_4_end},
    {flip_7_8_1_start, flip_7_8_1_end},
    {flip_7_8_2_start, flip_7_8_2_end},
    {flip_7_8_3_start, flip_7_8_3_end},
    {flip_7_8_4_start, flip_7_8_4_end},
    {flip_8_9_1_start, flip_8_9_1_end},
    {flip_8_9_2_start, flip_8_9_2_end},
    {flip_8_9_3_start, flip_8_9_3_end},
    {flip_8_9_4_start, flip_8_9_4_end},
    {flip_9_0_1_start, flip_9_0_1_end},
    {flip_9_0_2_start, flip_9_0_2_end},
    {flip_9_0_3_start, flip_9_0_3_end},
    {flip_9_0_4_start, flip_9_0_4_end},
    {flip_1_0_1_start, flip_1_0_1_end},
    {flip_1_0_2_start, flip_1_0_2_end},
    {flip_1_0_3_start, flip_1_0_3_end},
    {flip_1_0_4_start, flip_1_0_4_end},
    {flip_2_0_1_start, flip_2_0_1_end},
    {flip_2_0_2_start, flip_2_0_2_end},
    {flip_2_0_3_start, flip_2_0_3_end},
    {flip_2_0_4_start, flip_2_0_4_end},
    {flip_5_0_1_start, flip_5_0_1_end},
    {flip_5_0_2_start, flip_5_0_2_end},
    {flip_5_0_3_start, flip_5_0_3_end},
    {flip_5_0_4_start, flip_5_0_4_end},
    {flip_2_1_1_start, flip_2_1_1_end},
    {flip_2_1_2_start, flip_2_1_2_end},
    {flip_2_1_3_start, flip_2_1_3_end},
    {flip_2_1_4_start, flip_2_1_4_end},
    {flip_3_0_1_start, flip_3_0_1_end},
    {flip_3_0_2_start, flip_3_0_2_end},
    {flip_3_0_3_start, flip_3_0_3_end},
    {flip_3_0_4_start, flip_3_0_4_end},
    {flip_colon_on_1_start, flip_colon_on_1_end},
    {flip_colon_on_2_start, flip_colon_on_2_end},
    {flip_colon_on_3_start, flip_colon_on_3_end},
    {flip_colon_on_4_start, flip_colon_on_4_end},
    {flip_colon_off_1_start, flip_colon_off_1_end},
    {flip_colon_off_2_start, flip_colon_off_2_end},
    {flip_colon_off_3_start, flip_colon_off_3_end},
    {flip_colon_off_4_start, flip_colon_off_4_end}};

#endif
