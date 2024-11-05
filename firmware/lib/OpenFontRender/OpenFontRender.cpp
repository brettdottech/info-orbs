// -------------------------------------------------------
//  OpenFontRender.cpp
//
//  Copyright (c) 2021 takkaO
//
//  If you use, modify or redistribute this file as part of
//  the original repository, please follow the repository's license.
//
// -------------------------------------------------------

#include "OpenFontRender.h"

/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/
//
//  Data Structure Definition
//  (Use only within this file.)
//
/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/
/*! \cond PRIVATE */

typedef struct {
	enum OFR::LoadFontFrom from; // data source
	uint8_t debug_level;         // debug level
} FontDataInfo;

typedef struct {
	FT_Glyph glyph;
	FT_Vector pos;
} RenderStringInfo;

#ifdef FREERTOS_CONFIG_H
enum RenderTaskStatus {
	IDLE,
	LOCK,
	RENDERING,
	END_ERROR,
	END_SUCCESS
};

typedef struct {
	FTC_ImageCache ftc_image_cache; // cache
	FTC_ImageTypeRec image_type;    // parameter
	FT_UInt glyph_index;            // target index
	FT_Glyph aglyph;                // result
	FT_Error error;                 // ft_error
	uint8_t debug_level;            // debug level
} RenderTaskParameter;
#endif

/*! \endcond */

/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/
//
//  Function Definition
//  (See below more ditails.)
//
/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/
/*! \cond PRIVATE */

FT_Error ftc_face_requester(FTC_FaceID face_id,
                            FT_Library library,
                            FT_Pointer request_data,
                            FT_Face *face);
void debugPrintf(uint8_t level, const char *fmt, ...);

#ifdef FREERTOS_CONFIG_H
void RenderTask(void *pvParameters);
#endif

/*! \endcond */

/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/
//
//  Global Variables
//  (Use only within this file.)
//
/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/
/*! \cond PRIVATE */

FT_Library g_FtLibrary;
bool g_NeedInitialize                     = true;
std::function<void(const char *)> g_Print = [](const char *s) { return; };

#ifdef FREERTOS_CONFIG_H
TaskHandle_t g_RenderTaskHandle                   = NULL;
volatile enum RenderTaskStatus g_RenderTaskStatus = IDLE;
volatile bool g_UseRenderTask                     = (FREETYPE_MAJOR == 2 && FREETYPE_MINOR == 4 && FREETYPE_PATCH == 12) ? false : true;
unsigned int g_RenderTaskStackSize                = 8192 + 8192 + 4096;
RenderTaskParameter g_TaskParameter;
#endif

/*! \endcond */

/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/
//
//  OpenFontRender Class Public Methods
//
/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/

OpenFontRender::OpenFontRender() {
	// TODO: Automatic support some micro computers
	_drawPixel = [](int32_t x, int32_t y, uint16_t c) {
		static bool flag = true;
		if (flag) {
			g_Print("\n** [Warning] Please set drawPixel() using setDrawPixel(). **\n");
			flag = false;
		}
		return;
	};
	_drawFastHLine = [](int32_t x, int32_t y, int32_t w, uint16_t c) { return; };
	_startWrite    = []() { return; };
	_endWrite      = []() { return; };

	_cache.max_faces = OpenFontRender::CACHE_SIZE_MINIMUM;
	_cache.max_sizes = OpenFontRender::CACHE_SIZE_MINIMUM;
	_cache.max_bytes = OpenFontRender::CACHE_SIZE_MINIMUM;

	_face_id.filepath   = nullptr;
	_face_id.data       = nullptr;
	_face_id.data_size  = 0;
	_face_id.face_index = 0;

	_text.line_space_ratio = 1.0;    // Set default line space ratio
	_text.size             = 44;     // Set default font size
	_text.fg_color         = 0xFFFF; // Set default font color (White)
	_text.bg_color         = 0x0000; // Set default background color (Black)
	_text.align            = Align::Left;
	_text.bg_fill_method   = BgFillMethod::None;
	_text.layout           = Layout::Horizontal; // Set default layout Horizontal
	_debug_level           = OFR_NONE;

	_flags.enable_optimized_drawing = false;

	_ftc_manager     = nullptr;
	_ftc_cmap_cache  = nullptr;
	_ftc_image_cache = nullptr;

	_saved_state.drawn_bg_point       = {0, 0};
	_saved_state.prev_max_font_height = 0;
	_saved_state.prev_font_size       = 0;
}

/*!
 * @brief Set whether the rendering task should be performed independently or not.
 * @param[in] (enable) If true, makes the task independent.
 * @ingroup rtos_api
 * @note If FreeRTOS is not being used, it does nothing.
 * @note The default is `false` if the version of FreeType you are using is 2.4.12 (library default), and `true` for all other versions.
 */
void OpenFontRender::setUseRenderTask(bool enable) {
#ifdef FREERTOS_CONFIG_H
	g_UseRenderTask = enable;
#endif
}

/*!
 * @brief Specify the stack size for independent rendering tasks.
 * @param[in] (stack_size) Stack size used by the rendering task.
 * @ingroup rtos_api
 * @note If FreeRTOS is not being used, it does nothing.
 * @note Default value is 20480.
 */
void OpenFontRender::setRenderTaskStackSize(unsigned int stack_size) {
#ifdef FREERTOS_CONFIG_H
	g_RenderTaskStackSize = stack_size;
#endif
}

/*!
 * @brief Set cursor positions.
 * @param[in] (x) x-coordinate to set.
 * @param[in] (y) y-coordinate to set.
 * @ingroup cursor_api
 */
void OpenFontRender::setCursor(int32_t x, int32_t y) {
	_text.cursor = {x, y};
}

/*!
 * @brief Get current cursor x-coordinate.
 * @return Current cursor x-coordinate.
 * @ingroup cursor_api
 */
int32_t OpenFontRender::getCursorX() {
	return _text.cursor.x;
}

/*!
 * @brief Get current cursor y-coordinate.
 * @return Current cursor y-coordinate.
 * @ingroup cursor_api
 */
int32_t OpenFontRender::getCursorY() {
	return _text.cursor.y;
}

/*!
 * @brief Move cursor position.
 * @param[in] (delta_x) Amount to move the x-coordinate.
 * @param[in] (delta_y) Amount to move the y-coordinate.
 * @ingroup cursor_api
 */
void OpenFontRender::seekCursor(int32_t delta_x, int32_t delta_y) {
	_text.cursor.x += delta_x;
	_text.cursor.y += delta_y;
}

/*!
 * @brief Specify the text color.
 * @param[in] (font_color) 16 bit rgb color for text.
 * @ingroup font_api
 */
void OpenFontRender::setFontColor(uint16_t font_color) {
	_text.fg_color = font_color;
}

/*!
 * @brief Specify the background color.
 * @param[in] (font_bgcolor) 16 bit rgb color for background.
 * @ingroup font_api
 */
void OpenFontRender::setBackgroundColor(uint16_t font_bgcolor) {
	_text.bg_color = font_bgcolor;
}

