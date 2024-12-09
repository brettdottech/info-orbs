// -------------------------------------------------------
//  M5Stack_SD_Preset.h
//
//  Copyright (c) 2021 takkaO
//
//  If you use, modify, or redistribute this file independently
//  of the original repository, this program is licensed under
//  the MIT License.
//
//  If you use, modify or redistribute this file as part of
//  the original repository, please follow the repository's license.
//
// -------------------------------------------------------

#ifndef OFR_M5STACK_SD_PRESET_H
#define OFR_M5STACK_SD_PRESET_H

#include <SD.h>
#include <list>

std::list<File> ofr_file_list; // For multiple file loading

FT_FILE *OFR_fopen(const char *filename, const char *mode) {
	File f = SD.open(filename, mode);
	ofr_file_list.push_back(f);
	return &ofr_file_list.back();
}

void OFR_fclose(FT_FILE *stream) {
	((File *)stream)->close();
}

size_t OFR_fread(void *ptr, size_t size, size_t nmemb, FT_FILE *stream) {
	return ((File *)stream)->read((uint8_t *)ptr, size * nmemb);
}

int OFR_fseek(FT_FILE *stream, long int offset, int whence) {
	return ((File *)stream)->seek(offset, (SeekMode)whence);
}

long int OFR_ftell(FT_FILE *stream) {
	return ((File *)stream)->position();
}

#endif /* OFR_M5STACK_SD_PRESET_H */