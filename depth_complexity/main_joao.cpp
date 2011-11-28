//****************************************************************************
// File: dualray.cpp
//
// Author: Joao Comba (comba@inf.ufrgs.br)
//
// Description: Apply the dual ray transformation and generate either a
//		trapezoid or two triangles
//
//****************************************************************************

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifdef WIN32
#  include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define INCR 0.01
#define GLH_EXT_SINGLE_FILE
// #define CURVED

#define NSAMPLES 1000

#if 1
float colors[11][3] = {
  {0.0f, 0.0f, 0.0f},
  {1.0f, 0.0f, 0.0f},
  {0.0f, 1.0f, 0.0f},
  {0.0f, 0.0f, 1.0f},
  {0.5f, 0.0f, 0.5f},
  {0.5f, 0.5f, 0.0f},
  {0.0f, 0.5f, 0.5f},
  {0.5f, 0.5f, 0.5f},
  {1.0f, 0.0f, 1.0f},
  {0.0f, 1.0f, 1.0f},
  {0.0f, 0.5f, 1.0f},
};
#else
float colors[11][3] = {
  {0.0f, 0.0f, 0.0f},
  {1.0f, 0.0f, 0.0f},
  {0.0f, 1.0f, 0.0f},
  {0.0f, 0.0f, 1.0f},
  {0.5f, 0.0f, 0.5f},
  {0.5f, 0.5f, 0.0f},
  {0.0f, 0.5f, 0.5f},
  {0.5f, 0.5f, 0.5f},
  {1.0f, 0.0f, 1.0f},
  {0.0f, 1.0f, 1.0f},
  {0.0f, 0.5f, 1.0f},
};
#endif

short intersection_count[NSAMPLES][NSAMPLES];

#if 0
#define NLINES 3
double lines[3][4] = {
  {0.0f, 0.25f, 0.5f, 0.25f},
  {0.5f, 0.75f, 1.0f, 0.75f},
  {0.5f, 0.0f, 0.5f, 1.0f}
};
#else
#define NLINES 11
double lines[11][4] = {
  {0.5f, 0.125f, 0.75f, 0.25f},
  {0.75f, 0.25f, 0.875f, 0.5f},
  {0.875f, 0.5f, 0.75f, 0.75f},
  {0.75f, 0.75f, 0.5f, 0.625f},
  {0.5f, 0.625f, 0.25f, 0.75f},
  {0.25f, 0.75f, 0.125f, 0.5f},
  {0.125f, 0.5f, 0.375f, 0.5f},
  {0.375f, 0.5f, 0.125f, 0.375f},
  {0.125f, 0.375f, 0.5f, 0.3125f},
  {0.5f, 0.3125f, 0.25f, 0.25f},
  {0.25f, 0.25f, 0.5f, 0.125f},
};
#endif

double s1_x[2], s1_y[2], s2_x[2], s2_y[2], v_x[2], v_y[2];
double s_x[12], s_y[12];
double v_x_aux, v_y_aux;
double t1, t2, t3, t4, t5, t6, ti;
double u1, u2, d1, d2, l1, l2, r1, r2;
int testing = false;

int pointSel;
int displayL[20];
int displayV;
int displayCount=true;

typedef struct {
  double _v[2];
} Point2D;

typedef struct {
  Point2D _l[2];
} Line2D;

//-----------------------------------------------------------------------------
// lineIntersect
//-----------------------------------------------------------------------------
int lineIntersect(double s_x1, double s_y1, double s_x2, double s_y2, 
                  double v_x1, double v_y1, double v_x2, double v_y2,
                  float &t) {
  double s_dy = (s_y2-s_y1);
  double s_dx = (s_x2-s_x1);
  double v_dy = (v_y2-v_y1);
  double v_dx = (v_x2-v_x1);

  double a = v_dy;
  double b = -v_dx;
  double c = -(v_dy)*v_x1 + v_dx*v_y1;

  double denom = a * s_dx + b * s_dy;
  if (denom > -1.0E-5 && denom < 1.0E-5) {
    // printf ("\n No intersection");
    return 0;
  }

  double num = -(a * s_x1 + b * s_y1 + c);
  t = (num / denom);
  return 1;
}

//-----------------------------------------------------------------------------
// displayLine
//-----------------------------------------------------------------------------
void displayLine(double vx0, double vy0, double vx1, double vy1) {
  glColor3f(0.0, 0.0, 0.0);
  glBegin(GL_LINES);
    glVertex2f(vx0, vy0);
    glVertex2f(vx1, vy1);
  glEnd();
}

