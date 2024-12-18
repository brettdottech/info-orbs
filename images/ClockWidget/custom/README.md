# ClockWidget

To use the custom clock face, replace the JPGs in this folder with your own creations.
Make sure to save them as 240px \* 240px and DO NOT enable progressive encoding.

Then add
`#define USE_CLOCK_CUSTOM X`
to your config.h and set X to the number of custom clocks you want to include

If you want to disable the nixie clock, set
`#define USE_CLOCK_NIXIE 0`

The first custom clock (number 0) uses these images:

- 0_0.jpg
- 0_1.jpg
- ...
- 0_9.jpg
- 0_colon_off.jpg
- 0_colon_on.jpg

The second custom clock (number 1) uses these images:

- 1_0.jpg
- 1_1.jpg
- ...
- 1_9.jpg
- 1_colon_off.jpg
- 1_colon_on.jpg

and so on.

In the WebPortal, you can set the color of the circular "second tick"
and also override color of your custom clock if you want.
