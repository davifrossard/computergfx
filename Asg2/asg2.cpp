#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "TinyXML/tinyxml.h"
#include "colours.h"
using namespace std;

#define NUM_POLYGONS 100
#define MOV_SPEED 0.003
int key_status[256];
GLfloat win_size, bg_color_r = 0, bg_color_g = 0, bg_color_b = 0;
vector<tuple<float, float, float, GLfloat*>> enemies;
vector<tuple<float, float, float, GLfloat*>> track;
tuple<float, float, float, GLfloat*> player;
tuple<float, float, float, float, GLfloat*> start;
const char * const configs[] = {"arquivoDaArena", "nome", "tipo", "caminho"};
//                                     0            1        2        3
const string ids[] = {"Pista", "LargadaChegada", "Inimigo", "Jogador"};
//                       0             1             2          3

void display() {  
    glClearColor(bg_color_r, bg_color_g, bg_color_b, 1);  
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(get<0>(track[0])-get<2>(track[0]), get<0>(track[0])+get<2>(track[0]), 
            get<1>(track[0])-get<2>(track[0]), get<1>(track[0])+get<2>(track[0]), 
            -1.0, 1.0); //Use {[cx-r,cx+r], [cy-r,cy+r], [-1, 1]} coordinate system
    float cx, cy, r;
    GLfloat* color;
    
    //Draw track
    for(tuple<float, float, float, GLfloat*> trk : track)
    {
        cx = get<0>(trk);
        cy = get<1>(trk);
        r = get<2>(trk);
        color = get<3>(trk);
        glColor3fv(color);
        glBegin( GL_TRIANGLE_FAN );
            glVertex3f(cx, cy, 0);
            for(int i = 0; i <= NUM_POLYGONS; i++)
            {
                double theta = i * 2.0f * M_PIl / NUM_POLYGONS;
                glVertex3f(cx + r * cosf(theta), cy + r * sinf(theta), 0);
            }
        glEnd();
    }
    
    //Draw enemies
    for(tuple<float, float, float, GLfloat*> enmy : enemies)
    {
        cx = get<0>(enmy);
        cy = get<1>(enmy);
        r = get<2>(enmy);
        color = get<3>(enmy);
        glColor3fv(color);
        glBegin( GL_TRIANGLE_FAN );
        glVertex2f(cx, cy);
            for(int i = 0; i <= NUM_POLYGONS; i++)
            {
                double theta = i * 2.0f * M_PIl / NUM_POLYGONS;
                glVertex2f(cx + r * cosf(theta), cy + r * sinf(theta));
            }
        glEnd();
    }
    
    //Draw start point
    float x = get<0>(start);
    float y = get<1>(start);
    float w = get<2>(start);
    float h = get<3>(start);
    color = get<4>(start);
    glBegin(GL_POLYGON);
        glColor3fv(color);
        glVertex3f(x, y, 0);
        
        glVertex3f(x, y+h, 0);
        
        glVertex3f(x+w, y+h, 0);
        
        glVertex3f(x+w, y, 0);
    glEnd();
    
    //Draw player
    cx = get<0>(player);
    cy = get<1>(player);
    r = get<2>(player);
    color = get<3>(player);
    glColor3fv(color);
    glBegin( GL_TRIANGLE_FAN );
    glVertex2f(cx, cy);
        for(int i = 0; i <= NUM_POLYGONS; i++)
        {
            double theta = i * 2.0f * M_PIl / NUM_POLYGONS;
            glVertex2f(cx + r * cos(theta), cy + r * sin(theta));
        }
    glEnd();

    glutSwapBuffers(); 
    
}

int parseXML(const char* file)
{
    unordered_map<string, GLfloat*> colors = create_color_table(); //Color hash
    TiXmlDocument config;
    config.LoadFile(file);  
    TiXmlElement* root = config.FirstChildElement();
    if(!root)
        return 0;
        
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
        }
    }
    
    TiXmlDocument imagefile;
    imagefile.LoadFile(img.c_str());  
    TiXmlElement* root2 = imagefile.FirstChildElement();
    if(!root2)
        return 0;
        
    // Read image file
    for(TiXmlElement* elem = root2->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement()) {
        string elemName = elem->Value();
        string elemId = elem->Attribute("id");
        if(elemId == ids[0]) { // Track
            float center_x = atof(elem->Attribute("cx"));
            float center_y = atof(elem->Attribute("cy"));
            float radius = atof(elem->Attribute("r"));
            GLfloat* color = colors[elem->Attribute("fill")]; 
            track.push_back(make_tuple(center_x, center_y, radius, color));
        } else if(elemId == ids[1]) { // Start
            float center_x = atof(elem->Attribute("x"));
            float center_y = atof(elem->Attribute("y"));
            float width = atof(elem->Attribute("width"));
            float height = atof(elem->Attribute("height"));
            GLfloat* color = colors[elem->Attribute("fill")]; 
            start = make_tuple(center_x, center_y, width, height, color);
        } else if(elemId == ids[2]) { // Enemies
            float center_x = atof(elem->Attribute("cx"));
            float center_y = atof(elem->Attribute("cy"));
            float radius = atof(elem->Attribute("r"));
            GLfloat* color = colors[elem->Attribute("fill")]; 
            enemies.push_back(make_tuple(center_x, center_y, radius, color));
        } else if(elemId == ids[3]) { // Player
            float center_x = atof(elem->Attribute("cx"));
            float center_y = atof(elem->Attribute("cy"));
            float radius = atof(elem->Attribute("r"));
            GLfloat* color = colors[elem->Attribute("fill")];
            player = make_tuple(center_x, center_y, radius, color);
        }
    }
    
    // Guarantee that outer ring comes first in the track vector
    if(get<2>(track[0]) < get<2>(track[1]))
        swap(track[0], track[1]);
    return 1;
}

