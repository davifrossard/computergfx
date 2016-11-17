#include "OBJLoader.h"
#include "bmpread.h"
#include <GL/glut.h>
#include <GL/gl.h>

vector<vec3 > vertices;
vector<vec2 > uvs;
vector<vec3 > normals;
GLuint textureID;
float angle;

void display() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(50.0, /* FOV */
                 1.0, /* aspect ratio */
                 1., 50.0); /* Z near and far */

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(10, 10, 5,  /* eye is at (.5,.5,-2) */
    0.0, 0.0, 0.0,      /* center is at (.5,.5,0) */
    0.0, 0.0, 1.0);      /* up is in positive Y direction */


   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       glEnable(GL_TEXTURE_2D);

  glPushMatrix();
  glRotatef(angle, 0, 0, 1);
    glBegin(GL_TRIANGLES);
    for(unsigned int i = 0; i < vertices.size(); i++)
    {
      vec3 v = vertices[i];
      vec3 n = normals[i];
      vec2 uv = uvs[i];
      glTexCoord2d(uv.x, uv.y);
      glVertex3f(v.x, v.y, v.z);
      glNormal3f(n.x, n.y, n.z);
    }
    glEnd();
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
int main(int argc, char** argv)
{
  bool res = loadOBJ("../Objs/jeep.obj", vertices, uvs, normals);

  bmpread_t texture;
  if(!bmpread("../Objs/tex1.bmp", 0, &texture))
  {
      fprintf(stderr, "error loading bitmap file\n");
      exit(1);
  }




  glutInit(&argc, argv);
  glutIdleFunc(idle);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(1000,1000);
  glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-1000)/2,
                         (glutGet(GLUT_SCREEN_HEIGHT)-1000)/2);
  glutCreateWindow("Assignment 3");

  glClearColor(0.0, 0.0, 0.0, 0.0);

  glGenTextures(0, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_DECAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_DECAL);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  gluBuild2DMipmaps(GL_TEXTURE_2D, 4, texture.width, texture.height, GL_RGB, GL_UNSIGNED_BYTE, texture.rgb_data);
  glEnable(GL_DEPTH_TEST);
  glutDisplayFunc(display);
  glutMainLoop();
}
