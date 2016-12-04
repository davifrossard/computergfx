#include "draw_functions.h"

void _draw_cylinder(double height, double radius, GLuint texture, GLfloat normal)
{
  glBindTexture(GL_TEXTURE_2D, texture);
  double resolution = 0.01;

  glPushMatrix();
  glRotatef(90,1,0,0);
  glTranslatef(0, -0.5, 0);
    glBegin(GL_QUAD_STRIP);
      for (double i = 0; i <= 2 * M_PI; i += resolution) {
        const float tc = ( i / (float)( 2 * M_PI ) )*50;
        glTexCoord2f( tc, 0.0 );
        glVertex3f(radius * cos(i), 0, radius * sin(i));
        glNormal3f(normal * cos(i), 0, normal * sin(i));
        glTexCoord2f( tc, 2.5 );
        glVertex3f(radius * cos(i), height, radius * sin(i));
        glNormal3f(normal * cos(i), 0, normal * sin(i));
      }
      glTexCoord2f( 50, 0.0);
      glVertex3f(radius, 0, 0);
      glNormal3f(normal, 0, 0);

      glTexCoord2f( 50, 2.5);
      glNormal3f(normal, 0, 0);
      glVertex3f(radius, height, 0);
    glEnd();
  glPopMatrix();
}

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

void _draw_circle_texture(GLfloat radius, GLfloat* color, GLuint texture, GLenum normal)
{
  float angle, radian, x, y, xcos, ysin, tx, ty;
  GLUquadric* circ = gluNewQuadric();
  glBindTexture(GL_TEXTURE_2D, texture);
  gluQuadricTexture(circ, true);
  gluQuadricOrientation(circ, GLU_INSIDE);
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glPushMatrix();
  glScalef(15, 15, 1);
  gluDisk(circ,  0, radius, 100, 100);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
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
  glColor3fv(color);
  glutSolidSphere(0.1, 20, 20);
  glPopAttrib();
}

void _draw_sphere(GLuint texture)
{
  GLUquadricObj *obj = gluNewQuadric();
  gluQuadricTexture(obj, true);
  gluQuadricOrientation(obj, GLU_OUTSIDE);
  glBindTexture(GL_TEXTURE_2D, texture);
  gluSphere(obj, 0.1, 20, 20);
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
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_COMPRESSED_RGB, texture.width, texture.height, GL_RGB, GL_UNSIGNED_BYTE, texture.rgb_data);
  bmpread_free(&texture);
  glBindTexture(GL_TEXTURE_2D, 0);
  return 1;
}