/*!
 * @brief Specify the text color and background color.
 * @param[in] (font_color) 16 bit rgb color for text.
 * @param[in] (font_bgcolor) 16 bit rgb color for background.
 * @ingroup font_api
 */
void OpenFontRender::setFontColor(uint16_t font_color, uint16_t font_bgcolor) {
	_text.fg_color = font_color;
	_text.bg_color = font_bgcolor;
}

/*!
 * @brief Specify the text color.
 * @param[in] (r) Red color (8bit) for text.
 * @param[in] (g) Green color (8bit) for text.
 * @param[in] (b) Blue color (8bit) for text.
 * @ingroup font_api
 */
void OpenFontRender::setFontColor(uint8_t r, uint8_t g, uint8_t b) {
	_text.fg_color = color565(r, g, b);
}

/*!
 * @brief Specify the background color.
 * @param[in] (r) Red color (8bit) for background.
 * @param[in] (g) Green color (8bit) for background.
 * @param[in] (b) Blue color (8bit) for background.
 * @ingroup font_api
 */
void OpenFontRender::setBackgroundColor(uint8_t r, uint8_t g, uint8_t b) {
	_text.bg_color = color565(r, g, b);
}

/*!
 * @brief Specify the text color and background color..
 * @param[in] (fr) Red color (8bit) for text.
 * @param[in] (fg) Green color (8bit) for text.
 * @param[in] (fb) Blue color (8bit) for text.
 * @param[in] (br) Red color (8bit) for background.
 * @param[in] (bg) Green color (8bit) for background.
 * @param[in] (bb) Blue color (8bit) for background.
 * @ingroup font_api
 */
void OpenFontRender::setFontColor(uint8_t fr, uint8_t fg, uint8_t fb, uint8_t br, uint8_t bg, uint8_t bb) {
	_text.fg_color = color565(fr, fg, fb);
	_text.bg_color = color565(br, bg, bb);
}

/*!
 * @brief Get the current text color.
 * @return Current text color.
 * @ingroup font_api
 */
uint16_t OpenFontRender::getFontColor() {
	return _text.fg_color;
}

/*!
 * @brief Get the current background color.
 * @return Current background color.
 * @ingroup font_api
 */
uint16_t OpenFontRender::getBackgroundColor() {
	return _text.bg_color;
}

/*!
 * @brief Set the font size for rendering.
 * @param[in] (pixel) Font size.
 * @ingroup font_api
 * @note Default size is `44`.
 * @note It will draw larger characters in proportion to the size of the number you set.
 */
void OpenFontRender::setFontSize(unsigned int pixel) {
	_text.size = pixel;
}

/*!
 * @brief Get the current font size.
 * @return Current font size.
 * @ingroup font_api
 */
unsigned int OpenFontRender::getFontSize() {
	return _text.size;
}

/*!
 * @brief Adjusts the width between lines of text.
 * @param[in] (line_space_ratio) Correction rate for line spacing greater than 0.
 * @ingroup font_api
 * @note Default rate is `1.0`.
 * @note The larger the value, the wider the line spacing.
 */
double OpenFontRender::setLineSpaceRatio(double line_space_ratio) {
	if (line_space_ratio <= 0) {
		// Invalid argument
		return _text.line_space_ratio;
	}
	_text.line_space_ratio = line_space_ratio;
	return _text.line_space_ratio;
}

/*!
 * @brief Get the current line space ratio.
 * @return Current line space ratio.
 * @ingroup font_api
 */
double OpenFontRender::getLineSpaceRatio() {
	return _text.line_space_ratio;
}

/*!
 * @brief Set the background fill method.
 * @param[in] (method) Method of filling background.
 * @ingroup layout_api
 * @see BgFillMethod
 * @note Default is `None`.
 */
void OpenFontRender::setBackgroundFillMethod(BgFillMethod method) {
	_text.bg_fill_method = method;
}

/*!
 * @brief Get the current background fill method.
 * @return Current background fill method.
 * @ingroup layout_api
 * @see BgFillMethod
 */
BgFillMethod OpenFontRender::getBackgroundFillMethod() {
	return _text.bg_fill_method;
}

/*!
 * @brief Set the direction of text writing.
 * @param[in] (layout) Direction of text writing.
 * @ingroup layout_api
 * @see Layout
 * @note Default is `Horizontal`.
 * @attention `Vertical` is not yet supported (will be implemented in the future).
 */
void OpenFontRender::setLayout(Layout layout) {
	_text.layout = layout;
}

/*!
 * @brief Get the current direction of text writing.
 * @return Current direction of text writing.
 * @ingroup layout_api
 * @see Layout
 */
Layout OpenFontRender::getLayout() {
	return _text.layout;
}

/*!
 * @brief Set the text alignment.
 * @param[in] (align) Text alignment.
 * @ingroup layout_api
 * @see Align
 * @note Default is `TopLeft`.
 */
void OpenFontRender::setAlignment(Align align) {
	_text.align = align;
}

/*!
 * @brief Get the current text alignment.
 * @return Current text alignment.
 * @ingroup layout_api
 * @see Align
 */
Align OpenFontRender::getAlignment() {
	return _text.align;
}

/*!
 * @brief Set FreeType cache size.
 * @param[in] (max_faces) Maximum number of opened FT_Face objects.
 * @param[in] (max_sizes) Maximum number of opened FT_Size objects.
 * @param[in] (max_bytes) Maximum number of bytes to use for cached data.
 * @ingroup rendering_api
 * @see CACHE_SIZE_NO_LIMIT, CACHE_SIZE_MINIMUM
 * @note Default is `CACHE_SIZE_MINIMUM`.
 */
void OpenFontRender::setCacheSize(unsigned int max_faces, unsigned int max_sizes, unsigned long max_bytes) {
	_cache.max_faces = max_faces;
	_cache.max_sizes = max_sizes;
	_cache.max_bytes = max_bytes;
}

/*!
 * @brief Load font from memory.
 * @param[in] (*data) Font data array.
 * @param[in] (size) Font data array size.
 * @param[in] (target_face_index) Load font index. Default is 0.
 * @return FreeType error code. 0 is success.
 * @ingroup rendering_api
 */
FT_Error OpenFontRender::loadFont(const unsigned char *data, size_t size, uint8_t target_face_index) {
	_face_id.data_size = size;
	_face_id.data      = (unsigned char *)data;

	_face_id.face_index = target_face_index;
	return loadFont(OFR::FROM_MEMORY);
}

/*!
 * @brief Load font from external memory.
 * @param[in] (*fpath) Font file path.
 * @param[in] (target_face_index) Load font index. Default is 0.
 * @return FreeType error code. 0 is success.
 * @ingroup rendering_api
 * @note SD card access is strongly hardware dependent, so for hardware other than M5Stack and Wio Terminal,
 * @note you will need to add file manipulation functions to FileSupport.cpp/.h.
 * @note Any better solutions are welcome.
 */
FT_Error OpenFontRender::loadFont(const char *fpath, uint8_t target_face_index) {
	size_t len = strlen(fpath);

	_face_id.filepath = new char[len + 1]; // Release on unloadFont method
	strncpy(_face_id.filepath, fpath, len);
	_face_id.filepath[len] = '\0';

	_face_id.face_index = target_face_index;
	return loadFont(OFR::FROM_FILE);
}

