#include "../logging.h"
#include "../types.h"

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

void gltf_render(const char* path) {
    cgltf_options options = {0};
    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse_file(&options, "data/cube.gltf", &data);
    if (result != cgltf_result_success) {
        LOG_MSG(error, "CGLTF failed to parse %s\n", path);
        return;
    }
    cgltf_load_buffers(&options, data, "data/cube.gltf");

    cgltf_free(data);
}

