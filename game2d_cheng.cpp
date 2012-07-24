/*
 *  game2d_cheng.cpp
 *  Guangchun Cheng
 *
 *  Resize the window to change the configuration of the triangle and to restart the process.
 *  The code tested on platforms of Windows and Linux (Ubuntu), and should be compatible on 
 *  Mac but it is not tested.
 */

#include <stdlib.h>
#include <time.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>

static GLint T[3][2] = {0};				/* triangle */
static GLint ptCurrent[2] = {0};                        /* current point */
static GLint indRandom = 0;				/* chosen vertex */

static GLint viewsize = 0;				/* viewport size */
static GLint origin_left = 0, origin_bottom = 0;        /* origin of viewport*/
static GLint width = 0, height = 0;                     /* window size*/
static GLboolean clearcolor = GL_TRUE;                  /* erase background? */

void init(void) 
{
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_FLAT);

    glPointSize(2.0);
    srand(time(NULL));                                  /* initialize random seed */
}

void initSetting()
{
    indRandom = rand()%3;                               /* initialize vertex index*/

    ptCurrent[0] = rand() % viewsize;                   /* initialize current point */
    ptCurrent[1] = rand() % viewsize;

    T[0][0] = rand() % viewsize + origin_left;          /* initialize the triangle */
    T[0][1] = rand() % viewsize + origin_bottom;
    T[1][0] = rand() % viewsize + origin_left;
    T[1][1] = rand() % viewsize + origin_bottom;
    T[2][0] = rand() % viewsize + origin_left;
    T[2][1] = rand() % viewsize + origin_bottom;

    clearcolor = GL_TRUE;                               /* erase background */
}

void display(void)
{
    if (GL_TRUE == clearcolor)
    {
        glClear(GL_COLOR_BUFFER_BIT);
        clearcolor = GL_FALSE;
    }

    glPushMatrix();

    glBegin(GL_LINE_LOOP);
        glColor3f (1.0, 1.0, 1.0);
        glVertex2iv(T[0]);
        glVertex2iv(T[1]);
        glVertex2iv(T[2]);
    glEnd();

    /* Update current point */
    ptCurrent[0] = (ptCurrent[0]+T[indRandom][0])/2;
    ptCurrent[1] = (ptCurrent[1]+T[indRandom][1])/2;

    glBegin(GL_POINTS);
         glColor3f(0.0, 0.0, 1.0);
    glVertex2iv(ptCurrent);
    glEnd();

    glPopMatrix();
    glFlush();
}

void reshape (GLint w, GLint h)
{
    viewsize = (w<h) ? w : h;
    width = w; height = h;
    if (w<h)
    {
        origin_left = 0;
        origin_bottom = (h-w)/2;
    }
    else
    {
        origin_left = (w-h)/2;
        origin_bottom = 0;
    }
    initSetting();

    glViewport(origin_left, origin_bottom, viewsize, viewsize);
    
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluOrtho2D(origin_left, origin_left+viewsize, origin_bottom, origin_bottom+viewsize);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboardCallback (GLubyte key, GLint x, GLint y)
{
    switch (key) {
    case 27:
        exit(0);
    default:
        break;
    }
}

void timerCallback(GLint val)
{
    indRandom = rand()%3;
    glutPostRedisplay();
    glutTimerFunc(25,timerCallback, 0);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_RGB);
    glutInitWindowSize (550, 550);
    glutInitWindowPosition (100, 100);
    glutCreateWindow (argv[0]);
    init ();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardCallback);
    glutTimerFunc(25,timerCallback, 0);

    glutMainLoop();
    return 0;
}