/*!
 * @brief Unload font data.
 * @ingroup rendering_api
 */
void OpenFontRender::unloadFont() {
	if (!g_NeedInitialize) {
		FTC_Manager_RemoveFaceID(_ftc_manager, &_face_id);
		FTC_Manager_Reset(_ftc_manager);
		FTC_Manager_Done(_ftc_manager);
		FT_Done_FreeType(g_FtLibrary);

		delete[] _face_id.filepath;
	}
	g_NeedInitialize = true;
}

/*!
 * @brief Renders text horizontally.
 * @param[in] (*str) String to draw.
 * @param[in] (x) Drawing start position (x-coordinate).
 * @param[in] (y) Drawing start position (y-coordinate).
 * @param[in] (fg) 16 bit rgb color for text.
 * @param[in] (bg) 16 bit rgb color for background.
 * @param[in] (align) Text alignment.
 * @param[in] (drawing) Mode of draw to screen
 * @param[out] (&abbox) Bounding box around drawn text.
 * @param[out] (&error) Rendering error.
 * @return Number of characters success to write.
 * @ingroup rendering_api
 * @note Direct calls to this function are not recommended. This is because it complicates the call.
 * @note Instead, it is recommended to use the `printf` and `drawString` functions in combination with optional methods such as `setFontColor` and `setAlignment` function.
 */
