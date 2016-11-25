#include "draw_functions.h"

void _draw_circle(GLfloat radius, GLfloat* color)
{
  GLfloat r = radius;
  glColor3fv(color);
  glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0);
    for(int i = 0; i <= NUM_CIRC_POLYGONS; i++)
    {
      double theta =  i * 2 * M_PIl / NUM_CIRC_POLYGONS;
      glVertex3f(r * cosf(theta), r * sinf(theta), 0);
    }
  glEnd();
}

void _draw_circle_cont(GLfloat radius, GLfloat* color)
{
  GLfloat r = radius;
  glColor3fv(color);
  glBegin(GL_LINE_LOOP);
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
  GLfloat h = -height;
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
  glPushAttrib(GL_CURRENT_BIT);
  glBegin(GL_POINTS);
    glColor3fv(color);
    glutSolidSphere(0.5, 20, 20);
  glEnd();
  glPopAttrib();
}

void _draw_text(GLfloat x, GLfloat y, char* str, GLfloat* color, void* font)
{
  glPushAttrib(GL_CURRENT_COLOR);
  glColor3fv(color);
  glRasterPos2f(x, y);
  while( *str ){
    glutBitmapCharacter(font, *str);
    str++;
  }
  glPopAttrib();
}

bool _load_texture(const char * path, GLuint textureID)
{
  bmpread_t texture;
  if(!bmpread(path, 0, &texture))
  {
      printf("error loading bitmap file\n");
      return 0;
  }
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_DECAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_DECAL);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_COMPRESSED_RGB, texture.width, texture.height, GL_RGB, GL_UNSIGNED_BYTE, texture.rgb_data);
  bmpread_free(&texture);
  glBindTexture(GL_TEXTURE_2D, 0);
  return 1;
}
