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
bool night_mode = 0;
int current_cam = 0;
char current_time[10];
int key_status[256];
float velTiro, velCarro;
float eVelTiro, eFreqTiro, eVelCarro;
float camXY, camXZ, rbdown, c_angle, c_tip;

void headlights() {
  GLfloat white[4] = { 255, 0, 255, 255};
  GLfloat dir[4] = {0, -1, -0.5, 0};
  GLfloat zero[4] = {0, 0.1, 0, 1};
  sp_state ps = player->get_car_state();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(ps.position.x, ps.position.y, 0.6);
  glRotatef(ps.angle*180/M_PIl, 0, 0, 1);
    glPushMatrix();
    glTranslatef(0.88877, -1.5, 0.18182);
      glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, white);
      glLightfv(GL_LIGHT0, GL_SPECULAR, white);
      glLightfv(GL_LIGHT0, GL_POSITION, zero);
      glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
      glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 60);
      glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 5);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.88877, -1.5, 0.18182);
      glLightfv(GL_LIGHT1, GL_AMBIENT_AND_DIFFUSE, white);
      glLightfv(GL_LIGHT0, GL_SPECULAR, white);
      glLightfv(GL_LIGHT1, GL_POSITION, zero);
      glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dir);
      glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 60);
      glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 5);
    glPopMatrix();
  glPopMatrix();

  glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);
  glEnable(GL_LIGHTING);
}

void keyup(unsigned char key, int x, int y)
{
    key_status[key] = 0;
    if(key == 'n' || key == 'N') {
      if(night_mode) {
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        night_mode = 0;
        printf("Day mode\n");
      } else {
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        night_mode = 1;
        printf("Night mode\n");
      }
    }
}

void keydown(unsigned char key, int x, int y)
{
    key_status[key] = 1;
}

void draw()
{
  glEnable(GL_TEXTURE_2D);

  /* Draw player and enemies */
  player->draw_car();
  for(auto e: enemies)
  {
    e.second->draw_car();
  }

  /* Draw Arena */
  arena->draw_arena();
  headlights();

  glDisable(GL_TEXTURE_2D);
}


void third_person_cam()
{
  sp_state ps = player->get_car_state();
  float ca = c_angle * M_PI / 180.;
  gluLookAt(ps.position.x-1.95*sin(ps.angle),
            ps.position.y+1.95*cos(ps.angle),
            ps.position.z+2.55,  /* eye position */
            ps.position.x+60*sin(ps.angle-ca),
            ps.position.y-60*cos(ps.angle-ca),
            ps.position.z+1.05,      /* lookat */
            0.0, 0.0, 1.0);      /* up is (0, 0, 1) */
  draw();
}

void mouse_cam()
{
  sp_state ps = player->get_car_state();
  gluLookAt(ps.position.x + 8*sin(camXY*M_PI/180)*cos(camXZ*M_PI/180),
            ps.position.y - 8*cos(camXY*M_PI/180)*cos(camXZ*M_PI/180),
            ps.position.z + 8*sin(camXZ*M_PI/180),  /* eye position */
            ps.position.x,
            ps.position.y,
            ps.position.z,      /* lookat */
            0.0, 0.0, 1.0);     /* up is (0, 0, 1) */
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
            ps.position.z+1.45,  /* eye position */
            ps.position.x+60*sin(ps.angle),
            ps.position.y-60*cos(ps.angle),
            ps.position.z+1.05,      /* lookat */
            0.0, 0.0, 1.0);      /* up is (0, 0, 1) */
  draw();

  /* Rear view Mirror */
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
            ps.position.z+3.6,  /* eye position */
            ps.position.x-60*sin(ps.angle),
            ps.position.y+60*cos(ps.angle),
            ps.position.z+15,      /* lookat */
            0.0, 0.0, 1.0);      /* up is (0, 0, 1) */
  draw();
}

void minimap() {
  glDisable(GL_LIGHTING);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-40, 40, -40, 40, -5, 5);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, 250, 250);
    glLoadIdentity();
    glPushAttrib(GL_CURRENT_BIT);
    arena->draw_arena_2d();
    glPopAttrib();
  glEnable(GL_LIGHTING);
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
  } else if(current_cam == 2) {
    mouse_cam(); /* mouse controlled camera */
  }

  minimap();

  glFlush();
  glutSwapBuffers();
}

void control_cannon(int x, int y)
{
  c_angle = player->turn_cannon(x * (45 - (-45)) / (1000) + (-45));
  c_tip = player->tip_cannon(((500-y) * 45 / 1000)*3);
}

void control_camera(int x, int y)
{
  static int lastx, lasty;
  if(rbdown) {
    if(x > lastx) {
      camXY = (camXY < 180) ? camXY + 1 : camXY;
    } else if (x < lastx) {
      camXY = (camXY > -180) ? camXY - 1 : camXY;
    }
    if(y > lasty) {
      camXZ = (camXZ < 180) ? camXZ + 1 : camXZ;
    } else if (y < lasty) {
      camXZ = (camXZ > 0) ? camXZ - 1 : camXZ;
    }
    lastx = x;
    lasty = y;
  }
}

void shoot_cannon(int button, int state, int mx, int my)
{
  if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
  player->shoot();
  } else if(button == GLUT_RIGHT_BUTTON) {
    if(state == GLUT_DOWN) {
      rbdown = 1;
    } else {
      rbdown = 0;
    }
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
    glutPostRedisplay();
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
  glutCreateWindow("Final Assignment");

  srand(time(NULL)); // Seed generator
  // Read configuration file
  if(argc < 2) {
      printf("Please provide configuration file\n");
      return 0;
  }
  string buffer = argv[1];
  string arena_file = parseXML(buffer); // Read config.xml
  // Read Arena
  GLuint atexture[4];
  glGenTextures(4, atexture);
  _load_texture("Textures/ceiling_texture.bmp", atexture[0]);
  _load_texture("Textures/floor_texture.bmp", atexture[1]);
  _load_texture("Textures/inner_wall_texture.bmp", atexture[2]);
  _load_texture("Textures/wall_texture.bmp", atexture[3]);
  arena = new Arena(arena_file, atexture);

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
  glShadeModel(GL_SMOOTH);
  glutDisplayFunc(display);
  glutPassiveMotionFunc(control_cannon);
  glutMotionFunc(control_camera);
  glutKeyboardUpFunc(keyup);
  glutKeyboardFunc(keydown);
  glutMouseFunc(shoot_cannon);
  glutIdleFunc(idle);
  glutReshapeFunc(reshape);
  glutMainLoop();
  return 0;
}
