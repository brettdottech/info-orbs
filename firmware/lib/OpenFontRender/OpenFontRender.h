// -------------------------------------------------------
//  OpenFontRender.h
//
//  Copyright (c) 2021 takkaO
//
//  If you use, modify or redistribute this file as part of
//  the original repository, please follow the repository's license.
//
// -------------------------------------------------------

#ifndef OPEN_FONT_RENDER_H
#define OPEN_FONT_RENDER_H

#if defined(ARDUINO)
	#include <Arduino.h>
	#undef min
	#undef max
#endif

#include <stdarg.h>
#include <stdio.h>

#include "ft2build.h"
#include FT_CACHE_H
#include FT_FREETYPE_H

#include <functional>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "FileSupport.h"

/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/
//
//  Constant definition
//
/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/

/*!
 * @brief An enumeration for specifying the debug log level.
 */
enum OFR_DEBUG_LEVEL {
	OFR_NONE  = 0, ///< No output.
	OFR_ERROR = 1, ///< Output only errors.
	OFR_INFO  = 2, ///< Output some informations.
	OFR_DEBUG = 4, ///< Output character rendering result.
	OFR_RAW   = 8, ///< Use for internal.
};

/*!
 * @brief An enumeration for specifying the text alignment.
 * @see PREPARING figure
 */
enum class Align {
	Left,         ///< Alias of "TopLeft"
	Center,       ///< Alias of "TopCenter"
	Right,        ///< Alias of "TopRight"
	TopLeft,      ///< The cursor position is considered to be the top-left corner of the text box.
	TopCenter,    ///< The cursor position is considered to be the top-center of the text box.
	TopRight,     ///< The cursor position is considered to be the top-right corner of the text box.
	MiddleLeft,   ///< The cursor position is considered to be the middle-left of the text box.
	MiddleCenter, ///< The cursor position is considered to be the middle-center of the text box.
	MiddleRight,  ///< The cursor position is considered to be the middle-right of the text box.
	BottomLeft,   ///< The cursor position is considered to be the bottom-left corner of the text box.
	BottomCenter, ///< The cursor position is considered to be the bottom-center of the text box.
	BottomRight,  ///< The cursor position is considered to be the bottom-right corner of the text box.
};

/*!
 * @brief An enumeration for specifying the background drawing method.
 */
enum class BgFillMethod {
	None,    ///< Does not fill the background.
	Minimum, ///< Fill in the smallest area that surrounds each character.
	Block,   ///< Fill in the smallest area that surrounds the string.
};

/*!
 * @brief An enumeration for specifying the direction in which characters are written.
 */
enum class Layout {
	Horizontal, ///< Write from left to right.
	Vertical    ///< Write from top to bottom.
};

/*!
 * @brief An enumeration for specifying the whether to draw to screen.
 */
enum class Drawing {
	Execute, ///< The drawing process is executed and the screen will be updated.
	Skip     ///< The drawing process is skiped and the screen will not be updated.
};

/*! \cond PRIVATE */
namespace OFR {
	/* USER DO NOT USE DIRECTORY IN THIS SCOPE ELEMENTS */
	enum LoadFontFrom {
		FROM_FILE,
		FROM_MEMORY
	};

	typedef struct FaceRec_ {
		char *filepath;      // ttf file path
		unsigned char *data; // ttf array
		size_t data_size;    // ttf array size
		uint8_t face_index;  // face index (default is 0)
	} FaceRec, *Face;
};
/*! \endcond */

/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/
//
//  Class definition
//
/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/

/*!
 * @brief Set function to draw pixel to screen.
 * @param[in] (user_func) User function for draw pixel to screen.
 * @ingroup rendering_api
 * @note The function to be given must be a function like the one below that takes a specific argument.
 * @code {.cpp}
 * void example_function (int32_t x, int32_t y, uint16_t c)
 * @endcode
 * | Type    | Name | Description               |
 * | ------- | :--: | ------------------------- |
 * | int32_t |  x   | Draw position X           |
 * | int32_t |  y   | Draw position Y           |
 * | int32_t |  c   | Draw color (16 bit color) |
 */
#define setDrawPixel(F) set_drawPixel([&](int32_t x, int32_t y, uint16_t c) { return F(x, y, c); })
/*!
 * @brief Set function to fast draw horizontal line to screen. (Optional)
 * @param[in] (user_func) User function for fast draw horizontal line to screen.
 * @ingroup rendering_api
 * @note If you set this function, drawing method will be optimized and improve speed.
 * @code {.cpp}
 * void example_function (int32_t x, int32_t y, int32_t w, uint16_t c)
 * @endcode
 * | Type    | Name | Description               |
 * | ------- | :--: | ------------------------- |
 * | int32_t |  x   | Draw position X           |
 * | int32_t |  y   | Draw position Y           |
 * | int32_t |  w   | Draw Length               |
 * | int32_t |  c   | Draw color (16 bit color) |
 */