uint16_t OpenFontRender::drawHString(const char *str,
                                     int32_t x,
                                     int32_t y,
                                     uint16_t fg,
                                     uint16_t bg,
                                     Align align,
                                     Drawing drawing,
                                     FT_BBox &abbox,
                                     FT_Error &error) {

	uint16_t written_char_num    = 0;
	Cursor initial_position      = {x, y};
	Cursor current_line_position = {x, y};
	FT_Pos ascender              = 0;
	bool detect_control_char     = false;

	abbox.xMin = abbox.yMin = LONG_MAX;
	abbox.xMax = abbox.yMax = LONG_MIN;

	FTC_ImageTypeRec image_type;
	image_type.face_id = &_face_id;
	image_type.width   = 0;
	image_type.height  = _text.size;
	image_type.flags   = FT_LOAD_DEFAULT;

	// decode UTF8
	uint16_t unicode = '\0';
	std::queue<FT_UInt32> unicode_q;
	{
		uint16_t len = (uint16_t)strlen(str);
		uint16_t n   = 0;
		while (n < len) {
			unicode = decodeUTF8((uint8_t *)str, &n, len - n);
			unicode_q.push(unicode);
		}
	}

	FT_Int cmap_index;
	{
		FT_Size asize = NULL;
		FTC_ScalerRec scaler;
		scaler.face_id = &_face_id;
		scaler.width   = 0;
		scaler.height  = _text.size;
		scaler.pixel   = true;
		scaler.x_res   = 0;
		scaler.y_res   = 0;

		error = FTC_Manager_LookupSize(_ftc_manager, &scaler, &asize);
		if (error) {
			return written_char_num;
		}
		cmap_index = FT_Get_Charmap_Index(asize->face->charmap);
		ascender   = asize->face->size->metrics.ascender;
	}

	// Rendering loop
	while (unicode_q.size() != 0) {
		FT_Vector offset       = {0, 0};
		FT_Vector bearing_left = {0, 0};
		std::queue<FT_UInt32> rendering_unicode_q;
		FT_BBox bbox;
		bbox.xMin = bbox.yMin = LONG_MAX;
		bbox.xMax = bbox.yMax = LONG_MIN;

		detect_control_char   = false;
		current_line_position = {x, y};
		image_type.flags      = FT_LOAD_DEFAULT;
		bool isLineFirstChar  = true;

		// Glyph extraction
		while (unicode_q.size() != 0 && detect_control_char == false) {
			FT_Glyph aglyph;
			FT_UInt glyph_index;
			FT_BBox glyph_bbox;

			unicode = unicode_q.front();
			switch (unicode) {
			case '\r':
				[[fallthrough]]; // Fall Through
			case '\n':
				detect_control_char = true;
				break;
			default:
				glyph_index = FTC_CMapCache_Lookup(_ftc_cmap_cache,
				                                   &_face_id,
				                                   cmap_index,
				                                   unicode);

				error = FTC_ImageCache_Lookup(_ftc_image_cache, &image_type, glyph_index, &aglyph, NULL);
				if (error) {
					debugPrintf((_debug_level & OFR_ERROR), "FTC_ImageCache_Lookup error: 0x%02X\n", error);
					return written_char_num;
				}

				FT_Glyph_Get_CBox(aglyph, FT_GLYPH_BBOX_PIXELS, &glyph_bbox);
				if (isLineFirstChar == true) {
					// Get bearing X
					FT_Face aface;
					FTC_Manager_LookupFace(_ftc_manager, &_face_id, &aface);
					bearing_left.x = (aface->glyph->metrics.horiBearingX >> 6);
					// nothing to do for bearing.y
					isLineFirstChar = false;
				}

				// Move coordinates on the grid
				glyph_bbox.xMin += x;
				glyph_bbox.xMax += x;
				glyph_bbox.yMin += y;
				glyph_bbox.yMax += y;

				// Merge bbox
				bbox.xMin = std::min(bbox.xMin, glyph_bbox.xMin);
				bbox.yMin = std::min(bbox.yMin, glyph_bbox.yMin);
				bbox.xMax = std::max(bbox.xMax, glyph_bbox.xMax);
				bbox.yMax = std::max(bbox.yMax, glyph_bbox.yMax);

				x += (aglyph->advance.x >> 16);
				rendering_unicode_q.push(unicode);
			}
			unicode_q.pop();
		}

		// Serial.printf("bbox1: x=%f %f, y=%f %f\n", bbox.xMin, bbox.xMax, bbox.yMin, bbox.yMax);
		// Check that we really grew the string bbox
		if (bbox.xMin > bbox.xMax) {
			// Failed
			bbox.xMin = bbox.yMin = 0;
			bbox.xMax = bbox.yMax = 0;
		} else {
			// Transform coordinate space differences
			bbox.yMax = y - (bbox.yMax - y) + ((ascender) >> 6);
			bbox.yMin = y + (y - bbox.yMin) + ((ascender) >> 6);
			if (bbox.yMax < bbox.yMin) {
				std::swap(bbox.yMax, bbox.yMin);
			}
			// Correct slight misalignment of X-axis
			offset.x = bbox.xMin - current_line_position.x;
		}
		// Serial.printf("bbox2: x=%f %f, y=%f %f\n", bbox.xMin, bbox.xMax, bbox.yMin, bbox.yMax);

		// Calculate alignment offset
		switch (align) {
		case Align::Left:
			// Fallthrough
		case Align::TopLeft:
			// Nothing to do
			offset.x -= bearing_left.x;
			break;
		case Align::MiddleLeft:
			offset.x -= bearing_left.x;
			offset.y += ((bbox.yMax - bbox.yMin) / 2);
			break;
		case Align::BottomLeft:
			offset.x -= bearing_left.x;
			offset.y += (bbox.yMax - bbox.yMin);
			break;
		case Align::Center:
			// Fallthrough
		case Align::TopCenter:
			offset.x += ((bbox.xMax - bbox.xMin) / 2);
			offset.x -= bearing_left.x;
			current_line_position.x -= (bearing_left.x / 2);
			break;
		case Align::MiddleCenter:
			offset.x += ((bbox.xMax - bbox.xMin) / 2);
			offset.x -= bearing_left.x;
			current_line_position.x -= (bearing_left.x / 2);

			offset.y += ((bbox.yMax - bbox.yMin) / 2);
			break;
		case Align::BottomCenter:
			offset.x += ((bbox.xMax - bbox.xMin) / 2);
			offset.x -= bearing_left.x;
			current_line_position.x -= (bearing_left.x / 2);

			offset.y += (bbox.yMax - bbox.yMin);
			break;
		case Align::Right:
			// Fallthrough
		case Align::TopRight:
			offset.x += (bbox.xMax - bbox.xMin);
			offset.x -= bearing_left.x;
			current_line_position.x -= bearing_left.x;
			break;
		case Align::MiddleRight:
			offset.x += (bbox.xMax - bbox.xMin);
			offset.x -= bearing_left.x;
			current_line_position.x -= bearing_left.x;

			offset.y += ((bbox.yMax - bbox.yMin) / 2);
			break;
		case Align::BottomRight:
			offset.x += (bbox.xMax - bbox.xMin);
			offset.x -= bearing_left.x;
			current_line_position.x -= bearing_left.x;

			offset.y += (bbox.yMax - bbox.yMin);
			break;
		default:
			break;
		}

		bbox.xMin -= offset.x;
		bbox.xMax -= offset.x;
		bbox.yMin -= offset.y;
		bbox.yMax -= offset.y;

		// Serial.printf("bbox3: x=%f %f, y=%f %f\n", bbox.xMin, bbox.xMax, bbox.yMin, bbox.yMax);

		if (_text.bg_fill_method == BgFillMethod::Block) {
			// Clear the area to be rendering if fill method is Block
			if (_flags.enable_optimized_drawing) {
				// We can use fastHLine method
				for (int _y = bbox.yMin; _y <= bbox.yMax; _y++) {
					_drawFastHLine(bbox.xMin, _y, bbox.xMax - bbox.xMin, bg);
				}
			} else {
				for (int _y = bbox.yMin; _y <= bbox.yMax; _y++) {
					for (int _x = bbox.xMin; _x <= bbox.xMax; _x++) {
						_drawPixel(_x, _y, bg);
					}
				}
			}
		}

		// Restore coordinates
		x = current_line_position.x;
		y = current_line_position.y;

		if (drawing == Drawing::Execute) {
			image_type.flags              = FT_LOAD_RENDER;
			_saved_state.drawn_bg_point.x = (x - offset.x);
			uint16_t rendering_unicode;

			while (rendering_unicode_q.size() != 0) {
				rendering_unicode = rendering_unicode_q.front();

				FT_UInt glyph_index = FTC_CMapCache_Lookup(_ftc_cmap_cache,
				                                           &_face_id,
				                                           cmap_index,
				                                           rendering_unicode);
				FT_Glyph aglyph;
#ifdef FREERTOS_CONFIG_H
				if (g_UseRenderTask) {
					if (g_RenderTaskHandle == NULL) {
						debugPrintf((_debug_level & OFR_INFO), "Create render task\n");
						const uint8_t RUNNING_CORE = 1;
						const uint8_t PRIORITY     = 1;
						xTaskCreateUniversal(RenderTask,
						                     "RenderTask",
						                     g_RenderTaskStackSize, // Seems to need a lot of memory.
						                     NULL,
						                     PRIORITY,
						                     &g_RenderTaskHandle,
						                     RUNNING_CORE);
					}
					while (g_RenderTaskStatus != IDLE) {
						vTaskDelay(1);
					}
					g_RenderTaskStatus              = LOCK;
					g_TaskParameter.ftc_image_cache = _ftc_image_cache;
					g_TaskParameter.image_type      = image_type;
					g_TaskParameter.glyph_index     = glyph_index;
					g_TaskParameter.debug_level     = _debug_level;

					g_RenderTaskStatus = RENDERING;
					while (g_RenderTaskStatus == RENDERING) {
						vTaskDelay(1);
					}
					debugPrintf((g_TaskParameter.debug_level & OFR_INFO), "Render task Finish\n");
					aglyph             = g_TaskParameter.aglyph;
					error              = g_TaskParameter.error;
					g_RenderTaskStatus = IDLE;
				} else {
					error = FTC_ImageCache_Lookup(_ftc_image_cache, &image_type, glyph_index, &aglyph, NULL);
				}
#else
				error = FTC_ImageCache_Lookup(_ftc_image_cache, &image_type, glyph_index, &aglyph, NULL);
#endif
				if (error) {
					debugPrintf((_debug_level & OFR_ERROR), "FTC_ImageCache_Lookup error: 0x%02X\n", error);
					return written_char_num;
				}
				FT_Vector pos = {(x - offset.x), (y - offset.y)};
				// Change baseline to left top
				pos.y += ((ascender) >> 6);

				{
					FT_BitmapGlyph bit = (FT_BitmapGlyph)aglyph;

					draw2screen(bit, pos.x, pos.y, fg, bg);

					if (_saved_state.drawn_bg_point.x < (pos.x + (uint32_t)bit->bitmap.width)) {
						_saved_state.drawn_bg_point.x = pos.x + (uint32_t)bit->bitmap.width;
					}

					written_char_num++;
				}
				x += (aglyph->advance.x >> 16);

				rendering_unicode_q.pop();
			}
		}

		if (detect_control_char) {
			switch (unicode) {
			case '\r':
				x = initial_position.x;
				break;
			case '\n':
				x = initial_position.x;
				y += (int32_t)(getFontMaxHeight() * _text.line_space_ratio);
				break;
			default:
				// No supported control char
				break;
			}
		}

		// Merge bbox
		abbox.xMin = std::min(bbox.xMin, abbox.xMin);
		abbox.yMin = std::min(bbox.yMin, abbox.yMin);
		abbox.xMax = std::max(bbox.xMax, abbox.xMax);
		abbox.yMax = std::max(bbox.yMax, abbox.yMax);
	} // End of rendering loop

	if (detect_control_char && unicode == '\n') {
		// If string end with '\n' control char, expand bbox
		abbox.yMax += (int32_t)(getFontMaxHeight() * _text.line_space_ratio);
	}

	_text.cursor = {x, y};
	return written_char_num;
}

/*!
 * @brief Render single character.
 * @param[in] (character) Character to draw.
 * @param[in] (x) Drawing start position (x-coordinate).
 * @param[in] (y) Drawing start position (y-coordinate).
 * @param[in] (fg) 16 bit rgb color for text.
 * @param[in] (bg) 16 bit rgb color for background.
 * @param[in] (align) Text alignment.
 * @return Rendering error.
 * @ingroup rendering_api
 */
