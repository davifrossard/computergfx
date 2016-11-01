#include <GL/glut.h>
#include <GL/gl.h>
#include <vector>
#include <tuple>
#include <ctime>
#include "car.h"
#include "arena.h"
using namespace std;

Car* player;
vector<Car*> enemies;
Arena* arena;
int key_status[256];
GLfloat x, y, theta = 0;
float cannon_angle, velTiro, velCarro;
float car_to_arena, arena_to_car;

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
  glPointSize(5);
  glBegin(GL_POINTS);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
  glEnd();
}

void display() {
  static float last_time = glutGet(GLUT_ELAPSED_TIME);
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glOrtho(0, 1, 1, 0, -1.0, 1.0);
  glPushMatrix();
  glTranslatef(0.5, 0.5, 0);
    arena->draw_arena();
  glPopMatrix();

  glPushMatrix();
  glScalef(car_to_arena, car_to_arena, 0);
    for (vector<projectile>::iterator p = shots.begin(); p != shots.end(); p++)
    {
      glPushMatrix();
      glTranslatef(p->origin.x, p->origin.y, 0);
      glRotatef(p->angle*180/M_PIl, 0, 0, 1);
        draw_point();
      glPopMatrix();
    }
    glTranslatef(x, y, 0);
    glRotatef(theta*180/M_PIl, 0, 0, 1);
    player->draw_car();
  glPopMatrix();
  glutSwapBuffers();
  last_time = glutGet(GLUT_ELAPSED_TIME);
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
  static float last_time = glutGet(GLUT_ELAPSED_TIME);
  tuple<float, float> result;
  float inc = 0;
  static float wheel_angle = 0;
  float elapsed_time = glutGet(GLUT_ELAPSED_TIME) - last_time;
  if(key_status['a'] == 1 || key_status['A'] == 1) {
    wheel_angle = player->turn_wheel(-1);
  } if(key_status['d'] == 1 || key_status['D'] == 1) {
    wheel_angle = player->turn_wheel(1);
	} if(key_status['w'] == 1 || key_status['A'] == 1) {
    inc -= velCarro * elapsed_time;
    player->forward();
	} if(key_status['s'] == 1 || key_status['S'] == 1) {
    inc += velCarro * elapsed_time;
    player->back();
	}
  float thetap = theta + inc / player->get_axle_track() * tan(-wheel_angle * M_PIl/180);
  float xp = x + inc * sin(-thetap);
  float yp = y + inc * cos(-thetap);
  point p = {xp * car_to_arena, yp * car_to_arena};
  if(arena->check_player_colision(p))
  {
    theta = thetap;
    x = xp;
    y = yp;
    arena->set_player_position(p);
  }
  for (vector<projectile>::iterator p = shots.begin(); p != shots.end();)
  {
    p->origin.x += velTiro * elapsed_time * sin(p->angle);
    p->origin.y -= velTiro * elapsed_time * cos(p->angle);
    float px = p->origin.x * car_to_arena;
    float py = p->origin.y * car_to_arena;
    if(px > 1 || px < 0 || py > 1 || py < 0)
      p = shots.erase(p);
    else
      p++;
  }
  last_time = glutGet(GLUT_ELAPSED_TIME);
	glutPostRedisplay();
}


tuple<string,float,float> parseXML(string path)
{
  const char * const configs[7] = {"arquivoDaArena", "nome", "tipo", "caminho", "carro", "velTiro", "velCarro"};
  TiXmlDocument config;
  path += "config.xml";
  config.LoadFile(path.c_str());
  TiXmlElement* root = config.FirstChildElement();
  if(!root)
  {
    printf("Invalid configuration file.\n");
    exit(0);
  }
  // Read configuration file
  string img;
  float velTiro, velCarro;
  for(TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement()) {
    string elemName = elem->Value();
    if(elemName == configs[0]) {
      const char* file = elem->Attribute(configs[1]);
      const char* ext = elem->Attribute(configs[2]);
      const char* path = elem->Attribute(configs[3]);
      stringstream ss;
      ss << path << "/" << file << "." << ext;
      img = ss.str();
    } else if(elemName == configs[4]) {
      velTiro = atof(elem->Attribute(configs[5]));
      velCarro = atof(elem->Attribute(configs[6]));
    }
  }
  return make_tuple(img,velTiro,velCarro);
}


int main(int argc, char** argv) {
  // Read configuration file
  if(argc < 2) {
      printf("Please provide configuration file\n");
      return 0;
  }
  string buffer = argv[1];
  tuple<string,float,float> configs = parseXML(buffer); // Read config.xml

  // Read Arena
  string afile = get<0>(configs); // Get arena file
  arena = new Arena(afile); // Read arena file

  // Read car
  string car_file = "car.svg";
  player = new Car(car_file, -1, NULL);

  // Transformation constants
  car_to_arena = arena->get_player_diameter(); // Transfer from car to arena
  arena_to_car = 1/car_to_arena; // Transfer from arena to car

  // Create enemy vehicles
  int count = 0;
  for(circle e : arena->get_enemies())
  {
    Car* enemy = new Car(car_file, count++, e.color);
  }

  // Set car configurations
  velTiro = get<1>(configs) * car_to_arena; // Shot speed
  velCarro = get<2>(configs) * car_to_arena; // Car speed
  point player_position = arena->get_player_position(); // Get Initial position
  x = player_position.x * arena_to_car; // Set player initial position
  y = player_position.y * arena_to_car;

  // Glut stuff
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(1000,1000);
  glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-1000)/2,
                         (glutGet(GLUT_SCREEN_HEIGHT)-1000)/2);
  glutCreateWindow("Assignment 3");
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
