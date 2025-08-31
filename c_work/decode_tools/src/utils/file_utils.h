// src/utils/file_utils.h
#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>

int create_directory_if_not_exists(const char *path);
int get_dir_from_filepath(const char *file, char *dir, size_t size);

#endif