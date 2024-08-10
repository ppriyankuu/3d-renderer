#include "headers/matix.h"

mat4_t mat4_identity(void) {
    mat4_t m = {{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    }};
    return m;
}

mat4_t mat4_make_scale(float sx, float sy, float sz) {
    mat4_t m = mat4_identity();
    m[0][0] = sx;
    m[1][1] = sy;
    m[2][2] = sz;

    return m;
}