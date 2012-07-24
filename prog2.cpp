/*
 *  prog2.cpp:CSCE 5220 Program#2 (Robot Arm)
 *  Guangchun Cheng
 *
 *  See the right-click popup menu for operations.
 *  The code tested on platforms of Windows and Linux (Ubuntu),
 *  and should be compatible on Mac but not guaranteed.
 *  This code is adapted based on OpenGL Redbook's robot.c.
 */

#include <stdlib.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include <stdio.h>

GLdouble getArmLength();
void drawCube(GLdouble szCube);
void setProjectAndModelView();

static GLint shoulder   = 45;       /*Angle of shouler*/
static GLint elbow      = 20;       /*Angle of elbow*/
static GLint wrist      = 10;       /*Angle of wrist*/
static GLint knuckle1   = 45;       /*Angle of knuckle-1*/
static GLint knuckle2   = 30;       /*Angle of knuckle-2*/
static GLint knuckle3   = 45;       /*Angle of knuckle-3*/
static bool wireframe   = GL_TRUE;  /*Draw modes: wireframe or solid*/
static bool perspective = GL_TRUE;  /*Projection modes*/
static GLfloat scaleUpperArm[3] = {5.0, 1.0, 1.0};  /*XYZ scales for upperarm*/
static GLfloat scaleForeArm[3]  = {4.0, 1.0, 1.0};  /*XYZ scales for forearm*/
static GLfloat scaleHand[3]     = {2.0, 1.0, 1.0};  /*XYZ scales for hand*/
static GLfloat scaleFinger[3]   = {0.8, 0.2, 0.2};  /*XYZ scales for hand*/
static GLint xrot = 45;         /*Eye position angle along X-axis*/
static GLint yrot = 300;        /*Eye position angle along Y-axis*/

void init(void)
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}

void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glPushMatrix();
   /*Draw ppper arm*/
   glRotatef ((GLfloat) shoulder, 0.0, 0.0, 1.0);
   glTranslatef (0.5*scaleUpperArm[0], 0.0, 0.0);
   glPushMatrix();
   glScalef (scaleUpperArm[0], scaleUpperArm[1], scaleUpperArm[2]);
   glColor3d(1, 0, 0);
   drawCube (1.0);
   glPopMatrix();
   /*Draw forearm*/
   glTranslatef (0.5*scaleUpperArm[0], 0.0, 0.0);
   glRotatef ((GLfloat) elbow, 0.0, 0.0, 1.0);
   glTranslatef (0.5*scaleForeArm[0], 0.0, 0.0);
   glPushMatrix();
   glScalef (scaleForeArm[0], scaleForeArm[1], scaleForeArm[2]);
   glColor3d(0, 1, 0);
   drawCube (1.0);
   glPopMatrix();
   /*Draw hand*/
   glTranslatef (0.5*scaleForeArm[0], 0.0, 0.0);
   glRotatef ((GLfloat) wrist, 0.0, 0.0, 1.0);
   glTranslatef (0.5*scaleHand[0], 0.0, 0.0);
   glPushMatrix();
   glScalef (scaleHand[0], scaleHand[1], scaleHand[2]);
   glColor3d(0, 0, 1);
   drawCube (1.0);
   glPopMatrix();

   /*Draw the three fingers for simplicity*/
   glPushMatrix();
   glTranslatef(0.5*scaleHand[0],0.4*scaleHand[1], 0.0);
   glRotatef ((GLfloat)knuckle1, 0.0, 1.0, 0.0 );
   glTranslatef (0.5*scaleFinger[0], 0.0, 0.0);
   glScalef (scaleFinger[0], scaleFinger[1], scaleFinger[2]);
   glColor3d(0, 1, 1);
   drawCube (1.0);
   glPopMatrix();

   glPushMatrix();
   glTranslatef(0.5*scaleHand[0], 0.0, 0.0);
   glRotatef ((GLfloat)knuckle2, 0.0, 1.0, 0.0 );
   glTranslatef (0.5*scaleFinger[0], 0.0, 0.0);
   glScalef (scaleFinger[0], scaleFinger[1], scaleFinger[2]);
   glColor3d(0, 1, 1);
   drawCube (1.0);
   glPopMatrix();

   glPushMatrix();
   glTranslatef(0.5*scaleHand[0], -0.4*scaleHand[1], 0.0);
   glRotatef ((GLfloat)knuckle3, 0.0, 1.0, 0.0 );
   glTranslatef (0.5*scaleFinger[0], 0.0, 0.0);
   glScalef (scaleFinger[0], scaleFinger[1], scaleFinger[2]);
   glColor3d(0, 1, 1);
   drawCube (1.0);
   glPopMatrix();

   glPopMatrix();
   glutSwapBuffers();
}

void reshape (int w, int h)
{
    if (w<h)
    {
        glViewport(0, (h-w)/2, (GLsizei)w, (GLsizei)w);
    }
    else
    {
        glViewport((w-h)/2, 0, (GLsizei)h, (GLsizei)h);
    }
    setProjectAndModelView();
}

