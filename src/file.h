#include <stdbool.h>

#include "types.h"

bool file_exists(const char* path);
u32 file_size(const char* path);

/// Read an entire file into a buffer. Caller must free the resource.
/// \param path Filepath
/// \return Pointer to buffer, or NULL on failure.
u8* file_load(const char* path);


/// Read an entire file into an existing buffer.
/// \param path Filepath
/// \param buf Buffer to read file into
/// \param size Size of buffer
bool file_load_existing(const char* path, u8* buf, u32 size);

