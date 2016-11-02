#include <GL/glut.h>
#include <GL/gl.h>
#include <vector>
#include <tuple>
#include <ctime>
#include "car.h"
#include "arena.h"
using namespace std;

Car* player;
unordered_map<int,Car*> enemies;
unordered_map<string, GLfloat*> colors;
Arena* arena;
bool alive = 1, win = 0;
char current_time[10];
int key_status[256];
float velTiro, velCarro;
float eVelTiro, eFreqTiro, eVelCarro;
float car_to_arena;

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

void display() {
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
    for(auto ek : enemies) {
      Car* e = ek.second;
      glPushMatrix();
        e->draw_car();
      glPopMatrix();
    }
    glPushMatrix();
      player->draw_car();
    glPopMatrix();
  glPopMatrix();
  _draw_text(.9, 0.015, current_time, colors["black"]);

  if(!alive) _draw_text(.9, 0.03, "MORREU", colors["red"], GLUT_BITMAP_HELVETICA_18);
  if(win) _draw_text(.9, 0.03, "GANHOU", colors["green"], GLUT_BITMAP_HELVETICA_18);
  glutSwapBuffers();
}

void control_cannon(int x, int y)
{
  player->turn_cannon(x * (45 - (-45)) / (1000) + (-45));
}

void shoot_cannon(int button, int state, int mx, int my)
{
    if(button == 0 && state == 0)
    {
      player->shoot();
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
  static float cont = 0;
  static float last_time = glutGet(GLUT_ELAPSED_TIME);
  float elapsed_time = glutGet(GLUT_ELAPSED_TIME) - last_time;
  bool res_count = 0;
  cont += elapsed_time;

  if(alive && !win) {
    tuple<float, float> result;
    if(key_status['a'] == 1 || key_status['A'] == 1) {
      player->turn_wheel(-1);
    } if(key_status['d'] == 1 || key_status['D'] == 1) {
      player->turn_wheel(1);
  	} if(key_status['w'] == 1 || key_status['W'] == 1) {
      win = player->forward(-velCarro * elapsed_time);
  	} if(key_status['s'] == 1 || key_status['S'] == 1) {
      win = player->back(velCarro * elapsed_time);
  	}
    if(win)
      glutPostRedisplay();
    player->update_shots(velTiro * elapsed_time, &enemies, player);
    for(auto ck : enemies) {
      Car* c = ck.second;
      c->auto_forward(-eVelCarro * elapsed_time);
      c->auto_turn_cannon();
      if(cont >= (1./eFreqTiro)) {
        c->shoot();
        res_count = 1;
      } if(c->update_shots(eVelTiro * elapsed_time, &enemies, player)) {
        alive = 0;
        glutPostRedisplay();
      }
    }
    if(res_count) cont = 0;
    int seconds = last_time / 1000;
    int minutes = seconds / 60;
    sprintf(current_time, "%02d:%02d:%02d", minutes, seconds%60, (int)last_time%1000);
    glutPostRedisplay();
    last_time = glutGet(GLUT_ELAPSED_TIME);
  }
}


string parseXML(string path)
{
  const char * const configs[9] = {"arquivoDaArena", "nome", "tipo", "caminho", "carro", "velTiro", "velCarro", "carroInimigo", "freqTiro"};
  TiXmlDocument config;
  path += "config.xml";
  config.LoadFile(path.c_str());
  TiXmlElement* root = config.FirstChildElement();
  if(!root) {
    printf("Invalid configuration file.\n");
    exit(0);
  }
  // Read configuration file
  string img;
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
    } else if(elemName == configs[7]) {
      eFreqTiro = atof(elem->Attribute(configs[8]));
      eVelTiro = atof(elem->Attribute(configs[5]));
      eVelCarro = atof(elem->Attribute(configs[6]));
    }
  }
  return img;
}


int main(int argc, char** argv) {
  srand(time(NULL)); // Seed generator
  // Read configuration file
  if(argc < 2) {
      printf("Please provide configuration file\n");
      return 0;
  }
  string buffer = argv[1];
  string arena_file = parseXML(buffer); // Read config.xml

  // Read Arena
  arena = new Arena(arena_file); // Read arena file

  // Create player vehicle
  string car_file = "car.svg";
  player = new Car(car_file, -1, NULL, arena);

  // Create enemy vehicles
  for(auto ek : arena->get_enemies()) {
    Car* enemy = new Car(car_file, ek.first, ek.second.color, arena);
    enemies[ek.first] = enemy;
  }

  // Transformation constants
  car_to_arena = arena->get_player_radius(); // Transfer from car to arena
  velTiro = velTiro * car_to_arena / 4; // Shot speed
  eVelTiro = eVelTiro * car_to_arena / 4; // Enemy shot speed
  velCarro = velCarro * car_to_arena / 4; // Car speed
  eVelCarro = eVelCarro * car_to_arena / 4; // Car speed

  // Color table
  colors = create_color_table(); //Color hash

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
