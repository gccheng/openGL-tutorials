/*
 *  prog5_cheng.cpp:CSCE 5220 Program#5 (Quaternion)
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

#define PI (3.14159265)                         /* Pi value */

static GLint year1 = 0, year2 = 0;              /* year angles of two planets */
static GLint day1 = 0, day2 = 0;                /* day angles of two planets  */
static GLint moon = 0;                          /* moon angle of 2nd planet   */
static GLint year2angle = 1;                    /* orbit speed of 2nd planet  */
static GLdouble r = 6.6, zoom_factor = 1.0;     /* control parameters */
static GLint width = 550, height = 550;         /* width and height of the window */

static GLdouble orientation_angle = 0.0;                /* angle of orientation transformation */
static GLdouble orientation_axis[3] = {0.0, 0.0, 1.0};  /* axis of orientation transformation */
static GLdouble location_angle = 0.0;                   /* angle of location transformation */
static GLdouble location_axis[3] = {0.0, 0.0, 1.0};     /* axis of location transformation */

void init(void)
{
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_FLAT);
    glEnable(GL_DEPTH_TEST);
}

/* Quaternion multiplication: update R = Q*R */
void multiplyQuaternions(const GLdouble *Q, GLdouble *R)
{
    GLdouble qr1 = Q[0]*R[0]-Q[1]*R[1]-Q[2]*R[2]-Q[3]*R[3];
    GLdouble qr2 = Q[0]*R[1]+Q[1]*R[0]+Q[2]*R[3]-Q[3]*R[2];
    GLdouble qr3 = Q[0]*R[2]-Q[1]*R[3]+Q[2]*R[0]+Q[3]*R[1];
    GLdouble qr4 = Q[0]*R[3]+Q[1]*R[2]-Q[2]*R[1]+Q[3]*R[0];
    R[0] = qr1; R[1] = qr2; R[2] = qr3; R[3] = qr4;
}

/* Update transformation matrix R using quaternion multiplication */
void updateR (GLubyte c, GLdouble d, GLdouble *a, GLdouble *u)
{
    /* set quaternion R */
    GLdouble R[4] = {0.0};
    GLdouble radA = (*a)/180.0*PI;
    R[0] = cos(radA/2);
    R[1] = sin(radA/2)*u[0];
    R[2] = sin(radA/2)*u[1];
    R[3] = sin(radA/2)*u[2];

    /* set quaternion Q */
    GLdouble Q[4] = {0.0};
    GLdouble radD = d/180.0*PI;
    Q[0] = cos(radD/2);
    switch(c)
    {
        case 'x':
        case 'X':
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
            Q[1] = sin(radD/2)*1.0;
            break;
        case 'y':
        case 'Y':
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
            Q[2] = sin(radD/2)*1.0;
            break;
        case 'z':
        case 'Z':
            Q[3] = sin(radD/2)*1.0;
            break;
        default:
            break;
    }
    /* quaternion multiplication */
    multiplyQuaternions(Q, R);

    /* set return value. Due to machine precision, sometimes
       R[0] is a little out of range [-1,1], but the good news
       is sin(radA_2) cannot be absolutely zero! */
    GLdouble radA_2 = acos(R[0]>1.0?1.0:R[0]);
    if (fabs(sin(radA_2))>1e-8)
    {
        u[0] = R[1]/sin(radA_2);
        u[1] = R[2]/sin(radA_2);
        u[2] = R[3]/sin(radA_2);
    }
    (*a) = 2*radA_2/PI*180;
}

/* Display callback */
void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT);
    glClear (GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotated(orientation_angle, orientation_axis[0], orientation_axis[1], orientation_axis[2]);
    glTranslated(0.0, 0.0, -3.0*r);
    glRotated(location_angle, location_axis[0], location_axis[1], location_axis[2]);
    glTranslated(0.0, 0.0, 0.0);

    glPushMatrix();
    /* draw sun */
    glColor3f(1.0,1.0,0.0);
    glutSolidSphere(1.0, 20, 16);

    /* draw smaller planet */
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glRotatef ((GLfloat) year1, 0.0, 1.0, 0.0);
    glTranslatef (2.0, 0.0, 0.0);
    glRotatef ((GLfloat) day1, 0.0, 1.0, 0.0);
    glutSolidSphere(0.2, 10, 8);
    glPopMatrix();

    /* draw a second planet */
    glPushMatrix();
    glColor3f(0.0, 0.0, 1.0);
    glRotatef ((GLfloat) year2, 0.0, 1.0, 0.0);
    glTranslatef (5.0, 0.0, 0.0);
    glRotatef ((GLfloat) day2, 0.0, 1.0, 0.0);
    glutSolidSphere(0.5, 10, 8);
    /* draw the second planet's moon*/
    glColor3f(1.0, 1.0, 1.0);
    glRotatef ((GLfloat) moon, 0.0, 1.0, 0.0);
    glTranslatef (1.5, 0.0, 0.0);
    glutSolidSphere(0.1, 10, 8);
    glPopMatrix();

    glPopMatrix();
    glutSwapBuffers();
}

