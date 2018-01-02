#ifndef SMATH_H
#define SMATH_H

typedef union sfvec2 {
    struct {
        float x;
        float y;
    };
    float e[2];
} sfvec2;

typedef union sfvec3 {
    struct {
        float x;
        float y;
        float z;
    };
    struct {
        float r;
        float g;
        float b;
    };

    sfvec2 xy;

    float e[3];
} sfvec3;

typedef union sfvec4 {
    struct {
        float x;
        float y;
        float z;
        float w;
    };
    struct {
        float r;
        float g;
        float b;
        float a;
    };
    struct {
        sfvec2 xy;
        sfvec2 zw;
    };

    sfvec3 xyz;
    sfvec3 rgb;

    float e[4];
} sfvec4;

#endif //SMATH_H
