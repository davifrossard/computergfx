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
float car_to_arena, arena_to_car;

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
    _draw_text(0., 0., current_time, colors["black"]);
    if(!alive) _draw_text(0., -0.02, "MORREU", colors["red"], GLUT_BITMAP_HELVETICA_18);
    if(win) _draw_text(0., -0.02, "GANHOU", colors["green"], GLUT_BITMAP_HELVETICA_18);
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
  if(alive && !win) {
    static float last_time = glutGet(GLUT_ELAPSED_TIME);
    tuple<float, float> result;
    float inc = 0;
    float elapsed_time = glutGet(GLUT_ELAPSED_TIME) - last_time;
    if(key_status['a'] == 1 || key_status['A'] == 1) {
      player->turn_wheel(-1);
    } if(key_status['d'] == 1 || key_status['D'] == 1) {
      player->turn_wheel(1);
  	} if(key_status['w'] == 1 || key_status['W'] == 1) {
      inc -= velCarro * elapsed_time;
      win = player->forward(inc);
  	} if(key_status['s'] == 1 || key_status['S'] == 1) {
      inc += velCarro * elapsed_time;
      win = player->back(inc);
  	}
    if(win)
      glutPostRedisplay();
    player->update_shots(velTiro * elapsed_time, &enemies, player);
    for(auto ck : enemies) {
      Car* c = ck.second;
      c->auto_forward(-velCarro * elapsed_time);
      c->auto_turn_cannon();
      if((rand() % 100) >= 91)
        c->shoot();
      if(c->update_shots(velTiro * elapsed_time, &enemies, player))
      {
        alive = 0;
        glutPostRedisplay();
      }

    }

    last_time = glutGet(GLUT_ELAPSED_TIME);
    int seconds = last_time / 1000;
    int minutes = seconds / 60;
    sprintf(current_time, "%02d:%02d:%02d", minutes, seconds%60, (int)last_time%1000);
    glutPostRedisplay();
  }
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
  srand(time(NULL)); // Seed generator
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

  // Create player vehicle
  string car_file = "car.svg";
  player = new Car(car_file, -1, NULL, arena);

  // Create enemy vehicles
  for(auto ek : arena->get_enemies())
  {
    Car* enemy = new Car(car_file, ek.first, ek.second.color, arena);
    enemies[ek.first] = enemy;
  }

  // Transformation constants
  car_to_arena = arena->get_player_diameter(); // Transfer from car to arena
  arena_to_car = 1/car_to_arena; // Transfer from arena to car
  velTiro = get<1>(configs) * car_to_arena; // Shot speed
  velCarro = get<2>(configs) * car_to_arena; // Car speed

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
