#include <GL/gl.h>
#include <stdlib.h>
#include <unordered_map>
using namespace std;

GLfloat* make_color(float r, float g, float b)
{
    GLfloat *c = (GLfloat*) malloc(sizeof(GLfloat) * 3);
    c[0] = r;
    c[1] = g;
    c[2] = b;
    return c;
}

unordered_map<string, GLfloat*> create_color_table() {
    unordered_map<string, GLfloat*> colors = {
        {"black", make_color(0., 0., 0.)},
        {"white", make_color(1., 1., 1.)},
        {"gray", make_color(.5, .5, .5)},
        {"red", make_color(1., 0., 0.)},
        {"green", make_color(0., 1., 0.)},
        {"blue", make_color(0., 0., 1.)}
    };        
    return colors;
}
