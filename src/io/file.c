#include <sys/stat.h>

#include "../logging.h"
#include "file.h"

bool file_exists(const char* path) {
    struct stat st = {0};
    return (stat(path, &st) == 0);
}

u32 file_size(const char* path) {
    struct stat st = {0};
    if (stat(path, &st) != 0) {
        return 0;
    }
    return st.st_size;
}

u8* file_load(allocator_t allocator, const char* path) {
    if (!file_exists(path)) {
        LOG_MSG(error, "Requested file %s does not exist.\n", path);
        return NULL;
    }
    u32 size = file_size(path);
    u8* buffer = allocator.calloc(1, size);
    FILE* resource = fopen(path, "rb");

    if (resource != NULL && buffer != NULL) {
        fread(buffer, size, 1, resource);
        fclose(resource);
        return buffer;
    }
    return NULL;
}