//-----------------------------------------------------------------------------
// displayScene
//-----------------------------------------------------------------------------
void displayScene(float color_r, float color_g, float color_b, int a, int b, int c, int d) {
  glColor3f(color_r, color_g, color_b);
  glPolygonMode(GL_FRONT, GL_LINE);
  glBegin(GL_POLYGON);
    glVertex3f(s_x[0], s_y[0], 0.0);
    glVertex3f(s_x[1], s_y[1], 0.0);
    glVertex3f(s_x[3], s_y[3], 0.0);
    glVertex3f(s_x[2], s_y[2], 0.0);
  glEnd();
	
  glLineWidth(2.0);
  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_LINES);
    glVertex3f(s_x[a], s_y[a], 0.0);
    glVertex3f(s_x[b], s_y[b], 0.0);
  glEnd();

  glColor3f(0.0, 1.0, 0.0);
  glBegin(GL_LINES);
    glVertex3f(s_x[c], s_y[c], 0.0);
    glVertex3f(s_x[d], s_y[d], 0.0);
  glEnd();
  glLineWidth(1.0);

  if (displayV) displayLine(v_x[0], v_y[0], v_x[1], v_y[1]);
  for (int i=0; i<NLINES; i++) {
    if (displayL[i]) {
      displayLine(lines[i][0], lines[i][1], lines[i][2], lines[i][3]); 
      // printf ("displaying line %d\n", i);
    }
  }
}

//-----------------------------------------------------------------------------
// displayDual
//-----------------------------------------------------------------------------
void displayDual(double vx0, double vy0, double vx1, double vy1, float r, float g, float b) {
  glColor4f(r, g, b, 0.025);
  float npts = NSAMPLES;
  for (int i = 0; i < npts; i++) {
    float t1 = (float)i/(float)npts;
    for (int j = 0; j < npts; j++) {
      float t2 = (float)j/(float)npts;
      double x1 = s1_x[0] + t1 * (s1_x[1]-s1_x[0]);
      double y1 = s1_y[0] + t1 * (s1_y[1]-s1_y[0]);
      double x2 = s2_x[0] + t2 * (s2_x[1]-s2_x[0]);
      double y2 = s2_y[0] + t2 * (s2_y[1]-s2_y[0]);
      float t;
      int result = lineIntersect(vx0, vy0, vx1, vy1, x1, y1, x2, y2, t);
      if (result && t >= 0 && t <= 1.0) {
	intersection_count[i][j]++;
	glBegin(GL_POINTS);
	glVertex3f(t1, t2, 0.0);
	glEnd();
      }
    }
  }
}

void computeDual(double vx0, double vy0, double vx1, double vy1,
                 int a, int b, int c, int d) {
  float npts = NSAMPLES;
  for (int i = 0; i < npts; i++) {
    float t1 = (float)i/(float)npts;
    for (int j = 0; j < npts; j++) {
      float t2 = (float)j/(float)npts;
      double x1 = s_x[a] + t1 * (s_x[b]-s_x[a]);
      double y1 = s_y[a] + t1 * (s_y[b]-s_y[a]);
      double x2 = s_x[c] + t2 * (s_x[d]-s_x[c]);
      double y2 = s_y[c] + t2 * (s_y[d]-s_y[c]);
      float t;
      int result = lineIntersect(vx0, vy0, vx1, vy1, x1, y1, x2, y2, t);
      if (result && t >= 0 && t <= 1.0) {
        intersection_count[i][j]++;
      }
    }
  }
}

void displayDual2() {
  float npts = NSAMPLES;
  int count;
  for (int i = 0; i < npts; i++) {
    float t1 = (float)i/(float)npts;
    for (int j = 0; j < npts; j++) {
      float t2 = (float)j/(float)npts;
      count = intersection_count[i][j]; 
      if (count>0) {
	glColor4f(colors[count][0], colors[count][1], colors[count][2], 1);
	glBegin(GL_POINTS);
          glVertex3f(t1, t2, 0.0);
	glEnd();
      }
    }
  }
}

//-----------------------------------------------------------------------------
// displayBox
//-----------------------------------------------------------------------------
void displayBox() {
  glColor3f(0, 0, 0);
  glLineWidth(2.0);
  glPolygonMode(GL_FRONT, GL_LINE);
  glBegin(GL_POLYGON);
  glVertex3f(s_x[0], s_y[0], 0.0);
  glVertex3f(s_x[1], s_y[1], 0.0);
  glVertex3f(s_x[3], s_y[3], 0.0);
  glVertex3f(s_x[2], s_y[2], 0.0);
  glEnd();
  glLineWidth(1.0);
}

