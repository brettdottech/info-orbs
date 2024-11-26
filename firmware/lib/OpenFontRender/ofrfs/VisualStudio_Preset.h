// -------------------------------------------------------
//  VisualStudio_Preset.h
//
//  Copyright (c) 2024 takkaO
//
//  If you use, modify, or redistribute this file independently
//  of the original repository, this program is licensed under
//  the MIT License.
//
//  If you use, modify or redistribute this file as part of
//  the original repository, please follow the repository's license.
//
// -------------------------------------------------------

#ifndef OFR_VISUAL_STUDIO_PRESET_H
#define OFR_VISUAL_STUDIO_PRESET_H

#include <list>
#include <stdio.h>
#include <stdlib.h>

std::list<FILE *> ofr_file_list; // For multiple file loading

FT_FILE *OFR_fopen(const char *filename, const char *mode) {
	FILE *f = fopen(filename, mode);
	ofr_file_list.push_back(f);
	return ofr_file_list.back();
}

void OFR_fclose(FT_FILE *stream) {
	if (stream != nullptr) {
		fclose((FILE *)stream);
	}
}

size_t OFR_fread(void *ptr, size_t size, size_t nmemb, FT_FILE *stream) {
	if (stream != nullptr) {
		return fread((uint8_t *)ptr, size, nmemb, (FILE *)stream);
	}
	return 0;
}

int OFR_fseek(FT_FILE *stream, long int offset, int whence) {
	if (stream != nullptr) {
		return fseek((FILE *)stream, offset, whence);
	}
	return -1;
}

long int OFR_ftell(FT_FILE *stream) {
	if (stream != nullptr) {
		return ftell((FILE *)stream);
	}
	return -1L;
}

#endif /* OFR_VISUAL_STUDIO_PRESET_H */