FT_Error OpenFontRender::drawChar(char character,
                                  int32_t x,
                                  int32_t y,
                                  uint16_t fg,
                                  uint16_t bg,
                                  Align align) {
	FT_Error error;
	FT_BBox bbox;
	const char str[2] = {character, '\0'};

	drawHString(str, x, y, fg, bg, align, Drawing::Execute, bbox, error);

	return FT_Err_Ok;
}

/*!
 * @brief Renders text.
 * @param[in] (*str) String to draw.
 * @param[in] (x) Drawing start position (x-coordinate).
 * @param[in] (y) Drawing start position (y-coordinate).
 * @param[in] (fg) 16 bit rgb color for text.
 * @param[in] (bg) 16 bit rgb color for background.
 * @param[in] (layout) Direction of writing.
 * @return Number of characters success to write.
 * @ingroup rendering_api
 * @attention `Vertical` is not yet supported (will be implemented in the future).
 */
uint16_t OpenFontRender::drawString(const char *str,
                                    int32_t x,
                                    int32_t y,
                                    uint16_t fg,
                                    uint16_t bg,
                                    Layout layout) {
	FT_Error error;
	FT_BBox bbox;

	switch (layout) {
	case Layout::Horizontal:
		return drawHString(str, x, y, fg, bg, _text.align, Drawing::Execute, bbox, error);
	case Layout::Vertical:
		// Not support now
		return 0;
	default:
		return 0;
	}
}

/*!
 * @brief Renders text as Top-Center.
 * @param[in] (*str) String to draw.
 * @param[in] (x) Drawing start position (x-coordinate).
 * @param[in] (y) Drawing start position (y-coordinate).
 * @param[in] (fg) 16 bit rgb color for text.
 * @param[in] (bg) 16 bit rgb color for background.
 * @param[in] (layout) Direction of writing.
 * @return Number of characters success to write.
 * @ingroup rendering_api
 * @warning `Vertical` is not yet supported (will be implemented in the future).
 * @deprecated It is recommended to use the `printf` or `drawString` functions in combination with optional methods such as `setAlignment` function.
 * @deprecated It will be removed in the future.
 */
uint16_t OpenFontRender::cdrawString(const char *str,
                                     int32_t x,
                                     int32_t y,
                                     uint16_t fg,
                                     uint16_t bg,
                                     Layout layout) {
	FT_Error error;
	FT_BBox bbox;

	switch (layout) {
	case Layout::Horizontal:
		return drawHString(str, x, y, fg, bg, Align::Center, Drawing::Execute, bbox, error);
	case Layout::Vertical:
		// Not support now
		return 0;
	default:
		return 0;
	}
}

/*!
 * @brief Renders text as Top-Right.
 * @param[in] (*str) String to draw.
 * @param[in] (x) Drawing start position (x-coordinate).
 * @param[in] (y) Drawing start position (y-coordinate).
 * @param[in] (fg) 16 bit rgb color for text.
 * @param[in] (bg) 16 bit rgb color for background.
 * @param[in] (layout) Direction of writing.
 * @return Number of characters success to write.
 * @ingroup rendering_api
 * @warning `Vertical` is not yet supported (will be implemented in the future).
 * @deprecated It is recommended to use the `printf` or `drawString` functions in combination with optional methods such as `setAlignment` function.
 * @deprecated It will be removed in the future.
 */
uint16_t OpenFontRender::rdrawString(const char *str,
                                     int32_t x,
                                     int32_t y,
                                     uint16_t fg,
                                     uint16_t bg,
                                     Layout layout) {
	FT_Error error;
	FT_BBox bbox;

	switch (layout) {
	case Layout::Horizontal:
		return drawHString(str, x, y, fg, bg, Align::Right, Drawing::Execute, bbox, error);
	case Layout::Vertical:
		// Not support now
		return 0;
	default:
		return 0;
	}
}

/*!
 * @brief Renders text with format specifier.
 * @param[in] (*fmt) Format specifier.
 * @param[in] (...) Arguments for format specifier.
 * @return Number of characters success to write.
 * @ingroup rendering_api
 */
uint16_t OpenFontRender::printf(const char *fmt, ...) {
	char str[256] = {0};
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(str, 256, fmt, ap);
	va_end(ap);

	return drawString(str, _text.cursor.x, _text.cursor.y, _text.fg_color, _text.bg_color, _text.layout);
}

/*!
 * @brief Renders text as Top-Center with format specifier.
 * @param[in] (*fmt) Format specifier.
 * @param[in] (...) Arguments for format specifier.
 * @return Number of characters success to write.
 * @ingroup rendering_api
 * @deprecated It is recommended to use the `printf` or `drawString` functions in combination with optional methods such as `setAlignment` function.
 * @deprecated It will be removed in the future.
 */
uint16_t OpenFontRender::cprintf(const char *fmt, ...) {
	char str[256] = {0};
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(str, 256, fmt, ap);
	va_end(ap);

	return cdrawString(str, _text.cursor.x, _text.cursor.y, _text.fg_color, _text.bg_color, _text.layout);
}

/*!
 * @brief Renders text as Top-Right with format specifier.
 * @param[in] (*fmt) Format specifier.
 * @param[in] (...) Arguments for format specifier.
 * @return Number of characters success to write.
 * @ingroup rendering_api
 * @deprecated It is recommended to use the `printf` or `drawString` functions in combination with optional methods such as `setAlignment` function.
 * @deprecated It will be removed in the future.
 */
uint16_t OpenFontRender::rprintf(const char *fmt, ...) {
	char str[256] = {0};
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(str, 256, fmt, ap);
	va_end(ap);

	return rdrawString(str, _text.cursor.x, _text.cursor.y, _text.fg_color, _text.bg_color, _text.layout);
}

/*!
 * @brief Calculate text bounding box with format specifier.
 * @param[in] (x) Drawing start position (x-coordinate).
 * @param[in] (y) Drawing start position (y-coordinate).
 * @param[in] (font_size) Font size.
 * @param[in] (align) Text alignment.
 * @param[in] (layout) Direction of writing.
 * @param[in] (*fmt) Format specifier.
 * @param[in] (...) Arguments for format specifier.
 * @return Smallest rectangle that encloses the drawn string.
 * @ingroup rendering_api
 */
FT_BBox OpenFontRender::calculateBoundingBoxFmt(int32_t x,
                                                int32_t y,
                                                unsigned int font_size,
                                                Align align,
                                                Layout layout,
                                                const char *fmt, ...) {
	char str[256] = {0};
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(str, 256, fmt, ap);
	va_end(ap);

	return calculateBoundingBox(x, y, font_size, align, layout, (const char *)str);
}

/*!
 * @brief Calculate text bounding box.
 * @param[in] (x) Drawing start position (x-coordinate).
 * @param[in] (y) Drawing start position (y-coordinate).
 * @param[in] (font_size) Font size.
 * @param[in] (align) Text alignment.
 * @param[in] (layout) Direction of writing.
 * @param[in] (*str) Target string.
 * @return Smallest rectangle that encloses the drawn string.
 * @ingroup rendering_api
 */
