#include <GL/glut.h>
#include <GL/gl.h>
#include "car.h"

using namespace std;

Car* player;
float cannon_ang;
int key_status[256];

void keyup(unsigned char key, int x, int y)
{
    key_status[key] = 0;
    int i;
}

void keydown(unsigned char key, int x, int y)
{
    key_status[key] = 1;
    int i;
}

void display() {
    static int i;
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 1, 0, -1.0, 1.0);
    glPushMatrix();
    glTranslatef(0.5, 0.5, 0);
    glRotatef(i++, 0,0,1);
    player->draw_car();
    glPopMatrix();
    glutSwapBuffers();
}

void control_cannon(int x, int y)
{
  player->turn_cannon(x * (45 - (-45)) / (500) + (-45));
}

void idle(void) {
  float inc = 1.0;
	float degree_inc = 1;
  if(key_status['a'] == 1 || key_status['A'] == 1) {
    player->turn_wheel(-5);
  } if(key_status['d'] == 1 || key_status['D'] == 1) {
    player->turn_wheel(5);
	}
  player->forward();
	glutPostRedisplay();
}

int main(int argc, char** argv) {
  string car_file = "car.svg";
  player = new Car(car_file);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(500,500);
  glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-500)/2,
                         (glutGet(GLUT_SCREEN_HEIGHT)-500)/2);
  glutCreateWindow("Car Model");
  glutDisplayFunc(display);
  glutPassiveMotionFunc(control_cannon);
  glutKeyboardUpFunc(keyup);
  glutKeyboardFunc(keydown);
  glutIdleFunc(idle);
  glutMainLoop();
  return 0;
}
