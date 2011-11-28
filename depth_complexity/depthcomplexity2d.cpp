#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "dc_2d.h"
#include "flags.h"
#include "timer.h"

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <fstream>
#include <iostream>

const double kInc = 0.01;
int pointSel = 0;
bool showRays = false;

std::vector<Segment> segments;
DepthComplexity2D *dc2d;

std::vector<Segment> loadOFFSegments(std::istream& in){
  std::clog << "Loading OFF file" << std::endl;

  std::string head;
  in >> head;
  if (head != "OFF")
    throw "Does not start with OFF!";

  int nverts, nfaces, nedges;
  if (!(in >> nverts >> nfaces >> nedges))
    throw "Could not read number of vertices, faces, edges";

  std::vector<vec3d> vertices(nverts);
  for (int i=0; i<nverts; ++i) {
    in >> vertices[i].x >> vertices[i].y >> vertices[i].z;
  }

  std::vector<Segment> segments(nfaces);
  for (int i=0; i<nfaces; ++i) {
    int sz, a, b;
    in >> sz >> a >> b;
    segments[i] = Segment(vertices[a], vertices[b]);
  }
  std::clog << "Loaded: " << segments.size() << std::endl;

  return segments;
}

void drawQuadTex(const Point &p1, const Point &p2, const Point &p3, const Point &p4) {
  glColor4f(1.f, 0.f, 0.f, 1.0);
  glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex2f(p1.x, p1.y);
    glTexCoord2f(1.0, 0.0); glVertex2f(p2.x, p2.y);
    glTexCoord2f(1.0, 1.0); glVertex2f(p3.x, p3.y);
    glTexCoord2f(0.0, 1.0); glVertex2f(p4.x, p4.y);
  glEnd();
}

void drawBox() {
  glColor4f(0.0, 0.0, 0.0, 1.0);
  glBegin(GL_LINE_LOOP);
    glVertex2f(0.0, 0.0);
    glVertex2f(1.0, 0.0);
    glVertex2f(1.0, 1.0);
    glVertex2f(0.0, 1.0);
  glEnd();
}

// Draw a set of segments
void drawSegments(const std::vector<Segment> &segs, int color) {
  if (color) glColor4f(0.0, 0.0, 1.0, 1.0);
  else glColor4f(0.0, 0.0, 0.0, 1.0);
  glBegin(GL_LINES);
    for (unsigned i=0; i<segs.size(); ++i) {
      if (!segs[i].active) continue;
      glVertex2f(segs[i].a.x, segs[i].a.y);
      glVertex2f(segs[i].b.x, segs[i].b.y);
    }
  glEnd();
}

void drawSegmentBorder(const Segment &seg, int color) {
  if (color==0) glColor4f(1.0, 0.0, 0.0, 1.0);
  else glColor4f(0.0, 1.0, 0.0, 1.0);
  glLineWidth(3.0);
  glBegin(GL_LINES);
    glVertex2f(seg.a.x, seg.a.y);
    glVertex2f(seg.b.x, seg.b.y);
  glEnd();
  glLineWidth(1.0);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glBindTexture(GL_TEXTURE_2D, dc2d->textureId());
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  Segment from(Point(0.0, 0.0), Point(1.0, 0.0));
  Segment to(Point(0.0, 1.0), Point(1.0, 1.0));

  tic();
  dc2d->process(from, to, segments);
  toc("Computing Depth Complexity");

  int max = dc2d->maximum();
  printf("Max value is: %i\n", max);

  std::vector<Segment> rays;
  if (showRays) rays = dc2d->maximumRays();
  dc2d->copyStencilToColor();

  // Draw segments
  glPushMatrix();
    glTranslatef(40, 55, 0);
    glScalef(20, 20, 1);
    if (showRays) drawSegments(rays, 1);
    drawSegments(segments, 0);
    drawBox();
    drawSegmentBorder(from, 0);
    drawSegmentBorder(to, 1);
  glPopMatrix();

  // Draw dual space
  glPushMatrix();
    glTranslatef(40, 25, 0);
    glScalef(20, 20, 1);
    glEnable(GL_TEXTURE_2D);
      drawQuadTex(from.a, from.b, to.b, to.a);
    glDisable(GL_TEXTURE_2D);
  glPopMatrix();

  glutSwapBuffers();

}

void reshape(int w, int h) {
  //	float aspect = w/h;
  if(h==0) h = 1;
  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 100.0, 0.0, 100.0, 0.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
}

void init(void) {
  glClearColor(1.0, 1.0, 1.0, 1.0);

  // This function need to be called after 'glutCreateWindow'
  dc2d = new DepthComplexity2D(512, 512);
}

// keyboard
void keyboard( unsigned char key, int x, int y ) {
  switch (key) {
    case 'r':
      showRays = ! showRays;
      dc2d->setComputeMaximumRays(showRays);
      glutPostRedisplay(); break;
    case 27:
      exit(0);
  }
}

int main (int argc, char **argv) {
  cmd_usage("Programa para ejecutar Depth Complexity in 2D");
  const char *filename = cmd_option("-f", "models2d/segments1.off", "2D Segments in OFF format.");

  try {
    std::ifstream file(filename);
    segments = loadOFFSegments(file);
  } catch (const char* msg)  {
      std::cerr << "Failed: " << msg << std::endl;
      return 1;
  } catch (std::string msg) {
      std::cerr << "Failed: " << msg << std::endl;
      return 1;
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);

  glutInitWindowSize(800, 800);
  glutCreateWindow("Dual Ray Implementation");
  glewInit();

  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);

  glutMainLoop();
  return 0;
}