FT_BBox OpenFontRender::calculateBoundingBox(int32_t x,
                                             int32_t y,
                                             unsigned int font_size,
                                             Align align,
                                             Layout layout,
                                             const char *str) {
	FT_Error error;
	FT_BBox bbox               = {0, 0, 0, 0};
	unsigned int tmp_font_size = getFontSize();
	Cursor tmp_cursor          = _text.cursor;

	_text.cursor = {x, y};
	setFontSize(font_size);

	switch (layout) {
	case Layout::Horizontal:
		drawHString(str, x, y, 0xFFFF, 0x0000, align, Drawing::Skip, bbox, error);
		break;
	case Layout::Vertical:
		// Not support now
		break;
	default:
		break;
	}

	setFontSize(tmp_font_size);
	_text.cursor = tmp_cursor;

	return bbox;
}

/*!
 * @brief Calculate text width.
 * @param[in] (*fmt) Format specifier.
 * @param[in] (...) Arguments for format specifier.
 * @return Text width.
 * @ingroup rendering_api
 * @note This process takes time.
 */
uint32_t OpenFontRender::getTextWidth(const char *fmt, ...) {
	char str[256] = {0};
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(str, 256, fmt, ap);
	va_end(ap);

	FT_BBox bbox = calculateBoundingBox(0, 0, _text.size, Align::Left, _text.layout, str);
	return (bbox.xMax - bbox.xMin);
}

/*!
 * @brief Calculate text height.
 * @param[in] (*fmt) Format specifier.
 * @param[in] (...) Arguments for format specifier.
 * @return Text height.
 * @ingroup rendering_api
 * @note This process takes time.
 */
uint32_t OpenFontRender::getTextHeight(const char *fmt, ...) {
	char str[256] = {0};
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(str, 256, fmt, ap);
	va_end(ap);

	FT_BBox bbox = calculateBoundingBox(0, 0, _text.size, Align::Left, _text.layout, str);
	return (bbox.yMax - bbox.yMin);
}

/*!
 * @brief Calculates the maximum font size that will fit the specified format string and the specified rectangle.
 * @param[in] (limit_width) Limit width size.
 * @param[in] (limit_height) Limit height size.
 * @param[in] (layout) Direction of text writing.
 * @param[in] (*fmt) Format specifier.
 * @param[in] (...) Arguments for format specifier.
 * @return Calculated font size.
 * @ingroup rendering_api
 * @note This process takes time.
 * @attention Note that the calculated font size may not exactly meet the size limit.
 * @attention This is because the font size is not strictly measured, but only "estimated" by calculation.
 */
unsigned int OpenFontRender::calculateFitFontSizeFmt(uint32_t limit_width,
                                                     uint32_t limit_height,
                                                     Layout layout,
                                                     const char *fmt, ...) {
	char str[256] = {0};
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(str, 256, fmt, ap);
	va_end(ap);

	return calculateFitFontSize(limit_width, limit_height, layout, (const char *)str);
}

/*!
 * @brief Calculates the maximum font size that will fit the specified string and the specified rectangle.
 * @param[in] (limit_width) Limit width size.
 * @param[in] (limit_height) Limit height size.
 * @param[in] (layout) Direction of text writing.
 * @param[in] (*str) Target string.
 * @return Calculated font size.
 * @ingroup rendering_api
 * @note This process takes time.
 * @attention Note that the calculated font size may not exactly meet the size limit.
 * @attention This is because the font size is not strictly measured, but only "estimated" by calculation.
 */
unsigned int OpenFontRender::calculateFitFontSize(uint32_t limit_width,
                                                  uint32_t limit_height,
                                                  Layout layout,
                                                  const char *str) {
	FT_Error error;
	FT_BBox bbox1              = {0, 0, 0, 0};
	FT_BBox bbox2              = {0, 0, 0, 0};
	unsigned int tmp_font_size = getFontSize();
	Cursor tmp_cursor          = _text.cursor;
	unsigned int fs1           = 10;
	unsigned int fs2           = 50;
	int32_t w1, w2, h1, h2;

	// point1
	setFontSize(fs1);
	switch (layout) {
	case Layout::Horizontal:
		drawHString(str, 0, 0, 0xFFFF, 0x0000, Align::Left, Drawing::Skip, bbox1, error);
		break;
	case Layout::Vertical:
		// Not support now
		break;
	default:
		setFontSize(tmp_font_size);
		_text.cursor = tmp_cursor;
		return 0;
	}
	w1 = bbox1.xMax - bbox1.xMin;
	h1 = bbox1.yMax - bbox1.yMin;

	// point2
	setFontSize(fs2);
	switch (layout) {
	case Layout::Horizontal:
		drawHString(str, 0, 0, 0xFFFF, 0x0000, Align::Left, Drawing::Skip, bbox2, error);
		break;
	case Layout::Vertical:
		// Not support now
		break;
	default:
		setFontSize(tmp_font_size);
		_text.cursor = tmp_cursor;
		return 0;
	}
	w2 = bbox2.xMax - bbox2.xMin;
	h2 = bbox2.yMax - bbox2.yMin;

	unsigned int wfs = (unsigned int)(((fs2 - fs1) / (w2 - w1 + 0.000001)) * (limit_width - w1) + fs1);
	unsigned int hfs = (unsigned int)(((fs2 - fs1) / (h2 - h1 + 0.000001)) * (limit_height - h1) + fs1);

	setFontSize(tmp_font_size);
	_text.cursor = tmp_cursor;

	return std::min(wfs, hfs);
}

/*!
 * @brief Show using FreeType version.
 * @ingroup utility_api
 * @see setSerial, setPrintFunc
 * @note Need to set external output function.
 * @note Use the `setSerial` or `setPrintFunc` function to set the external output function.
 */
void OpenFontRender::showFreeTypeVersion() {
	char s[OpenFontRender::FT_VERSION_STRING_SIZE] = {0};
	getFreeTypeVersion(s);
	g_Print(s);
}

/*!
 * @brief Show FreeType credit.
 * @ingroup utility_api
 * @see setSerial, setPrintFunc
 * @note Need to set external output function.
 * @note Use the `setSerial` or `setPrintFunc` function to set the external output function.
 */
void OpenFontRender::showCredit() {
	char s[OpenFontRender::CREDIT_STRING_SIZE] = {0};
	getCredit(s);
	g_Print(s);
}

/*!
 * @brief Get using FreeType version text.
 * @ingroup utility_api
 * @param[out] (*str) FreeType version string.
 * @note The minimum string length required can be obtained with `OpenFontRender::FT_VERSION_STRING_SIZE`.
 * @attention An array of sufficient length must be passed as an argument.
 */
void OpenFontRender::getFreeTypeVersion(char *str) {
	snprintf(str, OpenFontRender::FT_VERSION_STRING_SIZE,
	         "FreeType version: %d.%d.%d\n", FREETYPE_MAJOR, FREETYPE_MINOR, FREETYPE_PATCH);
}

/*!
 * @brief Get using FreeType credit text.
 * @ingroup utility_api
 * @param[out] (*str) FreeType credit string.
 * @note The minimum string length required can be obtained with `OpenFontRender::CREDIT_STRING_SIZE`.
 * @attention An array of sufficient length must be passed as an argument.
 */
