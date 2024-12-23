# ClockWidget

To use the custom clock face, replace the JPGs in this folder with your own creations.
Make sure to save them as 240px \* 240px and DO NOT enable progressive encoding.

Then add
`#define USE_CLOCK_CUSTOM X`
to your config.h and set X to the number of custom clocks you want to include

If you want to disable the custom clocks, set
`#define USE_CLOCK_CUSTOM 0`

The first custom clock (number 0) uses these images:

- CustomClock0/0.jpg
- CustomClock0/1.jpg
- ...
- CustomClock0/9.jpg
- CustomClock0/10.jpg No colon image
- CustomClock0/11.jpg Colon image

The second custom clock (number 1) uses these images:

- CustomClock1/0.jpg
- CustomClock1/1.jpg
- ...
- CustomClock1/9.jpg
- CustomClock1/10.jpg No colon image
- CustomClock1/11.jpg Colon image

and so on.

In the WebPortal, you can set the color of the circular "second tick"
and also override color of your custom clock if you want.
