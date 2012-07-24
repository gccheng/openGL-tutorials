/*
 *  prog6_cheng.cpp:CSCE 5220 Program#6 (Bezier curve)
 *
 *  The program generates the piecewise Bezier curve using 7 control points, which are
 *  initially predefined and can be changed in an interactive way with some constraints.
 *  One constraint is that the two segments of the Bezier curve should have continuous
 *  tangents at the intersection point (P3). To do so, the three control points (i.e.
 *  the intersection point, its preceding (P2) and its succeeding (P4)) should remain
 *  collinear.
 *
 *  In the program, (1) when P2 (or P4) is moved, we change P4 (or P2) to keep the ratio
 *  of the distances r=|P2P3|/|P3P4|; (2) when P3 is moved, we add the same displacement
 *  of P3 (Delta=P3'-P3) to P2 and P4. In this way, the collinearity is restored after
 *  these three control points are moved.
 *
 *  Interactive moving the control points is implemented by user picking a control point
 *  in the window and dragging it to another position. Also user can change his location
 *  or orientation, and zoom in or out to get a better view.
 *
 *  The dragging interaction is implemented in the following way.
 *  (1) The window coordinates of the control points are computed using gluProject,
 *      then the coordinates are compared with the position of the cursor to determine
 *      if a control point has been selected. See getSelectedPoint().
 *  (2) After a control point Pi is selected, when the user moves the cursor, we use the
 *      cursor's position as Pi's new window coordinate, and project it back to world
 *      coordinate using gluUnProject. The trick here is how to get the Z value in the
 *      window coordinate of a control point, which is needed to restore the Z value in
 *      world coordinate. We get it by using the gluProject to the same control point
 *      before calling gluUnProject.
 *      See motion_callback.
 *
 *  Quaternion is used to change the user's location (eye position) and orientation.
 *
 *  See the right-click popup menu for operations and directions.
 *  The code tested on platforms of Windows and Linux (Ubuntu), and should be compatible
 *  on Mac but it has not been tested.
 *
 *  Guangchun Cheng (guangchuncheng@my.unt.edu)
 *  Date: 12/6/2011
 */

#include <stdlib.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include <stdio.h>

static const GLdouble PI = 3.14159265;                  /* Pi value */

static GLdouble zoom_factor = 1.0;                      /* zooming parameters */
static GLdouble orientation_angle = 0.0;                /* angle of orientation transformation */
static GLdouble orientation_axis[3] = {0.0, 0.0, 1.0};  /* axis of orientation transformation  */
static GLdouble location_angle = 0.0;                   /* angle of location transformation */
static GLdouble location_axis[3] = {0.0, 0.0, 1.0};     /* axis of location transformation  */
static GLint width = 550, height = 550;                 /* width and height of the window   */
static GLboolean leftBtnDown = GL_FALSE;                /* left button of mouse pressed */

static GLdouble ctrlpoints[7][3] = {                     /* control points */
                 {-4.0, 0, 0.0}, {-3.0, 2.0, 0.0},{-1.0, 2.0, 0.0}, {0.0, 0.0, 0.0},
                 {2.0, -4.0, 0.0}, {4.0, -2.0, 0.0}, {2.0, 1.0, 0.0}};
static GLint selectedPointIndex = -1;                    /* current seleted control point index */

GLint getSelectedPoint(GLint x, GLint y);
GLvoid getMinMax(GLdouble *minX, GLdouble *maxX, char axis);
GLdouble distance(GLdouble x[], GLdouble y[], GLint n);
GLdouble distance(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);

GLvoid init()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_MAP1_VERTEX_3);
}