//-----------------------------------------------------------------------------
// computeDualView
//-----------------------------------------------------------------------------
void computeDualView(int a, int b, int c, int d, float tx, float ty, float tz) {
  for (int i=0; i<NSAMPLES; i++)
    for (int j=0; j<NSAMPLES; j++)
      intersection_count[i][j] = 0;
  
  for (int i=0; i<NLINES; i++) 
    if (displayL[i])
      computeDual(lines[i][0], lines[i][1], lines[i][2], lines[i][3], a, b, c, d);

  if (displayV)
    computeDual(v_x[0], v_y[0], v_x[1], v_y[1], a, b, c, d);
  displayDual2();
  displayBox();
}

//-----------------------------------------------------------------------------
// display
//-----------------------------------------------------------------------------
void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if 0
  glPushMatrix();
  glTranslatef(-0.5, 0.5, 0.0);
  displayScene(0.5f, 0.5f, 0.2f, 0, 1, 2, 3);
  glTranslatef(0, -1.1, 0.0);
  computeDualView(10, 11, 8, 9, 1.1, 0.0, 0.0);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.6, 0.5, 0.0);
  displayScene(0.5f, 0.5f, 0.2f, 0, 2, 1, 3);
  glTranslatef(0, -1.1, 0.0);
  computeDualView(4, 6, 5, 7, 1.1, 0.0, 0.0);
  glPopMatrix();
#else
  glPushMatrix();
    glTranslatef(-0.5, 0.5, 0.0);
    displayScene(0.5f, 0.5f, 0.2f, 0, 1, 2, 3);
    glTranslatef(0, -1.1, 0.0);
    computeDualView(0, 1, 2, 3, 1.1, 0.0, 0.0);
  glPopMatrix();

  glPushMatrix();
    glTranslatef(0.6, 0.5, 0.0);
    displayScene(0.5f, 0.5f, 0.2f, 0, 2, 1, 3);
    glTranslatef(0, -1.1, 0.0);
    computeDualView(0, 2, 1, 3, 1.1, 0.0, 0.0);
  glPopMatrix();
#endif

#if 1

#ifdef CURVED
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(1.7, 0.5, 0.0);
  displayScene(0.5f, 0.5f, 0.2f, 0, 1, 0, 2);
  glTranslatef(0, -1.1, 0.0);
  computeDualView(0, 1, 0, 2, 1.1, 0.0, 0.0);
  glPopMatrix();
#else
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(1.7, 0.5, 0.0);
  displayScene(0.5f, 0.5f, 0.2f, 5, 1, 0, 2);
  glTranslatef(0, -1.1, 0.0);
  computeDualView(5, 1, 0, 2, 1.1, 0.0, 0.0);

  glTranslatef(0, -1.1, 0.0);
  displayScene(0.5f, 0.5f, 0.2f, 0, 1, 2, 8);
  glTranslatef(0, -1.1, 0.0);
  computeDualView(0, 1, 2, 8, 1.1, 0.0, 0.0);
  glPopMatrix();
#endif
  
#ifdef CURVED
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(2.8, 0.5, 0.0);
  displayScene(0.5f, 0.5f, 0.2f, 0, 1, 1, 3);
  glTranslatef(0, -1.1, 0.0);
  computeDualView(0, 1, 1, 3, 1.1, 0.0, 0.0);
  glPopMatrix();
#else
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(2.8, 0.5, 0.0);
  displayScene(0.5f, 0.5f, 0.2f, 4, 0, 1, 3);
  glTranslatef(0, -1.1, 0.0);
  computeDualView(4, 0, 1, 3, 1.1, 0.0, 0.0);

  glTranslatef(0, -1.1, 0.0);
  displayScene(0.5f, 0.5f, 0.2f, 0, 1, 3, 9);
  glTranslatef(0, -1.1, 0.0);
  computeDualView(0, 1, 3, 9, 1.1, 0.0, 0.0);
  glPopMatrix();
#endif

#ifdef CURVED
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(3.9, 0.5, 0.0);
  displayScene(0.5f, 0.5f, 0.2f, 0, 2, 2, 3);
  glTranslatef(0, -1.1, 0.0);
  computeDualView(0, 2, 2, 3, 1.1, 0.0, 0.0);
  glPopMatrix();
