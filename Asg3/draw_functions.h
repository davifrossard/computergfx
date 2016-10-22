#ifndef DRAW_FUNCTIONS_H
#define DRAW_FUNCTIONS_H
#define NUM_CIRC_POLYGONS 100
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>

void _draw_circle(GLfloat radius, GLfloat* color);
void _draw_rectangle(GLfloat height, GLfloat width, GLfloat* color);
void _draw_iso_triangle(GLfloat height, GLfloat width, GLfloat* color);

#endif