#define setDrawFastHLine(F) set_drawFastHLine([&](int32_t x, int32_t y, int32_t w, uint16_t c) { return F(x, y, w, c); })
/*!
 * @brief It is called only once at the beginning of a sequence of drawings. (Optional)
 * @brief Certain libraries can occupy the bus during continuous drawing to increase the drawing speed.
 * @param[in] (user_func) User function that call before draw to screen.
 * @ingroup rendering_api
 * @note If you set this function, drawing method will be optimized and improve speed.
 * @code {.cpp}
 * void example_function (void)
 * @endcode
 */
#define setStartWrite(F) set_startWrite([&](void) { return F(); })
/*!
 * @brief It is called only once at the beginning of a sequence of drawings. (Optional)
 * @brief Certain libraries can occupy the bus during continuous drawing to increase the drawing speed.
 * @param[in] (user_func) User function that call after draw to screen.
 * @ingroup rendering_api
 * @note If you set this function, drawing method will be optimized and improve speed.
 * @code {.cpp}
 * void example_function (void)
 * @endcode
 */
#define setEndWrite(F) set_endWrite([&](void) { return F(); })
/*!
 * @brief Specifies the standard output destination for the system. (Optional)
 * @param[in] (user_func) User function for output message.
 * @ingroup rendering_api
 * @note The function to be given must be a function like the one below that takes a specific argument.
 * @code {.cpp}
 * void example_function (const char *s)
 * @endcode
 */
#define setPrintFunc(F) set_printFunc([&](const char *s) { return F(s); })

class OpenFontRender {
public:
	static const unsigned char MAIN_VERSION  = 1; ///< Open Font Render library main version.
	static const unsigned char MINOR_VERSION = 2; ///< Open Font Render library minor version.

	static const unsigned char CACHE_SIZE_NO_LIMIT    = 0;   ///< FreeType cache size alias.
	static const unsigned char CACHE_SIZE_MINIMUM     = 1;   ///< FreeType cache size alias.
	static const unsigned char FT_VERSION_STRING_SIZE = 32;  ///< Minimum string length for FreeType version.
	static const unsigned char CREDIT_STRING_SIZE     = 128; ///< Minimum string length for FreeType credit.

	OpenFontRender();
	void setUseRenderTask(bool enable);
	void setRenderTaskStackSize(unsigned int stack_size);

	void setCursor(int32_t x, int32_t y);
	int32_t getCursorX();
	int32_t getCursorY();
	void seekCursor(int32_t delta_x, int32_t delta_y);

	void setFontColor(uint16_t font_color);
	void setBackgroundColor(uint16_t font_bgcolor);
	void setFontColor(uint16_t font_color, uint16_t font_bgcolor);

	void setFontColor(uint8_t r, uint8_t g, uint8_t b);
	void setBackgroundColor(uint8_t r, uint8_t g, uint8_t b);
	void setFontColor(uint8_t fr,
	                  uint8_t fg,
	                  uint8_t fb,
	                  uint8_t br,
	                  uint8_t bg,
	                  uint8_t bb);
	uint16_t getFontColor();
	uint16_t getBackgroundColor();
	void setFontSize(unsigned int pixel);
	unsigned int getFontSize();
	double setLineSpaceRatio(double line_space_ratio);
	double getLineSpaceRatio();
	void setBackgroundFillMethod(BgFillMethod method);
	BgFillMethod getBackgroundFillMethod();
	void setLayout(Layout layout);
	Layout getLayout();
	void setAlignment(Align align);
	Align getAlignment();
	void setCacheSize(unsigned int max_faces, unsigned int max_sizes, unsigned long max_bytes);

	FT_Error loadFont(const unsigned char *data, size_t size, uint8_t target_face_index = 0);
	FT_Error loadFont(const char *fpath, uint8_t target_face_index = 0);
	void unloadFont();

	uint16_t drawHString(const char *str,
	                     int32_t x,
	                     int32_t y,
	                     uint16_t fg,
	                     uint16_t bg,
	                     Align align,
	                     Drawing drawing,
	                     FT_BBox &abbox,
	                     FT_Error &error);
	FT_Error drawChar(char character,
	                  int32_t x   = 0,
	                  int32_t y   = 0,
	                  uint16_t fg = 0xFFFF,
	                  uint16_t bg = 0x0000,
	                  Align align = Align::Left);
	uint16_t drawString(const char *str,
	                    int32_t x     = 0,
	                    int32_t y     = 0,
	                    uint16_t fg   = 0xFFFF,
	                    uint16_t bg   = 0x0000,
	                    Layout layout = Layout::Horizontal);
	uint16_t cdrawString(const char *str,
	                     int32_t x     = 0,
	                     int32_t y     = 0,
	                     uint16_t fg   = 0xFFFF,
	                     uint16_t bg   = 0x0000,
	                     Layout layout = Layout::Horizontal);
	uint16_t rdrawString(const char *str,
	                     int32_t x     = 0,
	                     int32_t y     = 0,
	                     uint16_t fg   = 0xFFFF,
	                     uint16_t bg   = 0x0000,
	                     Layout layout = Layout::Horizontal);

