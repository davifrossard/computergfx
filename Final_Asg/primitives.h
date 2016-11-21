#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include <GL/gl.h>

struct circ_state {
  GLfloat x, y, r;
};

struct vec3 {
  GLfloat x, y, z;
};

struct sp_state {
  vec3 position;
  GLfloat angle;
};

struct vec2 {
  GLfloat x, y;
};

struct rectangle {
  vec3 origin;
  GLfloat w, h;
  GLfloat* color;
};

struct triangle {
  vec3 p[3];
  GLfloat* color;
};

struct circle {
  vec3 center;
  GLfloat r;
  GLfloat* color;
};

struct projectile {
  vec3 origin;
  GLfloat angle;
  GLfloat zangle;
};

#endif
