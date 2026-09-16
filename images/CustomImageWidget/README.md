# Custom Image Widget

The widget supports multiple pages of five images and can rotate through them
automatically.

The original files form page 1:

```text
screen_1.jpg
screen_2.jpg
screen_3.jpg
screen_4.jpg
screen_5.jpg
```

Add page 2 to the same directory using:

```text
page_2_screen_1.jpg
page_2_screen_2.jpg
page_2_screen_3.jpg
page_2_screen_4.jpg
page_2_screen_5.jpg
```

Continue with `page_3_screen_1.jpg` through `page_3_screen_5.jpg`, and so on.
Pages must be numbered consecutively and each page must contain all five
images. Up to 100 complete pages are detected automatically. The screen number
matches the physical display number from left to right.

Set the automatic page-change interval in `firmware/config/config.h`:

```c
#define CUSTOM_IMAGE_CHANGE_INTERVAL_SEC 30
```

The value is in seconds; use `0` to disable automatic changes. It can also be
changed in the advanced section of the web configuration portal. A short press
of the middle button advances to the next page immediately.

## Uploading from the web portal

Open the InfoOrbs web portal and select **Custom Images**. Choose a page number
and an image for any screen you want to replace. The browser previews and
converts each selected file to a 240 x 240 JPEG before uploading it to the
correct LittleFS filename. You can choose either:

- **Fill screen**, which crops the edges to avoid margins.
- **Show whole image**, which preserves the complete image and adds black
  margins when its aspect ratio is not square.

Uploads are processed one at a time to keep ESP32 memory usage low. For a new
page, upload all five screen images. Every earlier page must also be complete
for the widget to discover later pages.

Recommended image settings:

- JPEG format
- 240 x 240 pixels
- RGB colour
- Baseline encoding (progressive JPEG is not supported)
- Black background near the corners if you want the artwork to blend into the
  circular displays

Larger images are reduced automatically by factors of 2, 4 or 8, but preparing
them at exactly 240 x 240 gives the most predictable result and keeps the
firmware small.

Enable the widget in `firmware/config/config.h`:

```c
#define INCLUDE_CUSTOM_IMAGE WIDGET_ON
```

Images uploaded through the web portal are stored in LittleFS; no firmware
rebuild is required. Images placed in this directory locally are copied into
the LittleFS filesystem image during the build and require a filesystem upload.

The five bundled screen_1.jpg through screen_5.jpg files are sample copies of
the project startup image (images/logo.jpg), forming a complete first page.

Additional local images are ignored by Git. The five bundled samples are
tracked: replacing them locally will appear as a Git change. Upload personal
images through the web portal, and do not commit personal replacements.