void reshape(int w, int h)
{
    if (w > h)
        glViewport((w-h)/2., 0, (GLsizei) h, (GLsizei) h);
    else
        glViewport(0, (h-w)/2., (GLsizei) w, (GLsizei) w);
}

void keyup(unsigned char key, int x, int y)
{
    key_status[key] = 0;
}

void keydown(unsigned char key, int x, int y)
{
    key_status[key] = 1;
}

bool check_move(float cx_prime, float cy_prime) {
    float x, y, dist;
    float r = get<2>(player);
    
    //Valid track position
    x = pow(cx_prime - get<0>(track[0]), 2);
    y = pow(cy_prime - get<1>(track[0]), 2);
    dist = sqrt(x+y);
    if((dist - r) < get<2>(track[1]) || (dist+r) > get<2>(track[0]))
        return 0;
    
    //No collision with enemies
    for(tuple<float, float, float, GLfloat*> enemy : enemies)
    {
        x = pow(cx_prime - get<0>(enemy), 2);
        y = pow(cy_prime - get<1>(enemy), 2);
        float r2 = get<2>(enemy);
        dist = sqrt(x+y);
        if(dist < (r+r2))
            return 0;
    }

    return 1;
}

void idle(void) {
    float inc = MOV_SPEED*win_size;
    float cx_prime = get<0>(player), cy_prime = get<1>(player);
    if(key_status['w'] == 1 || key_status['W'] == 1) {
        cy_prime += inc;
        if(check_move(cx_prime, cy_prime) == 1)
        {
            get<0>(player) = cx_prime;
            get<1>(player) = cy_prime;
        }
    } 
    if(key_status['s'] == 1 || key_status['S'] == 1) {
        cy_prime -= inc;
        if(check_move(cx_prime, cy_prime) == 1)
        {
            get<0>(player) = cx_prime;
            get<1>(player) = cy_prime;
        }
    }
    if(key_status['a'] == 1 || key_status['A'] == 1) {
        cx_prime -= inc;
        if(check_move(cx_prime, cy_prime) == 1)
        {
            get<0>(player) = cx_prime;
            get<1>(player) = cy_prime;
        }
    }
    if(key_status['d'] == 1 || key_status['D'] == 1) {
        cx_prime += inc;
        if(check_move(cx_prime, cy_prime) == 1)
        {
            get<0>(player) = cx_prime;
            get<1>(player) = cy_prime;
        }
    }    
    
    glutPostRedisplay();
}

int main(int argc, char** argv) { 
    if(argc < 2) {
        printf("Please provide configuration file\n");
        return 0;
    }
    string buffer = argv[1];
    buffer+="/config.xml";

    if(parseXML(buffer.c_str()) == 0) {
        printf("Invalid configuration file\n");
        return 0;
    }
    
    win_size = get<2>(track[0])*2;
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);    
    glutInitWindowSize(win_size,win_size);         
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-win_size)/2,
                           (glutGet(GLUT_SCREEN_HEIGHT)-win_size)/2);   
    glutCreateWindow("Assignment 2"); 
   
    printf("Coordinate system:\n");
    printf("\t[%.2f, %.2f] [%.2f, %.2f]\n", get<0>(track[0])-get<2>(track[0]), get<0>(track[0])+get<2>(track[0]), 
            get<1>(track[0])-get<2>(track[0]), get<1>(track[0])+get<2>(track[0]));
            
    printf("\nTrack:\n");
    for(tuple<float, float, float, GLfloat*> obj : track)
        printf("\t%.2f %.2f %.2f (%.2f %.2f %.2f)\n", get<0>(obj), get<1>(obj), get<2>(obj), get<3>(obj)[0], get<3>(obj)[1], get<3>(obj)[2]);
    
    printf("\nEnemies:\n");
    for(tuple<float, float, float, GLfloat*> obj : enemies)
        printf("\t%.2f %.2f %.2f (%.2f %.2f %.2f)\n", get<0>(obj), get<1>(obj), get<2>(obj), get<3>(obj)[0], get<3>(obj)[1], get<3>(obj)[2]);
    
    printf("\nPlayer:\n");
    printf("\t%.2f %.2f %.2f (%.2f %.2f %.2f)\n", get<0>(player), get<1>(player), get<2>(player), get<3>(player)[0], get<3>(player)[1], get<3>(player)[2]);
    
    printf("\nStart Point:\n");
    printf("\t%.2f %.2f %.2f %.2f (%.2f %.2f %.2f)\n", get<0>(start), get<1>(start), get<2>(start), get<3>(start), get<4>(start)[0], get<4>(start)[1], get<4>(start)[2]);
    
    //Callbacks
    glutDisplayFunc(display);  
    glutReshapeFunc(reshape);
    glutKeyboardUpFunc(keyup);
    glutKeyboardFunc(keydown);
    glutIdleFunc(idle);
    glutMainLoop(); 
    return 0;
}
