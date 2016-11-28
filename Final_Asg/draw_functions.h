#ifndef DRAW_FUNCTIONS_H
#define DRAW_FUNCTIONS_H
#define NUM_CIRC_POLYGONS 100
#include <GL/gl.h>
#include <GL/glut.h>
#include <cstdio>
#include <cmath>
#include "Loader/bmpread.h"

using namespace std;

GLuint LoadTexture( const char * filename, int width, int height);
void _draw_cylinder(double height, double radius, GLuint texture, GLfloat normal);
void _draw_circle(GLfloat radius, GLfloat* color);
void _draw_circle_cont(GLfloat radius, GLfloat* color);
void _draw_circle_texture(GLfloat radius, GLfloat* color, GLuint texture, GLfloat normal);
void _draw_rectangle(GLfloat height, GLfloat width, GLfloat* color);
void _draw_iso_triangle(GLfloat height, GLfloat width, GLfloat* color);
void _draw_point(GLfloat* color);
void _draw_text(GLfloat x, GLfloat y, char* str, GLfloat* color, void* font=GLUT_BITMAP_9_BY_15);
bool _load_texture(const char * path, GLuint textureID);

#endif
