#include "draw_functions.h"

void _draw_circle(GLfloat radius, GLfloat* color)
{
  GLfloat r = radius;
  glColor3fv(color);
  glBegin( GL_TRIANGLE_FAN );
    glVertex3f(0, 0, 0);
    for(int i = 0; i <= NUM_CIRC_POLYGONS; i++)
    {
      double theta =  i * 2 * M_PIl / NUM_CIRC_POLYGONS;
      glVertex3f(r * cosf(theta), r * sinf(theta), 0);
    }
  glEnd();
}

void _draw_rectangle(GLfloat height, GLfloat width, GLfloat* color)
{
  GLfloat h = height;
  GLfloat w = width;
  glBegin(GL_POLYGON);
    glColor3fv(color);
    glVertex3f(0, 0, 0);
    glVertex3f(0, h, 0);
    glVertex3f(w, h, 0);
    glVertex3f(w, 0, 0);
  glEnd();
}

void _draw_iso_triangle(GLfloat height, GLfloat width, GLfloat* color)
{
  GLfloat h = height;
  GLfloat w = width;
  glBegin(GL_POLYGON);
    glColor3fv(color);
    glVertex3f(0, 0, 0);
    glVertex3f(w, 0, 0);
    glVertex3f(w/2., h, 0);
  glEnd();
}

void _draw_point(GLfloat* color)
{
  glPointSize(5);
  glBegin(GL_POINTS);
    glColor3fv(color);
    glVertex3f(0, 0, 0);
  glEnd();
}

void _draw_text(GLfloat x, GLfloat y, char* str, GLfloat* color, void* font)
{
  glColor3fv(color);
  glRasterPos2f(x, y);
  while( *str ){
    glutBitmapCharacter(font, *str);
    str++;
  }
}
