#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>   

GLfloat gx, gy;

void display() {  

    glClearColor(0, 0, 0, 0);  
    glClear(GL_COLOR_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

    glBegin(GL_POLYGON);
        glColor3f(1, 0, 1);
        glVertex3f(0.25+gx, 0.25+gy, 0);
        
        glColor3f(0, 1, 1);
        glVertex3f(0.75+gx, 0.25+gy, 0);
        
        glColor3f(1, 0, 1);
        glVertex3f(0.75+gx, 0.75+gy, 0);
        
        glColor3f(0, 1, 1);
        glVertex3f(0.25+gx, 0.75+gy, 0);
    glEnd(); 

    glutSwapBuffers(); 
    
}

void keypress(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'w': 
        case 'W':
            gy += 0.01;
            break;
        case 's':
        case 'S':
            gy -= 0.01;
            break;
        case 'a':
        case 'A':
            gx -= 0.01;
            break; 
        case 'd':
        case 'D':
            gx += 0.01;
            break; 
    }
    
    printf("DAE\n");
    glutPostRedisplay();
} 

int main(int argc, char** argv) { 
    
    gx = 0;
    gy = 0;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);    
    glutInitWindowSize(500,500);         
    glutInitWindowPosition(100,100);     
    glutCreateWindow("Circlin"); 
    glutDisplayFunc(display);  
    glutKeyboardFunc(keypress);          
    
    glutMainLoop(); 
    return 0;

}
