// -------------------------------------------------------
//  FileSupport.cpp
//
//  Copyright (c) 2021 takkaO
//
//  If you use, modify or redistribute this file as part of
//  the original repository, please follow the repository's license.
//
// -------------------------------------------------------

#include "FileSupport.h"

#if defined(_MSC_VER) // Support for VisualStudio

/*!
 * @brief Close file.
 * @param[in] (*stream) File pointer.
 * @note The weak symbol is defined.
 * @note Need to override this function delay on the device used.
 */
void _default_OFR_fclose(FT_FILE *stream) {
}

/*!
 * @brief Open file and get file pointer.
 * @param[in] (*filename) Target file path.
 * @param[in] (*mode) Open mode.
 * @return File pointer or false (NULL).
 * @note The weak symbol is defined.
 * @note Need to override this function delay on the device used.
 */
FT_FILE *_default_OFR_fopen(const char *filename, const char *mode) {
	return NULL;
}

/*!
 * @brief Read data from a file and store it in a buffer.
 * @param[in] (*ptr) Buffer pointer.
 * @param[in] (size) Data size.
 * @param[in] (nmemb) Number of read data.
 * @param[in] (*stream) File pointer.
 * @return Number of successfully read data.
 * @note The weak symbol is defined.
 * @note Need to override this function delay on the device used.
 */
size_t _default_OFR_fread(void *ptr, size_t size, size_t nmemb, FT_FILE *stream) {
	return 0;
}

/*!
 * @brief Moves the cursor to the specified position.
 * @param[in] (*stream) File pointer.
 * @param[in] (offset) Move offset.
 * @param[in] (whence) Seek position.
 * @return If successful, return 0, otherwise return non-zero.
 * @note The weak symbol is defined.
 * @note Need to override this function delay on the device used.
 */
int _default_OFR_fseek(FT_FILE *stream, long int offset, int whence) {
	return -1;
}

/*!
 * @brief Get file cursor position.
 * @param[in] (*stream) File pointer.
 * @return If successful, return position, otherwise return -1L.
 * @note The weak symbol is defined.
 * @note Need to override this function delay on the device used.
 */
long int _default_OFR_ftell(FT_FILE *stream) {
	return -1L;
}

#else

/*!
 * @brief Close file.
 * @param[in] (*stream) File pointer.
 * @note The weak symbol is defined.
 * @note Need to override this function delay on the device used.
 */
__attribute__((weak)) void OFR_fclose(FT_FILE *stream) {
}

/*!
 * @brief Open file and get file pointer.
 * @param[in] (*filename) Target file path.
 * @param[in] (*mode) Open mode.
 * @return File pointer or false (NULL).
 * @note The weak symbol is defined.
 * @note Need to override this function delay on the device used.
 */
__attribute__((weak)) FT_FILE *OFR_fopen(const char *filename, const char *mode) {
	return NULL;
}

/*!
 * @brief Read data from a file and store it in a buffer.
 * @param[in] (*ptr) Buffer pointer.
 * @param[in] (size) Data size.
 * @param[in] (nmemb) Number of read data.
 * @param[in] (*stream) File pointer.
 * @return Number of successfully read data.
 * @note The weak symbol is defined.
 * @note Need to override this function delay on the device used.
 */
__attribute__((weak)) size_t OFR_fread(void *ptr, size_t size, size_t nmemb, FT_FILE *stream) {
	return 0;
}

/*!
 * @brief Moves the cursor to the specified position.
 * @param[in] (*stream) File pointer.
 * @param[in] (offset) Move offset.
 * @param[in] (whence) Seek position.
 * @return If successful, return 0, otherwise return non-zero.
 * @note The weak symbol is defined.
 * @note Need to override this function delay on the device used.
 */
__attribute__((weak)) int OFR_fseek(FT_FILE *stream, long int offset, int whence) {
	return -1;
}

/*!
 * @brief Get file cursor position.
 * @param[in] (*stream) File pointer.
 * @return If successful, return position, otherwise return -1L.
 * @note The weak symbol is defined.
 * @note Need to override this function delay on the device used.
 */
__attribute__((weak)) long int OFR_ftell(FT_FILE *stream) {
	return -1L;
}

#endif
