/*
 *  prog4_cheng.cpp:CSCE 5220 Program#4 (Polygonal surface)
 *  Guangchun Cheng
 *
 *  See the right-click popup menu for operations.
 *  The code tested on platforms of Windows and Linux (Ubuntu),
 *  and should be compatible on Mac but not guaranteed.
 */

#include <stdlib.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include <stdio.h>

const static GLint K = 40;              /* number of partition */
const static GLint NV = (K+1)*(K+1);    /* number of vertices: (K+1)^2 */
const static GLint NT = 2*K*K;          /* number of triangles: 2K^2 */
static GLfloat v[NV][3];                /* list of vertices */
static GLint ltri[NT][3];               /* list of triangles */
static GLfloat vn[NV][3];               /* vertex normals */

static GLfloat zoom_factor = 1.0;       /* zoom factor*/
static GLint width = 0.0, height = 0.0; /* window width and height*/
static GLint xrot = 0;                  /* eye position angle along X-axis */
static GLint yrot = 0;                  /* eye position angle along Y-axis */

/* get z-values using function f */
GLfloat f(GLfloat x, GLfloat y)
{
    return 0.5f*exp(-.04f*sqrt((80*x-40)*(80*x-40) + (90*y-45)*(90*y-45))) *
            cos(0.15f*sqrt((80*x-40)*(80*x-40) + (90*y-45)*(90*y-45)));
}

/* get the vertices*/
void storeVertices()
{
    GLfloat h = 1.0 / K;
    GLint indv = 0;
    for (GLint j=0; j<=K; ++j)
    {
        GLfloat y = j * h;
        for (GLint i=0; i<=K; ++i)
        {
            GLfloat x = i * h;
            v[indv][0] = x;         /* coordinates*/
            v[indv][1] = y;
            v[indv][2] = f(x,y);    /* function value*/
            indv = indv + 1;
        }
    }
}

/* form triangles using vertex indices*/
void storeTriangles()
{
    GLint indt = 0;
    for (GLint j=1; j<=K; ++j)
    {
        for (GLint i=1; i<=K; ++i)
        {
            int indv = j*(K+1) + i;
            ltri[indt][0] = indv-K-2;       /* vertex indices*/
            ltri[indt][1] = indv-K-1;
            ltri[indt][2] = indv;

            ltri[indt+1][0] = indv-K-2;     /* vertex indices*/
            ltri[indt+1][1] = indv;
            ltri[indt+1][2] = indv-1;
            indt = indt + 2;
        }
    }
}

/* initialize the vertex normals */
void initVertNormals()
{
    for (int indv=0; indv<=NV-1; ++indv)
    {
        vn[indv][0] = 0;
        vn[indv][1] = 0;
        vn[indv][2] = 0;
    }
}

/* normalize an n-dimensional vector */
void normalize(GLfloat vec[], GLint n)
{
    GLfloat length = 0.0f;
    for (GLint i=0; i<n; ++i)
    {
        length += vec[i]*vec[i];
    }
    length = sqrt(length);

    for (GLint i=0; i<n; ++i)
    {
        vec[i] = vec[i] / length;
    }
}

/* normailize vertex normals*/
void normlizeVertNormals()
{
    for (GLint indv=0; indv<=NV-1; ++indv)
    {
        normalize(vn[indv], 3);
    }
}

/* add triangle's normals to vertex's */
void addTri2VertNormals()
{
    for (GLint indt=0; indt<=NT-1; ++indt)
    {
        GLint i1 = ltri[indt][0];
        GLint i2 = ltri[indt][1];
        GLint i3 = ltri[indt][2];
        GLfloat tn[3];				/* triangle normals */
        tn[0] = (v[i2][1]-v[i1][1])*(v[i3][2]-v[i1][2]) -
                (v[i2][2]-v[i1][2])*(v[i3][1]-v[i1][1]);
        tn[1] = (v[i2][2]-v[i1][2])*(v[i3][0]-v[i1][0]) -
                (v[i2][0]-v[i1][0])*(v[i3][2]-v[i1][2]);
        tn[2] = (v[i2][0]-v[i1][0])*(v[i3][1]-v[i1][1]) -
                (v[i2][1]-v[i1][1])*(v[i3][0]-v[i1][0]);
        normalize(tn, 3);			/* normal triangle normals*/
		/* add it to the vertices composing the triangle */
        vn[i1][0] += tn[0]; vn[i1][1] += tn[1]; vn[i1][2] += tn[2];
        vn[i2][0] += tn[0]; vn[i2][1] += tn[1]; vn[i2][2] += tn[2];
        vn[i3][0] += tn[0]; vn[i3][1] += tn[1]; vn[i3][2] += tn[2];
    }
}

/* set up vertices and triangles*/
void init(void) 
{
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);	/* don't show hidden surfaces */
    glEnable(GL_LIGHTING);		/* enable lighting explicitly */
    glEnable(GL_LIGHT0);		/* enable light source-1 */
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); /* use two side lighting*/

    /* set up all the vertices and its normals, and triangles */
    storeVertices();
    storeTriangles();
    initVertNormals();
    addTri2VertNormals();
    normlizeVertNormals();

    /* use array vertex */
    glEnableClientState (GL_VERTEX_ARRAY);
    glEnableClientState (GL_NORMAL_ARRAY);
    glVertexPointer (3, GL_FLOAT, 0, v);
    glNormalPointer(GL_FLOAT, 0, vn);
}

void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f (1.0, 1.0, 1.0);
    glPushMatrix();

    /* draw the triangles using vertex array */
    glDrawElements(GL_TRIANGLES, NT*3, GL_UNSIGNED_INT, ltri);

    glPopMatrix();
    glutSwapBuffers();
}

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
    /* enable zooming by change the perspective angle */
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    GLfloat fovy = 60.0*zoom_factor>180 ? 180:(60.0*zoom_factor);
    gluPerspective(fovy, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
    /* make the object at the center and rotate by the center */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -3.0);
    glRotatef(xrot, 1.0, 0.0, 0.0);
    glRotatef(yrot, 0.0, 1.0, 0.0);
    glTranslatef(-0.5, -0.5, 0.0);
}

/************************************************************************/
/*			User Interfaces: Keyboard and Pop-up Menu                   */
/************************************************************************/
void selectMenuOption(int idCommand)
{
    switch (idCommand)
    {
        case 'x':
            xrot = (xrot + 5) % 360;
            reshape(width, height);
            break;
        case 'X':
            xrot = (xrot - 5) % 360;
            reshape(width, height);
            break;
        case 'y':
            yrot = (yrot + 5) % 360;
            reshape(width, height);
            break;
        case 'Y':
            yrot = (yrot - 5) % 360;
            reshape(width, height);
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
    glutPostRedisplay();
}

void keyboard (unsigned char key, int x, int y)
{
    selectMenuOption(key);
}

int popupMenu()
{
    int menu = glutCreateMenu(selectMenuOption);
    glutAddMenuEntry("x: Rotate by X-axis CCW", 'x');
    glutAddMenuEntry("X: Rotate by X-axis CW", 'X');
    glutAddMenuEntry("y: Rotate by Y-axis CCW", 'y');
    glutAddMenuEntry("Y: Rotate by Y-axis CW", 'Y');
    glutAddMenuEntry("z: Zoom in", 'z');
    glutAddMenuEntry("Z: Zoom out", 'Z');
    glutAddMenuEntry("<Esc>: Exit", 27);

    return menu;
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (550, 550);
    glutInitWindowPosition (100, 100);
    glutCreateWindow (argv[0]);
    init ();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    popupMenu();
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    glutMainLoop();
    return 0;
}