/* Reshape callback function with zooming operation */
void reshape (GLint w, GLint h)
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
    glFrustum(-r*zoom_factor, r*zoom_factor, -r*zoom_factor, r*zoom_factor, r, 6*r);
}

void keyboard (GLubyte key, GLint x, GLint y)
{
    switch (key) {
    case 'd':
        day1 = (day1 + 6) % 360;
        day2 = (day2 + 10) % 360;
        break;
    case 'D':
        day1 = (day1 - 6) % 360;
        day2 = (day2 - 10) % 360;
        break;
    case 'n':
        year1 = (year1 + 5) % 360;
        year2 = (year2 + 1) % 360;
        moon = (moon + 10*year2angle) % 360;
        break;
    case 'N':
        year1 = (year1 - 5) % 360;
        year2 = (year2 - 1) % 360;
        moon = (moon - 10*year2angle) % 360;
        break;
    case 'x':
    case 'y':
    case 'z':
        updateR(key,5.0, &orientation_angle, orientation_axis);
        break;
    case 'X':
    case 'Y':
    case 'Z':
        updateR(key,-5.0, &orientation_angle, orientation_axis);
        break;
    case 27:
        exit(0);
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

/* Special keyboad stroke callback */
void special_keyboard(GLint key, GLint x, GLint y)
{
    switch (key) {
    case GLUT_KEY_UP:
    case GLUT_KEY_LEFT:
        updateR(key, -5.0, &location_angle, location_axis);
        break;
    case GLUT_KEY_DOWN:
    case GLUT_KEY_RIGHT:
        updateR(key, 5.0, &location_angle, location_axis);
        break;
    case GLUT_KEY_PAGE_UP:
        zoom_factor = zoom_factor * (1.0/1.1);
        reshape(width, height);
        break;
    case GLUT_KEY_PAGE_DOWN:
        zoom_factor = zoom_factor * 1.1;
        reshape(width, height);
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

/* Menu callback */
void selectMenuOption(GLint idCommand)
{
    if ( (GLUT_KEY_UP+128==idCommand) || (GLUT_KEY_LEFT+128==idCommand)
         || (GLUT_KEY_DOWN+128==idCommand) || (GLUT_KEY_RIGHT+128==idCommand)
         || (GLUT_KEY_PAGE_DOWN+128==idCommand) || (GLUT_KEY_PAGE_UP+128==idCommand))
    {
        special_keyboard(idCommand-128, 0, 0);
    }
    else
    {
        keyboard(idCommand, 0, 0);
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
    glutAddMenuEntry("n: Planets oribt the sun counterclockwise", 'n');
    glutAddMenuEntry("N: Planets oribt the sun  clockwise", 'N');
    glutAddMenuEntry("-------------------------------------", -1);
    glutAddMenuEntry("x: Change user orientation about X axis CCW", 'x');
    glutAddMenuEntry("X: Change user orientation about X axis CW", 'X');
    glutAddMenuEntry("y: Change user orientation about Y axis CCW", 'y');
    glutAddMenuEntry("Y: Change user orientation about Y axis CW", 'Y');
    glutAddMenuEntry("z: Change user orientation about Z axis CCW", 'z');
    glutAddMenuEntry("Z: Change user orientation about Z axis CW", 'Z');
    glutAddMenuEntry("-------------------------------------", -2);
    glutAddMenuEntry("<Up Arrow>: Change user location about X axis CCW", 128+GLUT_KEY_UP);
    glutAddMenuEntry("<Down Arrow>: Change user location about X axis CW", 128+GLUT_KEY_DOWN);
    glutAddMenuEntry("<Left Arrow>: Change user location about Y axis CCW", 128+GLUT_KEY_LEFT);
    glutAddMenuEntry("<Right Arrow>: Change user location about Y axis CW", 128+GLUT_KEY_RIGHT);
    glutAddMenuEntry("--------------------------------------", -3);
    glutAddMenuEntry("<Page Up>: Zoom in", 128+GLUT_KEY_PAGE_UP);
    glutAddMenuEntry("<Page Down>: Zoom out", 128+GLUT_KEY_PAGE_DOWN);
    glutAddMenuEntry("<Esc>: Exit", 27);

    return menu;
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (650, 650);
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