GLvoid display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotated(orientation_angle, orientation_axis[0], orientation_axis[1], orientation_axis[2]);
    glTranslated(0.0, 0.0, -5.0);
    glRotated(location_angle, location_axis[0], location_axis[1], location_axis[2]);
    glTranslated(0.0, 0.0, 0.0);

    glColor3f(1.0, 1.0, 1.0);
    glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);
    glBegin(GL_LINE_STRIP);
    for (GLint i = 0; i <= 30; i++)
        glEvalCoord1f((GLdouble) i/30.0);
    glEnd();
    glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[3][0]);
    glBegin(GL_LINE_STRIP);
    for (GLint j = 0; j <= 30; j++)
        glEvalCoord1f((GLdouble) j/30.0);
    glEnd();

    /* The following code displays the control points as dots. */
    glPointSize(5.0);
    glBegin(GL_POINTS);
    if (-1 != selectedPointIndex)
    {
        glColor3f(0.0, 1.0, 1.0);
        glVertex3dv(&ctrlpoints[selectedPointIndex][0]);
    }
    glColor3f(1.0, 1.0, 0.0);
    for (GLint i = 0; i < 7; i++)
    {
        if (i != selectedPointIndex)
        {
            glVertex3dv(&ctrlpoints[i][0]);
        }
    }
    glEnd();
    glFlush();
}

GLvoid reshape(GLint w, GLint h)
{
    width = w; height = h;
    glViewport((w-w*zoom_factor)/2, (h-h*zoom_factor)/2, (GLsizei) w*zoom_factor, (GLsizei) h*zoom_factor);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glOrtho(-8.0, 8.0, -8.0*(GLfloat)h/(GLfloat)w,
                8.0*(GLfloat)h/(GLfloat)w, -10.0, 10.0);
    else
        glOrtho(-8.0*(GLfloat)w/(GLfloat)h,
                8.0*(GLfloat)w/(GLfloat)h, -8.0, 8.0, -10.0, 10.0);
}

/* Selected control point to move using mouse */
GLvoid mouse_callback(GLint button, GLint state, GLint x, GLint y)
{
    if ((GLUT_LEFT_BUTTON == button) && (GLUT_DOWN == state))
    {
        leftBtnDown = GL_TRUE;
        //Select a control point
        selectedPointIndex = getSelectedPoint(x, y);
    }
    else if ((GLUT_LEFT_BUTTON == button) && (GLUT_UP == state))
    {
        leftBtnDown = GL_FALSE;
        selectedPointIndex = -1;
    }
    glutPostRedisplay();
}

/* Update selected control point while mouse moving */
GLvoid motion_callback(GLint x, GLint y)
{
    if ((GL_TRUE == leftBtnDown) && (-1 != selectedPointIndex))
    {
        GLdouble modelview[16];                         /* modelview matrix */
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        GLdouble projection[16];                        /* projection matrix */
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        GLint viewport[4];                              /* viewport */
        glGetIntegerv(GL_VIEWPORT, viewport);

        /* Firstly get the projected winZ which is to be use in gluUnProject */
        GLdouble winX=0.0, winY=0.0, winZ=0.0;
        GLint ret =  gluProject(ctrlpoints[selectedPointIndex][0],
                                ctrlpoints[selectedPointIndex][1],
                                ctrlpoints[selectedPointIndex][2],
                                modelview, projection, viewport,
                                &winX, &winY, &winZ);
        if (GL_FALSE == ret)	return;

        /* Then map the window coordinate to world coordinate */
        GLdouble objX=0.0, objY=0.0, objZ=0.0;          /* object coordinate */
        ret = gluUnProject(x, height-y, winZ,
                           modelview, projection, viewport,
                           &objX, &objY, &objZ);
        if (GL_TRUE == ret)
        {
            /* previous coordinate */
            GLdouble prevX = ctrlpoints[selectedPointIndex][0];
            GLdouble prevY = ctrlpoints[selectedPointIndex][1];
            GLdouble prevZ = ctrlpoints[selectedPointIndex][2];

            /* Update the selected control point */
            ctrlpoints[selectedPointIndex][0] = objX;
            ctrlpoints[selectedPointIndex][1] = objY;
            ctrlpoints[selectedPointIndex][2] = objZ;

            /** Additional update to keep tangent continuity **/
            /* If it's either P2 (or P4), update P4 (or P2) */
            GLdouble ratio = distance(&ctrlpoints[2][0], &ctrlpoints[3][0], 3)
                    /distance(&ctrlpoints[3][0], &ctrlpoints[4][0], 3);
            if (2==selectedPointIndex)
            {
                ctrlpoints[4][0] = ctrlpoints[3][0]+(ctrlpoints[3][0]-ctrlpoints[2][0])/ratio;
                ctrlpoints[4][1] = ctrlpoints[3][1]+(ctrlpoints[3][1]-ctrlpoints[2][1])/ratio;
                ctrlpoints[4][2] = ctrlpoints[3][2]+(ctrlpoints[3][2]-ctrlpoints[2][2])/ratio;
            }
            else if (4==selectedPointIndex)
            {
                ctrlpoints[2][0] = ctrlpoints[3][0]+(ctrlpoints[3][0]-ctrlpoints[4][0])*ratio;
                ctrlpoints[2][1] = ctrlpoints[3][1]+(ctrlpoints[3][1]-ctrlpoints[4][1])*ratio;
                ctrlpoints[2][2] = ctrlpoints[3][2]+(ctrlpoints[3][2]-ctrlpoints[4][2])*ratio;
            }
            /* If it's P3, translate P2 and P4 the same vector as P3 */
            else if (3==selectedPointIndex)
            {
                ctrlpoints[2][0] += objX - prevX;
                ctrlpoints[2][1] += objY - prevY;
                ctrlpoints[2][2] += objZ - prevZ;

                ctrlpoints[4][0] += objX - prevX;
                ctrlpoints[4][1] += objY - prevY;
                ctrlpoints[4][2] += objZ - prevZ;
            }

            glutPostRedisplay();
        }
    }
}