#else
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(3.9, 0.5, 0.0);
  displayScene(0.5f, 0.5f, 0.2f, 0, 2, 3, 7);
  glTranslatef(0, -1.1, 0.0);
  computeDualView(0, 2, 3, 7, 1.1, 0.0, 0.0);

  glTranslatef(0, -1.1, 0.0);
  displayScene(0.5f, 0.5f, 0.2f, 10, 0, 2, 3);
  glTranslatef(0, -1.1, 0.0);
  computeDualView(10, 0, 2, 3, 1.1, 0.0, 0.0);
  glPopMatrix();
#endif

#ifdef CURVED
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(5.0, 0.5, 0.0);
  displayScene(0.5f, 0.5f, 0.2f, 1, 3, 2, 3);
  glTranslatef(0, -1.1, 0.0);
  computeDualView(1, 3, 2, 3, 1.1, 0.0, 0.0);
  glPopMatrix();
#else
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(5.0, 0.5, 0.0);
  displayScene(0.5f, 0.5f, 0.2f, 1, 3, 2, 6);
  glTranslatef(0, -1.1, 0.0);
  computeDualView(1, 3, 2, 6, 1.1, 0.0, 0.0);

  glTranslatef(0, -1.1, 0.0);
  displayScene(0.5f, 0.5f, 0.2f, 1, 11, 2, 3);
  glTranslatef(0, -1.1, 0.0);
  computeDualView(1, 11, 2, 3, 1.1, 0.0, 0.0);
  glPopMatrix();
#endif
#endif

#if 0
  if (!displayCount) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
    if (displayV) displayDual(v_x[0], v_y[0], v_x[1], v_y[1], 0.4f, 0.4f, 0.5f);

    for (int i=0; i<NLINES; i++) 
      if (displayL[i])
          displayDual(lines[i][0], lines[i][1], lines[i][2], lines[i][3],
                      colors[i][0], colors[i][1], colors[i][2]);

    displayBox ();

    glDisable(GL_BLEND);
  }
  else {
    computeDualView(0, 1, 2, 3, 1.1, 0.0, 0.0);
    computeDualView(0, 2, 1, 3, 1.1, 0.0, 0.0);
    //computeDualView(4, 1, 5, 2, 1.1, 0.0, 0.0);
    computeDualView(0, 1, 1, 3, 1.1, 0.0, 0.0);
    computeDualView(0, 2, 2, 3, 1.1, 0.0, 0.0);
    computeDualView(1, 3, 2, 3, 1.1, 0.0, 0.0);
  }

  // if (displayL[0]) displayDual(0.0f, 0.25f, 0.5f, 0.25f, 1.0f, 0.0f, 0.0f);
  // if (displayL[1]) displayDual(0.5f, 0.75f, 1.0f, 0.75f, 0.0f, 1.0f, 0.0f);
  // if (displayL[2]) displayDual(0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f);

  glPopMatrix();
#endif

  glFlush();
  glutSwapBuffers();
  glFinish();
}