/**********************************
*      Interaction Operation
**********************************/
//Menu callback function
void selectMenuOption(int idCommand)
{
    switch (idCommand) {
    case 'h':
        wireframe = !wireframe;
        break;
    case 'p':
        perspective = !perspective;
        setProjectAndModelView();
        break;
    case 's':
        shoulder = (shoulder + 5) % 360;
        break;
    case 'S':
        shoulder = (shoulder - 5) % 360;
        break;
    case 'e':
        elbow = (elbow + 5) % 360;
        break;
    case 'E':
        elbow = (elbow - 5) % 360;
        break;
    case 'w':
        wrist = (wrist + 5) % 360;
        break;
    case 'W':
        wrist = (wrist - 5) % 360;
        break;
    case '1':
        knuckle1 = (knuckle1 - 5) % 360;
        break;
    case '2':
        knuckle1 = (knuckle1 + 5) % 360;
        break;
    case '3':
        knuckle2 = (knuckle2 - 5) % 360;
        break;
    case '4':
        knuckle2 = (knuckle2 + 5) % 360;
        break;
    case '5':
        knuckle3 = (knuckle3 - 5) % 360;
        break;
    case '6':
        knuckle3 = (knuckle3 + 5) % 360;
        break;
    case 'k':
        knuckle1 = (knuckle1 - 5) % 360;
        knuckle2 = (knuckle2 - 5) % 360;
        knuckle3 = (knuckle3 - 5) % 360;
        break;
    case 'K':
        knuckle1 = (knuckle1 + 5) % 360;
        knuckle2 = (knuckle2 + 5) % 360;
        knuckle3 = (knuckle3 + 5) % 360;
        break;
    case 'x':
        xrot = (xrot + 5) % 360;
        setProjectAndModelView();
        break;
    case 'X':
        xrot = (xrot - 5) % 360;
        setProjectAndModelView();
        break;
    case 'y':
        yrot = (yrot + 5) % 360;
        setProjectAndModelView();
        break;
    case 'Y':
        yrot = (yrot - 5) % 360;
        setProjectAndModelView();
        break;
        case 'r':
                shoulder = 0; elbow = 0; wrist = 0;
                knuckle1 = 0; knuckle2 = 0; knuckle3 = 0;
                xrot = 0; yrot = 0;
                setProjectAndModelView();
                break;
    case 27:
        exit(0);
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

// Construct a popup menu
int popupMenu()
{
    int menu = glutCreateMenu(selectMenuOption);
    glutAddMenuEntry("h: Wireframe or solid fill", 'h');
    glutAddMenuEntry("p: Orthographic and perspective", 'p');
    glutAddMenuEntry("r: Reset all angles to zero", 'r');
    glutAddMenuEntry("----------------------------", -1);
    glutAddMenuEntry("s: Rotate the should counterclockwise", 's');
    glutAddMenuEntry("S: Rotate the should clockwise", 'S');
    glutAddMenuEntry("e: Rotate the elbow counterclockwise", 'e');
    glutAddMenuEntry("E: Rotate the elbow clockwise", 'e');
    glutAddMenuEntry("w: Rotate the wrist counterclockwise", 'w');
    glutAddMenuEntry("W: Rotate the wrist clockwise", 'W');
    glutAddMenuEntry("----------------------------", -2);
    glutAddMenuEntry("1: Rotate finger-1 counterclockwise", '1');
    glutAddMenuEntry("2: Rotate finger-1 clockwise", '2');
    glutAddMenuEntry("3: Rotate finger-2 counterclockwise", '3');
    glutAddMenuEntry("4: Rotate finger-2 clockwise", '4');
    glutAddMenuEntry("5: Rotate finger-3 counterclockwise", '5');
    glutAddMenuEntry("6: Rotate finger-3 clockwise", '6');
    glutAddMenuEntry("k: Rotate all fingers counterclockwise", 'k');
    glutAddMenuEntry("K: Rotate all fingers clockwise", 'K');
    glutAddMenuEntry("----------------------------", -3);
    glutAddMenuEntry("x: Increase xrot of eye position", 'x');
    glutAddMenuEntry("Y: Decrease xrot of eye position", 'X');
    glutAddMenuEntry("y: Increase yrot of eye position", 'y');
    glutAddMenuEntry("Y: Increase yrot of eye position", 'Y');

    return menu;
}

void keyboard (unsigned char key, int x, int y)
{
    selectMenuOption(key);
}

/**********************************
*      Auxiliary Functions
**********************************/
/*Calculate the arm length for better view*/
GLdouble getArmLength()
{
    return (GLdouble)(scaleUpperArm[0]*1.0
                      + scaleForeArm[0]*1.0 + scaleHand[0]*1.0);
}

/*Draw one segement acoording to drawing mode*/
void drawCube(GLdouble szCube)
{
    if (GL_TRUE == wireframe)
    {
        glutWireCube(szCube);
    }
    else
    {
        glutSolidCube(szCube);
    }
}

/*Switch between projection modes and reusme modleview*/
void setProjectAndModelView()
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    GLint w = viewport[2]; GLint h = viewport[3];

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    if (GL_TRUE == perspective)
    {
        gluPerspective(65.0, (GLfloat) w/(GLfloat) h,
                       1.0, 3.0*getArmLength()+1.0);
    }
    else
    {
        glOrtho(-1.5*getArmLength(), 1.5*getArmLength(),
                -1.5*getArmLength(), 1.5*getArmLength(),
                1.0, 3.0*getArmLength()+1.0);
    }
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef (0.0, 0.0, -2.0*getArmLength());
    // Rotate by X and Y axies before translating down -Z axis
    glRotatef(xrot, 1.0, 0.0, 0.0);
    glRotatef(yrot, 0.0, 1.0, 0.0);
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (800, 800);
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   //glutFullScreen();
   init ();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   popupMenu();
   glutAttachMenu(GLUT_RIGHT_BUTTON);
   glutMainLoop();
   return 0;
}