/* Return selected point index by converting world coordinates into window
 * coordinates, and then compare them with coordinate of the cursor */
GLint getSelectedPoint(GLint x, GLint y)
{
    GLdouble modelview[16];                         /* modelview matrix */
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    GLdouble projection[16];                        /* projection matrix */
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    GLint viewport[4];                              /* viewport */
    glGetIntegerv(GL_VIEWPORT, viewport);

    GLdouble winX=0.0, winY=0.0, winZ=0.0;          /* window coordinate */
    for (GLint i=0; i<7; ++i)
    {
        GLint ret =  gluProject(ctrlpoints[i][0],ctrlpoints[i][1],ctrlpoints[i][2],
                                modelview, projection, viewport,
                                &winX, &winY, &winZ);
        if ( (GL_TRUE==ret) && (distance(winX, winY, x, height-y) < viewport[3]*0.01) )
            /*Note that it is WindowHeight-y, not ViewpotHeight -y */
        {
            return i;
        }
    }
    return -1;
}

/* Get the range along one axis */
GLvoid getMinMax(GLdouble *minX, GLdouble *maxX, char axis)
{
    (*minX) = 99999999.0;
    (*maxX) = -99999999.0;
    GLint dim = ('X'==axis) ? 0 : (('Y'==axis) ? 1 : 2);
    for(GLint i=0; i<7; ++i)
    {
        if (ctrlpoints[i][dim] < (*minX))
        {
            (*minX) = ctrlpoints[i][dim];
        }
        else if (ctrlpoints[i][dim] > (*maxX))
        {
            (*maxX) = ctrlpoints[i][dim];
        }
    }
}

