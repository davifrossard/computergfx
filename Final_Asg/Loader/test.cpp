#include "MeshObject.h"
#include "bmpread.h"
#include <unordered_map>
#include <GL/glut.h>
#include <GL/gl.h>

vector<vec3 > vertices;
vector<vec2 > uvs;
vector<vec3 > normals;
MeshObject chassis;
MeshObject cannon;
MeshObject seats;
MeshObject steering_wheel;
MeshObject wheel;
float angle;

void display() {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(50.0, /* FOV */
                 1.0, /* aspect ratio */
                 1., 20.0); /* Z near and far */

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(-5, -5, 5,  /* eye is at (.5,.5,-2) */
    0.0, 0.0, 0.0,      /* center is at (.5,.5,0) */
    0.0, 0.0, 1.0);      /* up is in positive Y direction */


   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       glEnable(GL_TEXTURE_2D);

  glPushMatrix();
  glRotatef(-angle, 0, 0, 1);
    chassis.draw();

    glPushMatrix();
    glTranslatef(-1.34227, -1.40356, -0.85082);
    glRotatef(10*angle, 1, 0, 0);
      wheel.draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.33411, -1.40356, -0.85082);
    glRotatef(10*angle, 1, 0, 0);
      wheel.draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.33411, 1.42296, -0.85082);
    glRotatef(10*angle, 1, 0, 0);
      wheel.draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.34227, 1.42296, -0.85082);
    glRotatef(10*angle, 1, 0, 0);
      wheel.draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0129, 1.65949, 0.06173);
    glRotatef(5*angle, 0, 0, 1);
      cannon.draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.10706, 0.01334, 0.22268);
      seats.draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.54111, -0.45362, 0.4301);
    glRotatef(angle, 0, 1, 0);
      steering_wheel.draw();
    glPopMatrix();

  glPopMatrix();
  glDisable(GL_TEXTURE_2D);
     glPopAttrib();

     glFlush();
  glutSwapBuffers();
}

void idle(void) {
  angle += 0.1;
  glutPostRedisplay();
}

bool load_texture(const char * path, GLuint textureID)
{
  bmpread_t texture;
  if(!bmpread(path, 0, &texture))
  {
      fprintf(stderr, "error loading bitmap file\n");
      return -1;
  }
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_DECAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_DECAL);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, texture.width, texture.height, GL_RGB, GL_UNSIGNED_BYTE, texture.rgb_data);
  bmpread_free(&texture);
}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutIdleFunc(idle);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(1000,1000);
  glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-1000)/2,
                         (glutGet(GLUT_SCREEN_HEIGHT)-1000)/2);
  glutCreateWindow("Assignment 3");

  GLuint texture[2];
  glGenTextures(2, texture);
  load_texture("../Obj_Parts/tex1.bmp", texture[0]);
  load_texture("../Obj_Parts/tex2.bmp", texture[1]);
  chassis.load_obj("../Obj_Parts/chassis.obj");
  chassis.bind_texture(texture[0]);
  cannon.load_obj("../Obj_Parts/cannon.obj");
  cannon.bind_texture(texture[1]);
  seats.load_obj("../Obj_Parts/seats.obj");
  seats.bind_texture(texture[1]);
  steering_wheel.load_obj("../Obj_Parts/steering_wheel.obj");
  steering_wheel.bind_texture(texture[1]);
  wheel.load_obj("../Obj_Parts/wheel.obj");
  wheel.bind_texture(texture[1]);


  glEnable(GL_DEPTH_TEST);
  glutDisplayFunc(display);
  glutMainLoop();
}
