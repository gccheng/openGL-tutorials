/*
 *  prog3_cheng.cpp:CSCE 5220 Program#3 (Planet)
 *  Guangchun Cheng
 *
 *  See the right-click popup menu for operations.
 *  The code tested on platforms of Windows and Linux (Ubuntu),
 *  and should be compatible on Mac but not guaranteed.
 *  This code is adapted based on OpenGL Redbook's planet.c.
 */

#include <stdlib.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include <stdio.h>

static int year1 = 0, year2 = 0;        /* year angles of two planets */
static int day1 = 0, day2 = 0;          /* day angles of two planets  */
static int moon = 0;                    /* moon angle of 2nd planet   */
static int year2angle = 1;              /* orbit speed of 2nd planet  */
static double r = 6.6, zoom_factor = 1.0;   /* control parameters */
static int width = 550, height = 550;       /* width and height of the window */
static double x_move = 0.0;                 /* move in x-direction for viewing */

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}

void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT);
   glColor3f (1.0, 1.0, 1.0);

   glPushMatrix();
   /* draw sun */
   glutWireSphere(1.0, 20, 16);

   /* draw smaller planet */
   glPushMatrix();
   glRotatef ((GLfloat) year1, 0.0, 1.0, 0.0);
   glTranslatef (2.0, 0.0, 0.0);
   glRotatef ((GLfloat) day1, 0.0, 1.0, 0.0);
   glutWireSphere(0.2, 10, 8);
   glPopMatrix();

   /* draw a second planet */
   glPushMatrix();
   glRotatef ((GLfloat) year2, 0.0, 1.0, 0.0);
   glTranslatef (5.0, 0.0, 0.0);
   glRotatef ((GLfloat) day2, 0.0, 1.0, 0.0);
   glutWireSphere(0.5, 10, 8);
    /* draw the second planet's moon*/
   glRotatef ((GLfloat) moon, 0.0, 1.0, 0.0);
   glTranslatef (1.5, 0.0, 0.0);
   glutWireSphere(0.1, 10, 8);
   glPopMatrix();

   glPopMatrix();
   glutSwapBuffers();
}

void reshape (int w, int h)
{
    width = w; height = h;
    if (w<h)
    {
        glViewport(0, (h-w)/2, (GLsizei)w, (GLsizei)w);
    }
    else
    {
        glViewport((w-h)/2, 0, (GLsizei)h, (GLsizei)h);
    }
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   glFrustum(-r*zoom_factor, r*zoom_factor, -r*zoom_factor, r*zoom_factor, r, 3*r);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef (x_move, 0.0, 0);
   gluLookAt (0.0, 0.0, 2*r, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void keyboard (unsigned char key, int x, int y)
{
   switch (key) {
        case 'd':
         day1 = (day1 + 6) % 360;
         day2 = (day2 + 10) % 360;
         glutPostRedisplay();
         break;
        case 'D':
         day1 = (day1 - 6) % 360;
         day2 = (day2 - 10) % 360;
         glutPostRedisplay();
         break;
        case 'y':
         year1 = (year1 + 5) % 360;
         year2 = (year2 + 1) % 360;
         moon = (moon + 10*year2angle) % 360;
         glutPostRedisplay();
         break;
        case 'Y':
         year1 = (year1 - 5) % 360;
         year2 = (year2 - 1) % 360;
         moon = (moon - 10*year2angle) % 360;
         glutPostRedisplay();
         break;
        case 'Z':
         zoom_factor = zoom_factor*1.1;
         reshape(width, height);
         glutPostRedisplay();
         break;
        case 'z':
          zoom_factor = zoom_factor*(1.0/1.1);
          reshape(width, height);
          glutPostRedisplay();
          break;
        case 27:
          exit(0);
          break;
        default:
         break;
   }
}

void special_keyboard(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_PAGE_UP:
            zoom_factor = zoom_factor*(1.0/1.1);
            reshape(width, height);
            glutPostRedisplay();
            break;
        case GLUT_KEY_PAGE_DOWN:
            zoom_factor = zoom_factor*1.1;
            reshape(width, height);
            glutPostRedisplay();
            break;
        case GLUT_KEY_LEFT:
            x_move = x_move + 0.5;
            reshape(width, height);
            glutPostRedisplay();
            break;
        case GLUT_KEY_RIGHT:
            x_move = x_move - 0.5;
            reshape(width, height);
            glutPostRedisplay();
            break;
        default:
            break;
    }
}

void selectMenuOption(int idCommand)
{
    if (('d'==idCommand) || ('D'==idCommand) || ('y'==idCommand) || ('Y'==idCommand)
            || ('z'==idCommand) || ('Z'==idCommand) || (27==idCommand))
    {
        keyboard(idCommand, 0, 0);
    }
    else
    {
        special_keyboard(idCommand-128, 0, 0);
    }
}

/* There's a trick here for "128+". We notice that GLUT_KEY_UP=101, GLUT_KEY_DOWN=103,
 * GLUT_KEY_LEFT=100, GLUT_KEY_RIGHT=102, while 'd'=100, 'e'=101, 'f'=102, 'g'=103, and
 * so on. Therefore, there's a potential risk of conflicts, and we thus add 128 to the
 * GLUT enum values to avoid conflicts since the maximum ASCII is 127.
*/
int popupMenu()
{
    int menu = glutCreateMenu(selectMenuOption);
    glutAddMenuEntry("d: Planets rotate counterclockwise", 'd');
    glutAddMenuEntry("D: Planets rotate clockwise", 'D');
    glutAddMenuEntry("y: Planets oribt the sun counterclockwise", 'y');
    glutAddMenuEntry("Y: Planets oribt the sun  clockwise", 'Y');
    glutAddMenuEntry("z: Zoom in", 'z');
    glutAddMenuEntry("Z: Zoom out", 'Z');
    glutAddMenuEntry("----------------------------", -1);
    glutAddMenuEntry("<Page Up>:   Zoom in", 128+GLUT_KEY_PAGE_UP);
    glutAddMenuEntry("<Page Down>: Zoom out", 258+GLUT_KEY_PAGE_DOWN);
    glutAddMenuEntry("<Left Arrow>:  Move the view leftwards", 128+GLUT_KEY_LEFT);
    glutAddMenuEntry("<Right Arrow>: Move the view rightwards", 128+GLUT_KEY_RIGHT);
    glutAddMenuEntry("----------------------------", -2);
    glutAddMenuEntry("<Esc>: Exit", 27);

    return menu;
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize (550, 550);
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutSpecialFunc(special_keyboard);
   popupMenu();
   glutAttachMenu(GLUT_RIGHT_BUTTON);
   glutMainLoop();
   return 0;
}
