#include <GL/glut.h>
#include <GL/gl.h>
#include <vector>
#include "car.h"
#include "arena.h"
using namespace std;

Car* player;
Arena* arena;
int key_status[256];
GLfloat x, y, theta = 0;
float cannon_angle;

vector<projectile> shots;

float sign(float x)
{
  return (x >= 0) ? 1 : -1;
}

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

void draw_point()
{
  glPointSize(10);
  glBegin(GL_POINTS);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
  glEnd();
}

void display() {
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(0, 1, 1, 0, -1.0, 1.0);
    float car_to_arena = player->get_max_attr() / (arena->get_max_attr() / arena->get_player_diameter());
    glPushMatrix();
    glTranslatef(0.5, 0.5, 0);
      arena->draw_arena();

      glPushMatrix();
        glScalef(car_to_arena, car_to_arena, 0);
        for (vector<projectile>::iterator p = shots.begin(); p != shots.end(); p++)
        {
          glPushMatrix();
          glTranslatef(p->origin.x, p->origin.y, 0);
          glRotatef(p->angle*180/M_PIl, 0, 0, 1);
            draw_point();
          glPopMatrix();
          p->origin.x += 0.1 * sin(p->angle);
          p->origin.y -= 0.1 * cos(p->angle);
        }
        glTranslatef(x, y, 0);
        glRotatef(theta*180/M_PIl, 0, 0, 1);
        player->draw_car();
      glPopMatrix();
    glPopMatrix();
    glutSwapBuffers();
}

void control_cannon(int x, int y)
{
  cannon_angle = player->turn_cannon(x * (45 - (-45)) / (1000) + (-45));
}

void shoot_cannon(int button, int state, int mx, int my)
{
    if(button == 0 && state == 0)
    {
      projectile shot;
      float disp =
      shot.origin.x = x-player->get_cannon_len()*sin(theta);
      shot.origin.y = y+player->get_cannon_len()*cos(theta);
      shot.angle = theta + cannon_angle*M_PIl/180;
      shots.push_back(shot);
    }
}

void reshape(int w, int h)
{
    if (w > h)
        glViewport((w-h)/2., 0, (GLsizei) h, (GLsizei) h);
    else
        glViewport(0, (h-w)/2., (GLsizei) w, (GLsizei) w);
}

void idle(void) {
  tuple<float, float> result;
  float inc = 0;
  static float wheel_angle = 0;
  if(key_status['a'] == 1 || key_status['A'] == 1) {
    wheel_angle = player->turn_wheel(-1);
  } if(key_status['d'] == 1 || key_status['D'] == 1) {
    wheel_angle = player->turn_wheel(1);
	} if(key_status['w'] == 1 || key_status['A'] == 1) {
    inc = -0.1;
    player->forward();
	} if(key_status['s'] == 1 || key_status['S'] == 1) {
    inc = 0.1;
    player->back();
	}
  float thetap = theta + inc / player->get_axle_track() * tan(-wheel_angle * M_PIl/180);
  float xp = x + inc * sin(-thetap);
  float yp = y + inc * cos(-thetap);
  float car_to_arena = player->get_max_attr() / (arena->get_max_attr() / arena->get_player_diameter());
  point p = {xp * car_to_arena, yp * car_to_arena};
  if(arena->check_player_colision(p))
  {
    theta = thetap;
    x = xp;
    y = yp;
    arena->set_player_position(p);
  }
	glutPostRedisplay();
}


int main(int argc, char** argv) {
  string car_file = "car.svg";
  player = new Car(car_file);
  if(argc < 2) {
      printf("Please provide configuration file\n");
      return 0;
  }
  string buffer = argv[1];
  arena = new Arena(buffer);
  point player_position = arena->get_player_position();
  float arena_to_car = (arena->get_max_attr() / arena->get_player_diameter()) / player->get_max_attr();
  x = player_position.x * arena_to_car;
  y = player_position.y * arena_to_car;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(1000,1000);
  glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-1000)/2,
                         (glutGet(GLUT_SCREEN_HEIGHT)-1000)/2);
  glutCreateWindow("Car Model");
  glutDisplayFunc(display);
  glutPassiveMotionFunc(control_cannon);
  glutKeyboardUpFunc(keyup);
  glutKeyboardFunc(keydown);
  glutMouseFunc(shoot_cannon);
  glutIdleFunc(idle);
  glutReshapeFunc(reshape);
  glutMainLoop();
  return 0;
}
