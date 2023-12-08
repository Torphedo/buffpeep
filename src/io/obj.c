#include <stdio.h>
#include <string.h>

#include "../logging.h"
#include "file.h"

typedef struct {
    u32 vert_pos_count;
    u32 tex_coord_count;
    u32 face_count;
}mesh_info;

mesh_info obj_get_info(char* obj_file) {
    mesh_info output = {0};

    // Break when we hit the null terminator at the end of the file
    while (obj_file[0] != 0x00) {
        // Get pointer to newline or NULL character
        char* line_end = strchrnul(obj_file, '\n');
        // Replace newline with null terminator to make strstr() work correctly
        *line_end = 0x00; 

        // Parse line by line
        if (obj_file[0] != '#') {
            // Faces
            if (obj_file[0] == 'f') {
                output.face_count++;
            }
            // Texture coordinate
            if (strstr(obj_file, "vt ") != NULL) {
                output.tex_coord_count++;
            }
            // Vertex position
            if (strstr(obj_file, "v ") != NULL) {
                output.vert_pos_count++;
            }
        }

        *line_end = '\n'; // Set null terminator back to newline

        // Set pointer to next line
        obj_file = line_end + 1;
    }
    return output;
}

mesh obj_load(const char* path, allocator_t allocator) {
    mesh output = {0};
    u8* obj = file_load(allocator_default, path);
    if (obj == NULL) {
        LOG_MSG(error, "Failed to load OBJ asset %s\n", path);
        return output;
    }

    // We need to know how much memory we need first.
    mesh_info obj_info = obj_get_info((char*)obj);
    output.index_count = obj_info.face_count * 3;

    u32 indices_size = output.index_count * sizeof(*output.indices);
    output.indices = allocator.calloc(1, indices_size);
    if (output.indices == NULL) {
        LOG_MSG(error, "Failed to allocate %d bytes for indices\n", indices_size);
        mesh failure = {0};
        return failure;
    }

    u32 vertex_count = obj_info.vert_pos_count * 3; 
    u32 vert_buf_size = vertex_count * sizeof(*output.vertices);
    output.vertices = allocator.calloc(1, vert_buf_size);
    if (output.vertices == NULL) {
        LOG_MSG(error, "Failed to allocate %d bytes for vertices\n", vert_buf_size);
        mesh failure = {0};
        return failure;
    }

    LOG_MSG(info, "OBJ asset %s has ", path);
    printf("%d vert positions, %d tex coords, %d faces\n", obj_info.vert_pos_count, obj_info.tex_coord_count, obj_info.face_count);

    return output;
}