/* Compute the distance between two points in the window */
GLdouble distance(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

/* Compute the distance between two points (vectors) in the world*/
GLdouble distance(GLdouble x[], GLdouble y[], GLint n)
{
    GLdouble ss = 0.0;
    for (GLint i=0; i<n; ++i)
    {
        ss += pow((x[i]-y[i]), 2);
    }
    return sqrt(ss);
}

/* Quaternion multiplication: update R = Q*R */
GLvoid multiplyQuaternions(const GLdouble *Q, GLdouble *R)
{
    GLdouble qr1 = Q[0]*R[0]-Q[1]*R[1]-Q[2]*R[2]-Q[3]*R[3];
    GLdouble qr2 = Q[0]*R[1]+Q[1]*R[0]+Q[2]*R[3]-Q[3]*R[2];
    GLdouble qr3 = Q[0]*R[2]-Q[1]*R[3]+Q[2]*R[0]+Q[3]*R[1];
    GLdouble qr4 = Q[0]*R[3]+Q[1]*R[2]-Q[2]*R[1]+Q[3]*R[0];
    R[0] = qr1; R[1] = qr2; R[2] = qr3; R[3] = qr4;
}

/* Update transformation matrix R using quaternion multiplication */
GLvoid updateR (GLubyte c, GLdouble d, GLdouble *a, GLdouble *u)
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

    /* set return value. Due to machine precision, sometimes R[0] is a little out of
       range [-1,1], but the good news is sin(radA_2) cannot be absolutely zero! */
    GLdouble radA_2 = acos(R[0]>1.0?1.0:R[0]);
    if (fabs(sin(radA_2))>1e-8)
    {
        u[0] = R[1]/sin(radA_2);
        u[1] = R[2]/sin(radA_2);
        u[2] = R[3]/sin(radA_2);
    }
    (*a) = 2*radA_2/PI*180;
}

/* Change user's orientation using keyboard */
GLvoid keyboard_callback(unsigned char key, int x, int y)
{
    switch (key)
    {
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
    }
    glutPostRedisplay();
}

/* Special keyboard stroke callback */
GLvoid special_keyboard(GLint key, GLint x, GLint y)
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
GLvoid selectMenuOption(GLint idCommand)
{
    if ( (GLUT_KEY_UP+128==idCommand) || (GLUT_KEY_LEFT+128==idCommand)
         || (GLUT_KEY_DOWN+128==idCommand) || (GLUT_KEY_RIGHT+128==idCommand)
         || (GLUT_KEY_PAGE_DOWN+128==idCommand) || (GLUT_KEY_PAGE_UP+128==idCommand))
    {
        special_keyboard(idCommand-128, 0, 0);
    }
    else
    {
        keyboard_callback(idCommand, 0, 0);
    }
}

/* Setup popup menu*/
int popupMenu()
{
    int menu = glutCreateMenu(selectMenuOption);
    glutAddMenuEntry("x: Change user orientation about X axis CCW", 'x');
    glutAddMenuEntry("X: Change user orientation about X axis CW", 'X');
    glutAddMenuEntry("y: Change user orientation about Y axis CCW", 'y');
    glutAddMenuEntry("Y: Change user orientation about Y axis CW", 'Y');
    glutAddMenuEntry("z: Change user orientation about Z axis CCW", 'z');
    glutAddMenuEntry("Z: Change user orientation about Z axis CW", 'Z');
    glutAddMenuEntry("______________________________________", -2);
    glutAddMenuEntry("^: Change user location about X axis CCW", 128+GLUT_KEY_UP);
    glutAddMenuEntry("v: Change user location about X axis CW", 128+GLUT_KEY_DOWN);
    glutAddMenuEntry("<: Change user location about Y axis CCW", 128+GLUT_KEY_LEFT);
    glutAddMenuEntry(">: Change user location about Y axis CW", 128+GLUT_KEY_RIGHT);
    glutAddMenuEntry("______________________________________", -1);
    glutAddMenuEntry("<Page Up>: Zoom in", 128+GLUT_KEY_PAGE_UP);
    glutAddMenuEntry("<Page Down>: Zoom out", 128+GLUT_KEY_PAGE_DOWN);
    glutAddMenuEntry("<Esc>: Exit", 27);
    glutAddMenuEntry("______________________________________", -101);
    glutAddMenuEntry("Pick a control point with left click", -102);
    glutAddMenuEntry("Drag that point while left button is pressed", -103);
    glutAddMenuEntry("Release the button to finish operation", -104);

    return menu;
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize (600, 600);
    glutInitWindowPosition (100, 100);
    glutCreateWindow (argv[0]);
    init ();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc (keyboard_callback);
    glutSpecialFunc(special_keyboard);
    glutMouseFunc(mouse_callback);
    glutMotionFunc(motion_callback);
    popupMenu();
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    glutMainLoop();
    return 0;
}
