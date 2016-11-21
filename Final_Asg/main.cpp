#include <GL/glut.h>
#include <GL/gl.h>
#include <vector>
#include <tuple>
#include <ctime>
#include "car.h"
#include "arena.h"
using namespace std;

Car* player;
Arena* arena;
unordered_map<int,Car*> enemies;
unordered_map<string, GLfloat*> colors;
bool alive = 1, win = 0;
int current_cam = 0;
char current_time[10];
int key_status[256];
float velTiro, velCarro;
float eVelTiro, eFreqTiro, eVelCarro;


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

void draw()
{
  static GLfloat amb[] = {0., 0., 10., 10};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, amb);
  glEnable(GL_TEXTURE_2D);
  glPushMatrix();
  player->draw_car();
  for(auto e: enemies)
  {
    e.second->draw_car();
  }
  glPopMatrix();
  glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_CURRENT_BIT);
  arena->draw_arena();
  glPopAttrib();
  glDisable(GL_TEXTURE_2D);
}


void third_person_cam()
{
  sp_state ps = player->get_car_state();
  gluLookAt(ps.position.x-5*sin(ps.angle),
            ps.position.y+5*cos(ps.angle),
            ps.position.z+2.87649,  /* eye position */
            ps.position.x+60*sin(ps.angle),
            ps.position.y-60*cos(ps.angle),
            ps.position.z+0.77649,      /* lookat */
            0.0, 0.0, 1.0);      /* up is (0, 0, 1) */
  draw();
}

void first_person_cam()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, /* FOV */
                 0.8, /* aspect ratio */
                 0.1, 200.0); /* Z near and far */
  glMatrixMode(GL_MODELVIEW);
  glViewport(0, 0, 1000, 800);
  glLoadIdentity();
  sp_state ps = player->get_car_state();
  gluLookAt(ps.position.x+0.5*cos(ps.angle)-0.3*sin(ps.angle),
            ps.position.y+0.35*cos(ps.angle)+0.5*sin(ps.angle),
            ps.position.z+0.85,  /* eye position */
            ps.position.x+60*sin(ps.angle),
            ps.position.y-60*cos(ps.angle),
            ps.position.z+0.45,      /* lookat */
            0.0, 0.0, 1.0);      /* up is (0, 0, 1) */
  draw();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, /* FOV */
                 0.8, /* aspect ratio */
                 0.1, 200.0); /* Z near and far */
  glMatrixMode(GL_MODELVIEW);
  glViewport(0, 750, 1000, 1000);
  glLoadIdentity();
  gluLookAt(ps.position.x+2.5*sin(ps.angle),
            ps.position.y-2.5*cos(ps.angle),
            ps.position.z+3,  /* eye position */
            ps.position.x-60*sin(ps.angle),
            ps.position.y+60*cos(ps.angle),
            ps.position.z+15,      /* lookat */
            0.0, 0.0, 1.0);      /* up is (0, 0, 1) */
  draw();
}

void minimap() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-40, 40, -40, 40, -5, 5);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, 250, 250);
    glLoadIdentity();
    glPushAttrib(GL_CURRENT_BIT);
    arena->draw_arena_2d();
    glPopAttrib();
}

void display() {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(50.0, /* FOV */
                 1, /* aspect ratio */
                 0.1, 200.0); /* Z near and far */
  glMatrixMode(GL_MODELVIEW);
  glViewport(0, 0, 1000, 1000);
  glLoadIdentity();
  if(current_cam == 0) { /* third person cam */
    third_person_cam();
  } else if(current_cam == 1) { /* first person cam */
    first_person_cam();
  }

  minimap();

  glFlush();
  glutSwapBuffers();
}
void control_cannon(int x, int y)
{
  player->turn_cannon(x * (45 - (-45)) / (1000) + (-45));
  player->tip_cannon(((500-y) * 45 / 1000)*3);
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
      player->turn_wheel(-5);
    } if(key_status['d'] == 1 || key_status['D'] == 1) {
      player->turn_wheel(5);
  	} if(key_status['w'] == 1 || key_status['W'] == 1) {
      win = player->forward(-velCarro * elapsed_time);
  	} if(key_status['s'] == 1 || key_status['S'] == 1) {
      win = player->back(velCarro * elapsed_time);
  	} if(key_status['1'] == 1) {
      current_cam = 0;
  	} if(key_status['2'] == 1) {
      current_cam = 1;
  	} if(key_status['3'] == 1) {
      current_cam = 2;
  	}
    if(win)
      glutPostRedisplay();
    player->update_shots(velTiro * elapsed_time, &enemies, player);
    for(auto ck : enemies) {
      Car* c = ck.second;
      c->auto_forward(-eVelCarro * elapsed_time);
      if(cont >= (1./eFreqTiro)) {
        c->auto_turn_cannon();
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
    last_time = glutGet(GLUT_ELAPSED_TIME);
    glViewport(0, 500, 1000, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 10,  /* eye is at (.5,.5,-2) */
      0.0, 0.0, 0.0,      /* center is at (.5,.5,0) */
      0.0, -1.0, 0.0);      /* up is in positive Y direction */
    display();

    glViewport(0, 0, 1000, 500);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 10, 0,  /* eye is at (.5,.5,-2) */
      0.0, 0.0, 0.0,      /* center is at (.5,.5,0) */
      0.0, 0.0, 1.0);      /* up is in positive Y direction */
    display();
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
  // Glut stuff
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(1000,1000);
  glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-1000)/2,
                         (glutGet(GLUT_SCREEN_HEIGHT)-1000)/2);
  glutCreateWindow("Assignment 3");

  srand(time(NULL)); // Seed generator
  // Read configuration file
  if(argc < 2) {
      printf("Please provide configuration file\n");
      return 0;
  }
  string buffer = argv[1];
  string arena_file = parseXML(buffer); // Read config.xml
  // Read Arena
  arena = new Arena(arena_file);

  // Create player vehicle
  string car_file = "Obj_Parts/";
  GLuint ptexture[2];
  glGenTextures(2, ptexture);
  _load_texture((car_file+"tex1.bmp").c_str(), ptexture[0]);
  _load_texture((car_file+"tex2.bmp").c_str(), ptexture[1]);
  player = new Car(car_file, -1, NULL, arena, ptexture);
  // Create enemy vehicles
  GLuint etexture[2];
  glGenTextures(2, etexture);
  _load_texture((car_file+"tex3.bmp").c_str(), etexture[0]);
  _load_texture((car_file+"tex4.bmp").c_str(), etexture[1]);
  for(auto ek : arena->get_enemies()) {
    Car* enemy = new Car(car_file, ek.first, ek.second.color, arena, etexture);
    enemies[ek.first] = enemy;
  }

  // Color table
  colors = create_color_table(); //Color hash

  // More glut stuff
  glEnable(GL_DEPTH_TEST);
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
