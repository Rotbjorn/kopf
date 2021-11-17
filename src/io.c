#include "io.h"
#include "pch.h"
#include <rotbjorn/skrivarn.h>

const char *read_file(const char* path) {
    skrivarn_infof("Attempting to read: %s", path);
    FILE* file = fopen(path, "rb");

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char* buf = malloc(file_size + 1);
    fread(buf, sizeof(char), file_size, file);

    buf[file_size] = 0;

    fclose(file);
    return buf;
}
