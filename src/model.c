#include <stdio.h>
#include <string.h>

#include <glad/glad.h>

#include <common/logging.h>
#include <common/file.h>
#include "model.h"

void model_upload(model* m) {
    gl_obj buffers[2];
    glGenBuffers(2, buffers);
    glGenVertexArrays(1, &m->vao);
    m->vbuf = buffers[0];
    m->ibuf = buffers[1];

    // Setup vertex buffer
    glBindVertexArray(m->vao);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * m->vert_count, m->vertices, GL_STATIC_DRAW);

    // Setup index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ibuf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * m->idx_count, m->indices, GL_STATIC_DRAW);

    // Create vertex layout
    glVertexAttribPointer(0, sizeof(vec3) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, sizeof(vec4) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, color));
    glEnableVertexAttribArray(1);

    // Unbind our buffers to avoid messing our state up
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

u32 model_size(const model m) {
    return sizeof(m) - (2 * sizeof(void*)) + (m.vert_count * sizeof(vertex)) + (m.idx_count * sizeof(u16));
}

model obj_load(u8* txt) {
    vertex* vertices = NULL;
    u16* indices = NULL;
    u16 vert_count = 0;
    u16 idx_count = 0;

    // We parse the file in 2 passes. The first pass tallies the vertex/face
    // counts to allocate the vertex/index buffers, and the second loads the
    // actual data.
    for (u8 i = 1; i < 3; i++) {
        const bool tally_pass = (i == 1);
        const bool read_pass = (i == 2);

        // Used to track current position in buffer.
        vertex* vpos = vertices;
        u16* ipos = indices;

        // Loop over the already-loaded OBJ data.
        // Final null terminator indicates end of data
        char* line = txt;
        while (*line != 0x00) {
            // Get pointer to end of line (NUL or newline)
            char* line_end = strchr(line, '\n');
            if (line_end == NULL) {
                break;
            }
            // Replace newline with NUL, so we only search the current line in
            // strstr(). Save original character so we can undo the change.
            const char end = *line_end;
            *line_end = 0x00; 

            if (line[0] != '#') {
                // Faces
                if (line[0] == 'f') {
                    if (tally_pass) {
                        // Each face requires 3 indices
                        idx_count += 3;
                    }
                    else {
                        // Read the index data
                        sscanf(line, "f %hd %hd %hd", &ipos[0], &ipos[1], &ipos[2]);
                        // OBJ indices are 1-based...
                        ipos[0]--;
                        ipos[1]--;
                        ipos[2]--;
                        ipos += 3; // Advance by 3 indices
                    }
                }
                // Vertex position
                if (strstr(line, "v ") != NULL) {
                    if (tally_pass) {
                        vert_count++;
                    }
                    else {
                        // Read the vertex data
                        sscanf(line, "v %f %f %f %f %f %f", &vpos->position[0], &vpos->position[1],&vpos->position[2], &vpos->color[0], &vpos->color[1], &vpos->color[2]);
                        vpos->color[3] = 1.0f; // Set alpha channel
                        vpos++;
                    }
                }
            }

            // Undo our change & advance to next line
            *line_end = end;
            line = line_end + 1;
        }

        // Before the second pass, we need to alloc the vertex & index buffers.
        if (tally_pass) {
            vertices = calloc(vert_count, sizeof(vertex));
            indices = calloc(idx_count, sizeof(u16));
            if (vertices == NULL || indices == NULL) {
                LOG_MSG(error, "Buffer alloc failure!\n");
                printf("\tVertex buffer %p (0x%hx bytes)\n", vertices, vert_count * (u16)sizeof(vertex));
                printf("\tIndex buffer %p (0x%hx bytes)\n", indices, idx_count * (u16)sizeof(u16));
                break;
            }
        }
    }

    return (model) {
        vertices, indices, vert_count, idx_count,
    };
}