void OpenFontRender::getCredit(char *str) {
	snprintf(str, OpenFontRender::CREDIT_STRING_SIZE,
	         "Portions of this software are copyright (c) < %d.%d.%d > The FreeTypeProject (www.freetype.org).  All rights reserved.\n",
	         FREETYPE_MAJOR, FREETYPE_MINOR, FREETYPE_PATCH);
}

/*!
 * @brief Set debug output level.
 * @param[out] (level) Debug level.
 * @ingroup utility_api
 * @note The OFR_DEBUG_LEVEL enumeration is recommended for specifying the debug level.
 * @note Multiple levels can be specified for debugging levels simultaneously using the `OR` operation.
 */
void OpenFontRender::setDebugLevel(uint8_t level) {
	_debug_level = level;
}

/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/
//
//  OpenFontRender Class Public Methods
//  ( Direct calls are deprecated. )
//
/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/
/*! \cond PRIVATE */

void OpenFontRender::set_drawPixel(std::function<void(int32_t, int32_t, uint16_t)> user_func) {
	_drawPixel = user_func;
}
void OpenFontRender::set_drawFastHLine(std::function<void(int32_t, int32_t, int32_t, uint16_t)> user_func) {
	_drawFastHLine                  = user_func;
	_flags.enable_optimized_drawing = true; // Enable optimized drawing method
}
void OpenFontRender::set_startWrite(std::function<void(void)> user_func) {
	_startWrite = user_func;
}
void OpenFontRender::set_endWrite(std::function<void(void)> user_func) {
	_endWrite = user_func;
}
void OpenFontRender::set_printFunc(std::function<void(const char *)> user_func) {
	// This function is static member method
	g_Print = user_func;
}

/*! \endcond */

/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/
//
//  OpenFontRender Class Private Methods
//
/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/

FT_Error OpenFontRender::loadFont(enum OFR::LoadFontFrom from) {
	FT_Face face;
	FT_Error error;
	FontDataInfo info = {from, _debug_level};

	if (g_NeedInitialize) {
		error = FT_Init_FreeType(&g_FtLibrary);
		if (error) {
			debugPrintf((_debug_level & OFR_ERROR), "FT_Init_FreeType error: 0x%02X\n", error);
			return error;
		}
		g_NeedInitialize = false;
	}

	// 現在の引数は適当
	error = FTC_Manager_New(g_FtLibrary, _cache.max_faces, _cache.max_sizes, _cache.max_bytes, &ftc_face_requester, &info, &_ftc_manager);
	if (error) {
		debugPrintf((_debug_level & OFR_ERROR), "FTC_Manager_New error: 0x%02X\n", error);
		return error;
	}

	error = FTC_Manager_LookupFace(_ftc_manager, &_face_id, &face);
	if (error) {
		debugPrintf((_debug_level & OFR_ERROR), "FTC_Manager_LookupFace error: 0x%02X\n", error);
		return error;
	}

	error = FTC_CMapCache_New(_ftc_manager, &_ftc_cmap_cache);
	if (error) {
		debugPrintf((_debug_level & OFR_ERROR), "FTC_CMapCache_New error: 0x%02X\n", error);
		return error;
	}

	error = FTC_ImageCache_New(_ftc_manager, &_ftc_image_cache);
	if (error) {
		debugPrintf((_debug_level & OFR_ERROR), "FTC_ImageCache_New error: 0x%02X\n", error);
		return error;
	}

	if (FT_HAS_VERTICAL(face) == 0) {
		// Current font does NOT support vertical layout
		_flags.support_vertical = false;
	} else {
		_flags.support_vertical = true;
	}

	return FT_Err_Ok;
}

uint32_t OpenFontRender::getFontMaxHeight() {
	FT_Error error;
	FT_Face face;
	FTC_ScalerRec scaler;
	FT_Size asize = NULL;

	if (_saved_state.prev_font_size == _text.size) {
		return _saved_state.prev_max_font_height;
	}

	scaler.face_id = &_face_id;
	scaler.width   = 0;
	scaler.height  = _text.size;
	scaler.pixel   = true;
	scaler.x_res   = 0;
	scaler.y_res   = 0;

	// error = FTC_Manager_LookupFace(_ftc_manager, (FTC_FaceID)_face_id, &face);
	error = FTC_Manager_LookupSize(_ftc_manager, &scaler, &asize);
	if (error) {
		return 0;
	}
	face = asize->face;

	int bbox_ymax = FT_MulFix(face->bbox.yMax, face->size->metrics.y_scale) >> 6;
	int bbox_ymin = FT_MulFix(face->bbox.yMin, face->size->metrics.y_scale) >> 6;

	_saved_state.prev_max_font_height = (bbox_ymax - bbox_ymin);
	_saved_state.prev_font_size       = _text.size;
	return _saved_state.prev_max_font_height;
}

void OpenFontRender::draw2screen(FT_BitmapGlyph glyph, uint32_t x, uint32_t y, uint16_t fg, uint16_t bg) {
	_startWrite();

	if (_flags.enable_optimized_drawing) {
		// Start of new render code for efficient rendering of pixel runs to a TFT
		// Background fill code commented out thus //-bg-// as it is only filling the glyph bounding box
		// Code for this will need to track the last background end x as glyphs may overlap
		// Ideally need to keep track of the cursor position and use the font height for the fill box
		// Could also then use a line buffer for the glyph image (entire glyph buffer could be large!)

		int16_t fxs = x; // Start point of foreground x-coordinate
		uint32_t fl = 0; // Foreground line length
		int16_t bxs = x; // Start point of background x-coordinate
		uint32_t bl = 0; // Background line length

		for (int32_t _y = 0; _y < glyph->bitmap.rows; ++_y) {
			for (int32_t _x = 0; _x < glyph->bitmap.width; ++_x) {
				uint8_t alpha = glyph->bitmap.buffer[_y * glyph->bitmap.pitch + _x];
				debugPrintf((_debug_level & OFR_DEBUG) ? OFR_RAW : OFR_NONE, "%c", (alpha == 0x00 ? ' ' : 'o'));

				if (alpha) {
					if (bl) {
						_drawFastHLine(bxs, _y + y - glyph->top, bl, bg);
						bl = 0;
					}
					if (alpha != 0xFF) {
						// Draw anti-aliasing area
						if (fl) {
							if (fl == 1) {
								_drawPixel(fxs, _y + y - glyph->top, fg);
							} else {
								_drawFastHLine(fxs, _y + y - glyph->top, fl, fg);
							}
							fl = 0;
						}
						_drawPixel(_x + x + glyph->left, _y + y - glyph->top, alphaBlend(alpha, fg, bg));
					} else {
						if (fl == 0) {
							fxs = _x + x + glyph->left;
						}
						fl++;
					}
				} else {
					if (fl) {
						_drawFastHLine(fxs, _y + y - glyph->top, fl, fg);
						fl = 0;
					}
					if (_text.bg_fill_method == BgFillMethod::Minimum) {
						if (_saved_state.drawn_bg_point.x <= (x + _x)) {
							if (bl == 0) {
								bxs = _x + x + glyph->left;
							}
							bl++;
						}
					}
				}
				// End of new render code
			}

			if (fl) {
				_drawFastHLine(fxs, _y + y - glyph->top, fl, fg);
				fl = 0;
			} else if (bl) {
				_drawFastHLine(bxs, _y + y - glyph->top, bl, bg);
				bl = 0;
			}
			debugPrintf((_debug_level & OFR_DEBUG) ? OFR_RAW : OFR_NONE, "\n");
		}
	} else {
		// The old draw method
		// Not optimized, but can work with minimal method definitions
		for (int32_t _y = 0; _y < glyph->bitmap.rows; ++_y) {
			for (int32_t _x = 0; _x < glyph->bitmap.width; ++_x) {
				uint8_t alpha = glyph->bitmap.buffer[_y * glyph->bitmap.pitch + _x];
				debugPrintf((_debug_level & OFR_DEBUG) ? OFR_RAW : OFR_NONE, "%c", (alpha == 0x00 ? ' ' : 'o'));

				if (alpha) {
					_drawPixel(_x + x + glyph->left, _y + y - glyph->top, alphaBlend(alpha, fg, bg));
				} else if (_text.bg_fill_method == BgFillMethod::Minimum) {
					if (_saved_state.drawn_bg_point.x <= (x + _x)) {
						_drawPixel(_x + x + glyph->left, _y + y - glyph->top, bg);
					}
				}
			}
			debugPrintf((_debug_level & OFR_DEBUG) ? OFR_RAW : OFR_NONE, "\n");
		}
	}

	_endWrite();
}

