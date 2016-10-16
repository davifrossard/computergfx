#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include <GL/gl.h>

struct point {
  GLfloat x, y;
};

struct rectangle {
  point origin;
  GLfloat w, h;
  GLfloat* color;
};

struct triangle {
  point p[3];
  GLfloat* color;
};

struct circle {
  point center;
  GLfloat r;
  GLfloat* color;
};

#endif
