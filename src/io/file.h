#include <stdbool.h>

#include "../types.h"
#include "../allocator.h"

bool file_exists(const char* path);
u32 file_size(const char* path);

/// Read an entire file into a buffer. Caller must free the resource.
/// \param allocator The allocator to use (if you don't have one, use allocator_default)
/// \param path Filepath
/// \return Pointer to buffer, or NULL on failure.
u8* file_load(allocator_t allocator, const char* path);