uint16_t OpenFontRender::decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining) {
	uint16_t c = buf[(*index)++];
	//
	// 7 bit Unicode
	if ((c & 0x80) == 0x00)
		return c;

	// 11 bit Unicode
	if (((c & 0xE0) == 0xC0) && (remaining > 1))
		return ((c & 0x1F) << 6) | (buf[(*index)++] & 0x3F);

	// 16 bit Unicode
	if (((c & 0xF0) == 0xE0) && (remaining > 2)) {
		c = ((c & 0x0F) << 12) | ((buf[(*index)++] & 0x3F) << 6);
		return c | ((buf[(*index)++] & 0x3F));
	}

	// 21 bit Unicode not supported so fall-back to extended ASCII
	// if ((c & 0xF8) == 0xF0) return c;

	return c; // fall-back to extended ASCII
}

uint16_t OpenFontRender::color565(uint8_t r, uint8_t g, uint8_t b) {
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

uint16_t OpenFontRender::alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc) {
	// For speed use fixed point maths and rounding to permit a power of 2 division
	uint16_t fgR = ((fgc >> 10) & 0x3E) + 1;
	uint16_t fgG = ((fgc >> 4) & 0x7E) + 1;
	uint16_t fgB = ((fgc << 1) & 0x3E) + 1;

	uint16_t bgR = ((bgc >> 10) & 0x3E) + 1;
	uint16_t bgG = ((bgc >> 4) & 0x7E) + 1;
	uint16_t bgB = ((bgc << 1) & 0x3E) + 1;

	// Shift right 1 to drop rounding bit and shift right 8 to divide by 256
	uint16_t r = (((fgR * alpha) + (bgR * (255 - alpha))) >> 9);
	uint16_t g = (((fgG * alpha) + (bgG * (255 - alpha))) >> 9);
	uint16_t b = (((fgB * alpha) + (bgB * (255 - alpha))) >> 9);

	// Combine RGB565 colours into 16 bits
	// return ((r&0x18) << 11) | ((g&0x30) << 5) | ((b&0x18) << 0); // 2 bit greyscale
	// return ((r&0x1E) << 11) | ((g&0x3C) << 5) | ((b&0x1E) << 0); // 4 bit greyscale
	return (r << 11) | (g << 5) | (b << 0);
}

/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/
//
//  Functions
//  (Use only within this file.)
//
/*_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/*/
/*! \cond PRIVATE */

FT_Error ftc_face_requester(FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face *aface) {
	FT_Error error     = FT_Err_Ok;
	OFR::Face face     = (OFR::Face)face_id;
	FontDataInfo *info = (FontDataInfo *)request_data;

	debugPrintf((info->debug_level & OFR_INFO), "Font load required. FaceId: 0x%p\n", face_id);

	if (info->from == OFR::FROM_FILE) {
		debugPrintf((info->debug_level & OFR_INFO), "Load from file.\n");
		const uint8_t FACE_INDEX = 0;

		error = FT_New_Face(library, face->filepath, FACE_INDEX, aface); // create face object
		if (error) {
			debugPrintf((info->debug_level & OFR_ERROR), "Font load Failed: 0x%02X\n", error);
		} else {
			debugPrintf((info->debug_level & OFR_INFO), "Font load Success!\n");
		}

	} else if (info->from == OFR::FROM_MEMORY) {
		debugPrintf((info->debug_level & OFR_INFO), "Load from memory.\n");
		const uint8_t FACE_INDEX = 0;

		error = FT_New_Memory_Face(library, face->data, face->data_size, FACE_INDEX, aface); // create face object
		if (error) {
			debugPrintf((info->debug_level & OFR_ERROR), "Font load Failed: 0x%02X\n", error);
		} else {
			debugPrintf((info->debug_level & OFR_INFO), "Font load Success!\n");
		}
	}
	return error;
}

void debugPrintf(uint8_t level, const char *fmt, ...) {
	char str[256] = {0};
	va_list ap;

	switch (level) {
	case OFR_NONE:
		return;
	case OFR_ERROR: {
		va_start(ap, fmt);
		vsnprintf(str, 256, fmt, ap);
		va_end(ap);

		g_Print("[OFR ERROR] ");
		g_Print(str);
		return;
	}
	case OFR_INFO: {
		va_start(ap, fmt);
		vsnprintf(str, 256, fmt, ap);
		va_end(ap);

		g_Print("[OFR INFO] ");
		g_Print(str);
		return;
	}
	case OFR_DEBUG: {
		va_start(ap, fmt);
		vsnprintf(str, 256, fmt, ap);
		va_end(ap);

		g_Print("[OFR DEBUG] ");
		g_Print(str);
		return;
	}
	case OFR_RAW: {
		va_start(ap, fmt);
		vsnprintf(str, 256, fmt, ap);
		va_end(ap);

		g_Print(str);
		return;
	}
	default:
		return;
	}
}

#ifdef FREERTOS_CONFIG_H
void RenderTask(void *pvParameters) {
	while (g_UseRenderTask == true) {
		if (g_RenderTaskStatus != RENDERING) {
			vTaskDelay(1);
			continue;
		}
		g_TaskParameter.error = FTC_ImageCache_Lookup(g_TaskParameter.ftc_image_cache,
		                                              &g_TaskParameter.image_type,
		                                              g_TaskParameter.glyph_index,
		                                              &g_TaskParameter.aglyph,
		                                              NULL);
		g_RenderTaskStatus    = (g_TaskParameter.error == FT_Err_Ok) ? END_SUCCESS : END_ERROR;
	}
	g_RenderTaskHandle = NULL;
	vTaskDelete(NULL);
}

/*! \endcond */

#endif