//-----------------------------------------------------------------------------
// reshape
//-----------------------------------------------------------------------------
void 
reshape(int w, int h) {
  //	float aspect = w/h;	
  if(h==0) h = 1;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();	
  glViewport(0, 0, w, h);
  //glOrtho(-1, 2, -1, 2, -1.0, 1.0);
  //glOrtho(-0.6, 1.6, -0.4, 2.2, -1.0, 1.0);
  glOrtho(-0.6, 6.1, -3.5, 1.8, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
}

//-----------------------------------------------------------------------------
// setupRC
//-----------------------------------------------------------------------------
void 
setupRC(void) {
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glClearColor(1.0, 1.0, 1.0, 1.0);
}

//-----------------------------------------------------------------------------
// keyboard
//-----------------------------------------------------------------------------
void keyboard( unsigned char key, int x, int y ) {
  switch( key ) {
  case '0':
    displayL[0] = ! displayL[0]; glutPostRedisplay(); break;
  case '1':
    displayL[1] = ! displayL[1]; glutPostRedisplay(); break;
  case '2':
    displayL[2] = ! displayL[2]; glutPostRedisplay(); break;
  case '3':
    displayL[3] = ! displayL[3]; glutPostRedisplay(); break;
  case '4':
    displayL[4] = ! displayL[4]; glutPostRedisplay(); break;
  case '5':
    displayL[5] = ! displayL[5]; glutPostRedisplay(); break;
  case '6':
    displayL[6] = ! displayL[6]; glutPostRedisplay(); break;
  case '7':
    displayL[7] = ! displayL[7]; glutPostRedisplay(); break;
  case '8':
    displayL[8] = ! displayL[8]; glutPostRedisplay(); break;
  case '9':
    displayL[9] = ! displayL[9]; glutPostRedisplay(); break;
  case 'a':
    displayL[10] = ! displayL[10]; glutPostRedisplay(); break;
  case 'v':
    displayV = ! displayV; glutPostRedisplay(); break;
  case 'c':
    displayCount = ! displayCount; glutPostRedisplay(); break;
  case 27:
    exit(0); break;
  case 't': 
    pointSel = ! pointSel; break;
  case 'i': 		
    testing = true;

    printf ("\n\nL1[%5.2lf] L2[%5.2lf] U1[%5.2lf] U2[%5.2lf] R1[%5.2lf] R2[%5.2lf] D1[%5.2lf] D2[%5.2lf]", l1, l2, u1, u2, r1, r2, d1, d2 ); 
    //if (((t1 >= 0.0) && (t1 <= 1.0)) && ((t2 >= 0.0) && (t2 <= 1.0)))
    if (((t1 > 0.0) && (t1 < 1.0)) && ((t2 > 0.0) && (t2 < 1.0)))
      printf ("\nOCORRE INTERSECCAO");
    else printf ("\nNAO OCORRE INTERSECCAO");
    printf ("\nPonto 1 [%5.3lf][%5.3lf] Ponto 2 [%5.3lf][%5.3lf]", v_x[0], v_y[0], v_x[1], v_y[1]);
    printf ("\n t1[%5.3lf] t2[%5.3lf]", t1, t2);		
    break;
  }
}

//-----------------------------------------------------------------------------
// special
//-----------------------------------------------------------------------------
void special( int key, int x, int y )
{
  switch( key ) {
    case GLUT_KEY_UP   : if (pointSel) v_y[0] += INCR; else v_y[1] += INCR; break;
    case GLUT_KEY_DOWN : if (pointSel) v_y[0] -= INCR; else v_y[1] -= INCR; break;
    case GLUT_KEY_RIGHT: if (pointSel) v_x[0] += INCR; else v_x[1] += INCR; break;
    case GLUT_KEY_LEFT : if (pointSel) v_x[0] -= INCR; else v_x[1] -= INCR; break;
  }
  if (v_x[0] > v_x[1]) {
      v_x_aux = v_x[0];
      v_y_aux = v_y[0];
      v_x[0] = v_x[1];
      v_y[0] = v_y[1];
      v_x[1] = v_x_aux;
      v_y[1] = v_y_aux;
      pointSel = ! pointSel;
  }
  glutPostRedisplay();
}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main (int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(1200, 800);
  glutCreateWindow("Dual Ray Implementation");
  glutDisplayFunc(display);	
  
  // Define base line		  
  s1_x[0] = 0.0; s1_y[0] = 0.0;
  s1_x[1] = 1.0; s1_y[1] = 0.0;
  // Define top line
  s2_x[0] = 0.0; s2_y[0] = 1.0;
  s2_x[1] = 1.0; s2_y[1] = 1.0;

  s_x[0] = 0.0; s_y[0] = 0.0;
  s_x[1] = 1.0; s_y[1] = 0.0;
  // Define top line
  s_x[2] = 0.0; s_y[2] = 1.0;
  s_x[3] = 1.0; s_y[3] = 1.0;
	
  //s_x[4] = 0.0; s_y[4] = -1.0;
  //s_x[5] = -1.0; s_y[5] = -1.0;

  s_x[4] = 0.0; s_y[4] = -1.0;
  s_x[5] = 1.0; s_y[5] = -1.0;	
  s_x[6] = 0.0; s_y[6] = 2.0;
  s_x[7] = 1.0; s_y[7] = 2.0;	
  s_x[8] = -1.0; s_y[8] = 1.0;
  s_x[9] = 2.0; s_y[9] = 1.0;
  s_x[10] = -1.0; s_y[10] = 0.0;
  s_x[11] = 2.0; s_y[11] = 0.0;

  // Define query line
  //v_x[0] = 0.5; v_y[0] = 0.5;
  //v_x[1] = 0.5; v_y[1] = 0.4;
  //v_x[0] = 0.0; v_y[0] = 0.2;
  //v_x[1] = 0.1; v_y[1] = 0.0;
  
  v_x[0] = 0.5; v_y[0] = 0.73;	
  v_x[1] = 0.30; v_y[1] = 0.1;	

  pointSel = true;
  displayV = false;
  for (int i=0; i<NLINES; i++) displayL[i] = true;

  setupRC();
  glutReshapeFunc(reshape);
  glutKeyboardFunc( keyboard );
  glutSpecialFunc( special );
  
  glutMainLoop();

  return 0;
}
