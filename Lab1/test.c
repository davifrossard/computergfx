#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>   

int key_status[256];
GLfloat gx, gy;


void display() {  

    glClearColor(0, 0, 0, 0);  
    glClear(GL_COLOR_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

    glBegin(GL_POLYGON);
        glColor3f(1, 0, 1);
        glVertex3f(gx-0.25, gy-0.25, 0);
        
        glColor3f(0, 1, 1);
        glVertex3f(gx+0.25, gy-0.25, 0);
        
        glColor3f(1, 0, 1);
        glVertex3f(gx+0.25, gy+0.25, 0);
        
        glColor3f(0, 1, 1);
        glVertex3f(gx-0.25, gy+0.25, 0);
    glEnd(); 

    glutSwapBuffers(); 
    
}

void keyup(unsigned char key, int x, int y)
{
    key_status[key] = 0;
    int i;
    for(i=0; i<256; i++)
        printf("%d ", key_status[i]);
    printf("\n");
}


void keydown(unsigned char key, int x, int y)
{
    key_status[key] = 1;
    int i;
    for(i=0; i<256; i++)
        printf("%d ", key_status[i]);
    printf("\n");
}


void idle(void) {
    float inc = 0.01;
    if(key_status['w'] == 1 || key_status['W'] == 1)
        gy += inc;
    if(key_status['s'] == 1 || key_status['S'] == 1)
        gy -= inc;
    if(key_status['a'] == 1 || key_status['A'] == 1)
        gx -= inc;
    if(key_status['d'] == 1 || key_status['D'] == 1)
        gx += inc;
        
    glutPostRedisplay();
}

void mouse(int x, int y)
{
    gx = (float)x/1000.;
    gy = (float)(1000-y)/1000.;
    printf("%d %d\n", x, y);
}


void mouse_click(int btn, int state, int x, int y)
{
    printf("%d %d\n", x, y);
}

int main(int argc, char** argv) { 
    
    gx = 0;
    gy = 0;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);    
    glutInitWindowSize(1000,1000);         
    glutInitWindowPosition(100,100);     
    glutCreateWindow("Quadradeta"); 
    glutDisplayFunc(display);  
    glutKeyboardUpFunc(keyup);
    glutKeyboardFunc(keydown);
    glutIdleFunc(idle);
    glutPassiveMotionFunc(mouse);
    glutMotionFunc(mouse);
    glutSetCursor(GLUT_CURSOR_NONE);
    
    glutMainLoop(); 
    return 0;

}