	uint16_t printf(const char *fmt, ...);
	uint16_t cprintf(const char *fmt, ...);
	uint16_t rprintf(const char *fmt, ...);

	FT_BBox calculateBoundingBoxFmt(int32_t x, int32_t y, unsigned int font_size, Align align, Layout layout, const char *fmt, ...);
	FT_BBox calculateBoundingBox(int32_t x, int32_t y, unsigned int font_size, Align align, Layout layout, const char *str);

	uint32_t getTextWidth(const char *fmt, ...);
	uint32_t getTextHeight(const char *fmt, ...);

	unsigned int calculateFitFontSizeFmt(uint32_t limit_width, uint32_t limit_height, Layout layout, const char *fmt, ...);
	unsigned int calculateFitFontSize(uint32_t limit_width, uint32_t limit_height, Layout layout, const char *str);

	void showFreeTypeVersion();
	void showCredit();
	void getFreeTypeVersion(char *str);
	void getCredit(char *str);
	void setDebugLevel(uint8_t level);

	/*!
	 * @brief Collectively set up screen control functions.
	 * @tparam (T) Screen control class.
	 * @param[in] (&drawer) Instance for screen control.
	 * @ingroup rendering_api
	 * @attention To use this function, it must be possible to call the following four methods on the instance given as the argument.
	 * @attention `drawPixel(x, y, c)`, `drawFastHLine(x, y, w, c)`, `startWrite()`, `endWrite()`
	 * @note This function only calls the following four methods internally.
	 * @note setDrawPixel(), setDrawFastHLine(), setStartWrite(), setEndWrite()
	 */
	template <typename T>
	void setDrawer(T &drawer) {
		set_drawPixel([&](int32_t x, int32_t y, uint16_t c) { return drawer.drawPixel(x, y, c); });
		set_drawFastHLine([&](int32_t x, int32_t y, int32_t w, uint16_t c) { return drawer.drawFastHLine(x, y, w, c); });
		set_startWrite([&](void) { return drawer.startWrite(); });
		set_endWrite([&](void) { return drawer.endWrite(); });
	}

	// Direct calls are deprecated.
	void set_drawPixel(std::function<void(int32_t, int32_t, uint16_t)> user_func);
	void set_drawFastHLine(std::function<void(int32_t, int32_t, int32_t, uint16_t)> user_func);
	void set_startWrite(std::function<void(void)> user_func);
	void set_endWrite(std::function<void(void)> user_func);

	/* Static member method */
	/*!
	 * @brief Set up serial output control functions.
	 * @tparam (T) Serial control class.
	 * @param[in] (&output) Instance for serial control.
	 * @ingroup utility_api
	 * @attention To use this function, it must be possible to call the `print` method on the instance given as the argument.
	 * @note This function only calls the setPrintFunc() method internally.
	 */
	template <typename T>
	static void setSerial(T &output) {
		set_printFunc([&](const char *s) { return output.print(s); });
	}
	// Direct calls are deprecated.
	static void set_printFunc(std::function<void(const char *)> user_func);

	/*!
	 * @brief Structure for handling cursor position.
	 */
	struct Cursor {
		int32_t x; ///< x-coordinate
		int32_t y; ///< y-coordinate
	};

private:
	FT_Error loadFont(enum OFR::LoadFontFrom from);
	uint32_t getFontMaxHeight();
	void draw2screen(FT_BitmapGlyph glyph, uint32_t x, uint32_t y, uint16_t fg, uint16_t bg);
	uint16_t decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining);
	uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
	uint16_t alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc);

	std::function<void(int32_t, int32_t, uint16_t)> _drawPixel;
	std::function<void(int32_t, int32_t, int32_t, uint16_t)> _drawFastHLine;
	std::function<void(void)> _startWrite;
	std::function<void(void)> _endWrite;

	FTC_Manager _ftc_manager;
	FTC_CMapCache _ftc_cmap_cache;
	FTC_ImageCache _ftc_image_cache;

	OFR::FaceRec _face_id;

	struct Flags {
		bool enable_optimized_drawing;
		bool support_vertical;
	};
	struct Flags _flags;

	struct CacheParameter {
		unsigned int max_faces;
		unsigned int max_sizes;
		unsigned long max_bytes;
	};
	struct CacheParameter _cache;

	struct SavedStateVariables {
		struct Cursor drawn_bg_point;
		uint32_t prev_max_font_height;
		unsigned int prev_font_size;
	};
	struct SavedStateVariables _saved_state;

	struct TextParameter {
		double line_space_ratio;
		unsigned int size;
		uint16_t fg_color;
		uint16_t bg_color;
		struct Cursor cursor;
		Align align;
		BgFillMethod bg_fill_method;
		Layout layout;
	};
	struct TextParameter _text;

	uint8_t _debug_level;
};

#endif