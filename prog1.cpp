/*
 *  progl.cpp:CSCE 5220 Program#1 (Rorating a pendulum)
 *
 *  Guangchun Cheng
 *
 *  This is a program rotating pendulum.
 *  Pressing the '+' key rotates the pendulum counterclockwise.
 *  Pressing the '-' key rotates the pendulum clockwise.
 *  Pressing the '<Space>' key stops the spinning.
 *  Pressing the '<Escape>' key exits the program.
 *  Mouse right click on the window invokes the popup menu
 *
 *  And,
 *  Mouse left click on the window starts the spinning
 *  Mouse middle click on the window stops the spinning
 */

#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

#define PI 3.14159265

static GLfloat spin = 0.0;          // spin degree
static GLfloat r = 1.0;             // radius of the pendulum
static GLfloat l = 10.0;            // length of the wire
static GLboolean ccw = GL_TRUE;     // indicator of counter-clockwise or clockwise

enum {                              // values of menu entries
    MENU_CCW = '+',
    MENU_CW = '-',
    MENU_PAUSE = 32,
    MENU_EXIT = 27
};

void display(void)
{
    glPushMatrix();

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glLoadIdentity();

    // draw the anchor point
    glBegin(GL_POINTS);
        glVertex2f(0,0);
    glEnd();

    // draw the wire
    GLfloat xPendulum = (r+l)*cos(spin*PI/180);
    GLfloat yPendulum = (r+l)*sin(spin*PI/180);
    glBegin(GL_LINES);
        glVertex2f(0.0f,0.0f);
        glVertex2f(xPendulum, yPendulum);
    glEnd();

    // draw the pendulum
    glRotatef(spin, 0.0, 0.0, 1.0);                 // model transformation
    glTranslatef(r+l, 0.0, 0.0);
    GLUquadric* pNewQuadric = gluNewQuadric();      // create a quadric object
    if (0==pNewQuadric)
    {
        perror("gluNewQuadric: No enough memory!");
        return;
    }
    gluQuadricDrawStyle(pNewQuadric, GLU_FILL);
    gluDisk(pNewQuadric, 0.0, (GLdouble)r, 32, 32); // draw the pendulum

    glutSwapBuffers();
    glPopMatrix();
}

void spinDisplay(void)
{
    // counter-clockwise or clockwise spinning
    if (GL_TRUE==ccw)
    {
        spin = spin + 1.0;
    }    else
    {
        spin = spin - 1.0;
    }
    // limit the spin within [0 360]
    if (spin > 360.0)
    {
        spin = spin - 360.0;
    }
    if (spin < 0)
    {
        spin = spin + 360.0;
    }
    glutPostRedisplay();
}

void init(void)
{
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_FLAT);
}

// reshape function keeps the square aspect ratio
void reshape(int w, int h)
{
    if (w<h) // use the smaller value to keep square aspect ratio
    {
        glViewport(0, (h-w)/2, (GLsizei)w, (GLsizei)w);
    }
    else
    {
        glViewport((w-h)/2, 0, (GLsizei)h, (GLsizei)h);
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat a = 1.25*l;
    gluOrtho2D(-a, a, -a, a);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// menu callback function
void selectMenuOption(int idCommand)
{
    switch(idCommand)
    {
    case MENU_CCW:
        ccw = GL_TRUE;
        glutIdleFunc(spinDisplay);
        break;
    case MENU_CW:
        ccw = GL_FALSE;
        glutIdleFunc(spinDisplay);
        break;
    case MENU_PAUSE:
        glutIdleFunc(NULL);
        break;
    case MENU_EXIT:
        exit(0);
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

// construct a popup menu
int popupMenu()
{
    int menu;

    menu = glutCreateMenu(selectMenuOption);
    glutAddMenuEntry("+:Start spinning couterslockwise", MENU_CCW);
    glutAddMenuEntry("-:Start spinning clockwise", MENU_CW);
    glutAddMenuEntry("<space>: Stop the spinning", MENU_PAUSE);
    glutAddMenuEntry("<Escape>:Exit", MENU_EXIT);

    return menu;
}

void mouse(int button, int state, int x, int y)
{
    switch (button) {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN)
            glutIdleFunc(spinDisplay);
        break;
    case GLUT_MIDDLE_BUTTON:
    case GLUT_RIGHT_BUTTON:
        if (state == GLUT_DOWN)
            glutIdleFunc(NULL);
        break;
    default:
        break;
    }
}

// keyboard callback simply invokes the menu callback function
void keyboard(unsigned char key, int x, int y)
{
    selectMenuOption(key);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (500, 500);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("Guangchun Cheng: Program #1");
    init ();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    popupMenu();
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    glutMainLoop();

    return 0;
}
