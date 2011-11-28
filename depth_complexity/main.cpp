#include <iostream>
#include <algorithm>
#include <istream>
#include <cmath>
#include <string>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <assert.h>
#include <fstream>
#include <map>
#include <boost/tokenizer.hpp>

#include <GL/glew.h>
#include <GL/glfw.h>
#include <AntTweakBar.h>

#include "vector.hpp"
#include "camera.hpp"
#include "util.h"
#include "dc_2d.h"

std::map<Segment, int> maximumRays;
std::vector<Segment> intersectionVectors;
std::vector<Point> intersectionPoints;
std::map<int, int> histogram;
float radius = 0.01;
vec4f sphereColor(0.0, 1.0, 0.0, 1.0);

enum PlaneAlign
{
    AlignZ,
    AlignY,
    AlignX
};



unsigned discretSteps = 10;
unsigned maximumDepth = 0;

template<class T>
T mix(const T& a, const T& b, double x)
{
    const T& p = (1-x)*a;
    const T& q = x*b;
    return p + q;
}

template<class T>
T clamp(const T& min, const T& x, const T& max)
{
    return x < min ? min : (x > max ? max : x);
}

// used to store in a map
bool operator<(const Segment &s1, const Segment &s2) 
{
  if (s1.a.x == s2.a.x)
    return s1.a.y < s2.a.y;
  return s1.a.x < s2.a.x;
}

bool intersectPlaneSegment(const vec4d& plane, const vec3d& p0, const vec3d& p1, vec3d& pt)
{
    double num = -plane.w - dot(plane.xyz(), p0);
    double den = dot(plane.xyz(), p1 - p0);
    double r = num / den;
    pt = mix(p0, p1, r);
    if (0 >= r && r <= 1)
        return true;
    return false;
}

bool intersectPlaneTriangle(const vec4d& plane, const Triangle& tri, Segment& seg)
{
    double da, db, dc;
    da = dot(plane, vec4d(tri.a, 1));
    db = dot(plane, vec4d(tri.b, 1));
    dc = dot(plane, vec4d(tri.c, 1));

    if (da == 0 && db == 0 && dc == 0)
        return false;

    vec3d *p = &seg.a;

    if (std::signbit(da) != std::signbit(db)) {
        intersectPlaneSegment(plane, tri.a, tri.b, *p);
        p = &seg.b;
    }
    
    if (std::signbit(db) != std::signbit(dc)) {
        intersectPlaneSegment(plane, tri.b, tri.c, *p);
        p = &seg.b;
    }
    
    if (std::signbit(dc) != std::signbit(da)) {
        intersectPlaneSegment(plane, tri.c, tri.a, *p);
        p = &seg.b;
    }
    return p == &seg.b;
}


TriMesh
loadOFFMesh(std::istream& in)
{
    std::clog << "Loading OFF file" << std::endl;

    std::string head;
    in >> head;
    if (head != "OFF")
         throw "Does not start with OFF!";

    int nverts, nfaces, nedges;

    if (!(in >> nverts >> nfaces >> nedges))
        throw "Could not read number of vertices, faces, edges";
    

    TriMesh mesh;
    mesh.faces.reserve(nfaces);

    std::vector<vec3d> vertices(nverts);
    for (int i=0; i<nverts; ++i) {
        in >> vertices[i].x >> vertices[i].y >> vertices[i].z;
        mesh.aabb.merge(vertices[i]); // build AABB
    }
    
    for (int i=0; i<nfaces; ++i) {
        int sz, a, b, c;
        Triangle t;
        in >> sz >> a >> b >> c;
        t.a = vertices.at(a);
        t.b = vertices.at(b);
        t.c = vertices.at(c);
        mesh.faces.push_back(t);
        
        for (int j=3; j<sz; ++j) {
            //std::clog << "triangulating face " << i << std::endl;
            
            b = c;
            in >> c;
            t.b = vertices.at(b);
            t.c = vertices.at(c);
            mesh.faces.push_back(t);
        }
    }

    std::clog << "Loaded " << mesh.faces.size() << " faces" << std::endl;

    // now build the normals
    for (unsigned i=0; i<mesh.faces.size(); ++i) {
        Triangle& t = mesh.faces[i];
        const vec3d ab = t.b - t.a;
        const vec3d ac = t.c - t.a;
        vec3d n = cross(ab, ac);
        n.normalize();
        t.na = t.nb = t.nc = n;
    }

    return mesh;
}


TriMesh
loadOBJMesh(std::istream& in)
{
    std::clog << "Loading OBJ file" << std::endl;

    std::string line;

    std::vector<vec3d> vertices;
    std::vector<vec3d> normals;
    
    TriMesh mesh;

    for(;;) {
        if (!getline(in, line))
            break;
        if (line == "")
            continue;

        switch (line[0]) {
            case 'v':
                if (line[1] == ' ') {
                    vec3d p;
                    if (sscanf(line.c_str()+2, "%lf %lf %lf", &p.x, &p.y, &p.z) != 3)
                        throw std::string("Error reading vertex at line " + line);
                    vertices.push_back(p);
                    mesh.aabb.merge(p);
                } else if (line[1] == 'n') {
                    vec3d n;
                    if (sscanf(line.c_str()+3, "%lf %lf %lf", &n.x, &n.y, &n.z) != 3)
                        throw std::string("Error reading normal at line " + line);
                    normals.push_back(n);
                }
                break;

            case 'f':
            {
                std::string content = line.substr(2);
                boost::char_separator<char> sep(" ");
                boost::tokenizer<boost::char_separator<char> > tok(content, sep);
                int a=-2, b=-2, c=-2, na=-2, nb=-2, nc=-2;
                for (boost::tokenizer<boost::char_separator<char> >::iterator i=tok.begin(); i!=tok.end(); ++i) {
                    int v=-1, n=-1;
                    if (sscanf(i->c_str(), "%d//%d", &v, &n) != 2
                        &&
                        sscanf(i->c_str(), "%d/%*d/%d", &v, &n) != 2
                        &&
                        sscanf(i->c_str(), "%d/%*d", &v) != 1 )
                        throw std::string("Error reading face at line " + line);

                    if (a == -2) {
                        // first vertex
                        a = v;
                        na = n;
                    } else if (b == -2) {
                        // second vertex
                        b = v;
                        nb = n;
                    } else {
                        // third or later vertices, formed a complete face
                        c = v;
                        nc = n;

                        Triangle t;
                        t.a = vertices.at(a-1);
                        t.b = vertices.at(b-1);
                        t.c = vertices.at(c-1);

                        // if we don't have a normal, make one
                        if (na <= 0) {
                            vec3d normal = cross(t.b - t.a, t.c - t.a);
                            normal.normalize();
                            std::clog << "created normal for face " << mesh.faces.size() << std::endl;
                            normals.push_back(normal);
                            na = nb = nc = normals.size();
                        }
                        t.na = normals.at(na-1);
                        t.nb = normals.at(nb-1);
                        t.nc = normals.at(nc-1);
                        mesh.faces.push_back(t);

                        b = c;
                        nb = nc;
                    }
                }
                break;
            }
            default: {
                std::clog << "ignored line: " << line << std::endl;
            }
        }
    }

    std::clog << "Loaded " << mesh.faces.size() << " faces" << std::endl;

    return mesh;
}


vec4d makePlane(const vec3d& a, const vec3d& b, const vec3d& c)
{
    vec3d normal = cross(b-a, c-a);
    normal.normalize();
    double d = dot(a, normal);
    return vec4d(normal, -d);
}


void
processMeshPlane(const TriMesh& mesh,
                 const vec4d& plane,
                 std::vector<Segment>& segments)
{
    
    for (unsigned i=0; i<mesh.faces.size(); ++i) {
        Segment s;
        if (intersectPlaneTriangle(plane, mesh.faces[i], s))
            segments.push_back(s);
    }
}



/* Call this varying palign and salign.
 */
void
processMeshAlign(const TriMesh& mesh, PlaneAlign palign, PlaneAlign salign, int& maximum)
{
    assert(palign != salign);

    BoundingBox aabb = mesh.aabb;
    aabb.merge(aabb.min - aabb.extents()/10.0);
    aabb.merge(aabb.max + aabb.extents()/10.0);

    vec3d c0 = vec3d(aabb.min.x, aabb.min.y, aabb.min.z);
    vec3d c1 = vec3d(aabb.max.x, aabb.min.y, aabb.min.z);
    vec3d c2 = vec3d(aabb.min.x, aabb.max.y, aabb.min.z);
    vec3d c3 = vec3d(aabb.max.x, aabb.max.y, aabb.min.z);
    vec3d c4 = vec3d(aabb.min.x, aabb.min.y, aabb.max.z);
    vec3d c5 = vec3d(aabb.max.x, aabb.min.y, aabb.max.z);
    vec3d c6 = vec3d(aabb.min.x, aabb.max.y, aabb.max.z);
    vec3d c7 = vec3d(aabb.max.x, aabb.max.y, aabb.max.z);

    std::vector<Segment> segments;
    std::vector<Segment> rays;
    int tempMaximum;
    
    // generate all planes varying on z
    const unsigned steps = discretSteps;
    
    for (unsigned az = 0; az<steps; ++az) {
        // double t = 3*az / (steps - 1.0) - 1; // [-1, 2]
        double t = az / (steps - 1.0);
        for (unsigned bz = 0; bz<steps; ++bz) {
            // double u = 3*bz / (steps - 1.0) - 1; // [-1, 2]
            double u = bz / (steps - 1.0);
            
            Segment sa, sb;

            switch (palign) {
                case AlignZ: {
                    Point a = mix(c0, c4, t); // along Z
                    Point b = mix(c7, c3, u); // along Z
                    sa.a = sa.b = a;
                    sb.a = sb.b = b;

                    if (salign == AlignX) {
                        // extend along X
                        sa.b.x = aabb.max.x;
                        sb.a.x = aabb.min.x;
                    } else {
                        // extend along Y
                        sa.b.y = aabb.max.y;
                        sb.a.y = aabb.min.y;
                    }
                    break;
                }
                case AlignY: {
                    Point a = mix(c0, c2, t); // along Y
                    Point b = mix(c7, c5, u); // along Y
                    sa.a = sa.b = a;
                    sb.a = sb.b = b;

                    if (salign == AlignX) {
                        // extend along X
                        sa.b.x = aabb.max.x;
                        sb.a.x = aabb.min.x;
                    } else {
                        // extend along Z
                        sa.b.z = aabb.max.z;
                        sb.a.z = aabb.min.z;
                    }
                    break;
                }
                case AlignX:  {
                    Point a = mix(c0, c1, t);
                    Point b = mix(c7, c6, u);
                    sa.a = sa.b = a;
                    sb.a = sb.b = b;

                    if (salign == AlignY) {
                        // extend along Y
                        sa.b.y = aabb.max.y;
                        sb.a.y = aabb.min.y;
                    } else {
                        // extend along Z
                        sa.b.z = aabb.max.z;
                        sb.a.z = aabb.min.z;
                    }
                    break;
                }
            }

            vec4d plane = makePlane(sa.a, sa.b, sb.a);
            processMeshPlane(mesh, plane, segments);

            // make the segments extra-long
            vec3d dsa = sa.b - sa.a;
            sa.a -= dsa;
            sa.b += dsa;
            vec3d dsb = sb.b - sb.a;
            sb.a -= dsb;
            sb.b += dsb;

            tempMaximum = findDepthComplexity2DPartial(sa, sb, segments);
            if (tempMaximum >= maximum) {
                if (tempMaximum > maximum) {
                    maximumRays.clear();
                    intersectionVectors.clear();
                    intersectionPoints.clear();
                }
                maximum = tempMaximum;
                getRaysWithMaxIntersectionPartial(sa, sb, tempMaximum, &rays, &histogram);
                std::vector<Point> points;
                unsigned countIntersection;
                for (unsigned r=0; r<rays.size(); ++r) {
                  countIntersection = 0;
                  for (unsigned s=0; s<segments.size(); ++s) {
                    double t1, t2;
                    if(segmentIntersection3D(rays[r], segments[s], &t1, &t2)) {
                      points.push_back(rays[r].a + t1*(rays[r].b-rays[r].a));
                      countIntersection++;
                    }
                  }
                  // saving maximum rays and intersection's number
                  maximumRays[rays[r]] = countIntersection;
                }
                // maximumRays.insert(maximumRays.end(), rays.begin(), rays.end());
                intersectionVectors.insert(intersectionVectors.end(), segments.begin(), segments.end());
                intersectionPoints.insert(intersectionPoints.end(), points.begin(), points.end());
            }
            segments.clear();
            rays.clear();
        }
    }
}

void drawPlane(const vec4d& plane)
{
    const vec3d& n = plane.xyz();
    
    // axis = axis of min rotation from z to n
    const vec3d& axis = cross(vec3d(0,0,1), n);
    
    double degrees = std::asin(clamp(-1.0, axis.length(), 1.0)) * 180 / M_PI;
    
    glPushMatrix();
      glRotatef(degrees, axis.x, axis.y, axis.z);
      glTranslatef(0, 0, -plane.w);
    
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

      glBegin(GL_QUADS);
        glColor4f(0, .2, .5, 0.3);
        glTexCoord2f(0.0, 0.0); glVertex2f(-2, -2);
        glTexCoord2f(1.0, 0.0); glVertex2f(2, -2);
        glTexCoord2f(1.0, 1.0); glVertex2f(2, 2);
        glTexCoord2f(0.0, 1.0); glVertex2f(-2, 2);
      glEnd();
    glPopMatrix();
    glDisable(GL_BLEND);
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



static int winWidth, winHeight;


// Callback function called by GLFW when window size changes
void GLFWCALL WindowSizeCB(int width, int height)
{
    winWidth = width;
    winHeight = height;
    // Send the new window size to AntTweakBar
    TwWindowSize(width, height);
}

vec3d triCenter(const Triangle& t)
{
    return (t.a + t.b + t.c) / 3.0;
}

template<class T>
vec3<T> sorted(const T& a, const T& b, const T& c)
{
    vec3<T> r(a, b, c);
    if (r.y < r.x)
        std::swap(r.x, r.y);
    if (r.z < r.x)
        std::swap(r.x, r.z);
    if (r.z < r.y)
        std::swap(r.y, r.z);
    return r;
}


struct ByDist {
    const vec3d dir;
    ByDist(const vec3f& d) : dir(d) {}
    
    bool operator()(const Triangle& t1, const Triangle& t2) const
    {
#if 1
        vec3d c1 = triCenter(t1);
        vec3d c2 = triCenter(t2);
        return dot(c1, dir) > dot(c2, dir);
#else
        double da1 = dot(dir, t1.a);
        double db1 = dot(dir, t1.b);
        double dc1 = dot(dir, t1.c);

        double da2 = dot(dir, t2.a);
        double db2 = dot(dir, t2.b);
        double dc2 = dot(dir, t2.c);

        vec3d s1 = sorted(da1, db1, dc1);
        vec3d s2 = sorted(da2, db2, dc2);

        if (s1.x > s2.x)
            return true;
        if (s1.x < s2.x)
            return false;
        if (s1.y > s2.y)
            return true;
        if (s1.y < s2.y)
            return false;
        return s1.z > s2.z;
#endif
    }
};


std::vector<Triangle> sorted_faces;

void drawMesh(const TriMesh& mesh, const vec3f& dir)
{
    //std::clog << "sorting...";
    if (sorted_faces.empty()) {
        sorted_faces = mesh.faces;
    }
    std::sort(sorted_faces.begin(), sorted_faces.end(), ByDist(dir));
    //std::clog << "done" << std::endl;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //glDisable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    glEnable(GL_VERTEX_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_DOUBLE, 2*sizeof(vec3d), &sorted_faces[0].a.x);
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_DOUBLE, 2*sizeof(vec3d), &sorted_faces[0].na.x);

    glDrawArrays(GL_TRIANGLES, 0, sorted_faces.size()*3);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisable(GL_NORMAL_ARRAY);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}


void drawPlaneIntersection(const std::vector<Segment>& segments)
{
    // draw the plane-mesh intersection
    glDisable(GL_LIGHTING);
    glLineWidth(2);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    for (unsigned i=0; i<intersectionVectors.size(); ++i) {
        const Segment & s = intersectionVectors[i];
        glVertex3f(s.a.x, s.a.y, s.a.z);
        glVertex3f(s.b.x, s.b.y, s.b.z);
    }
    glEnd();
}

void drawRays(const std::map<Segment, int>& rays)
{
    // // draw rays
    glLineWidth(1);
    glBegin(GL_LINES);
    glColor3f(0.5, 0.0, 0.5);
      for (std::map<Segment, int>::const_iterator it = rays.begin(); it != rays.end(); ++it) {
        const Segment & r = it->first;
        glVertex3f(r.a.x, r.a.y, r.a.z);
        glVertex3f(r.b.x, r.b.y, r.b.z);
      }
    glEnd();

    glEnable(GL_LIGHTING);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &sphereColor.x);
    for (unsigned i=0; i<intersectionPoints.size(); ++i)  {
      const Point p = intersectionPoints[i];
      glPushMatrix();
        glTranslatef(p.x, p.y, p.z);
        glutSolidSphere(radius, 10, 10);
      glPopMatrix();
    }
}



void setupCamera(Camera& cam)
{
    glViewport(0, 0, winWidth, winHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50, (double)winWidth/winHeight, 0.1, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    cam.applyTransform();
}


void recompute(void *data)
{
    const TriMesh* mesh = reinterpret_cast<const TriMesh*>(data);
    
    int maximum = 0;

    processMeshAlign(*mesh, AlignZ, AlignX, maximum);
    processMeshAlign(*mesh, AlignZ, AlignY, maximum);

    processMeshAlign(*mesh, AlignY, AlignX, maximum);
    processMeshAlign(*mesh, AlignY, AlignZ, maximum);

    processMeshAlign(*mesh, AlignX, AlignY, maximum);
    processMeshAlign(*mesh, AlignX, AlignZ, maximum);

    maximumDepth = maximum;
    
    // printing intersection number for each ray 
    unsigned count = 0;
    for (std::map<Segment, int>::const_iterator it = maximumRays.begin(); it != maximumRays.end(); ++it) {
      std::cout << "Ray [" << count++ <<"] has "
                << it->second << " intersection\n";
    }

    // printing histogram
    for (std::map<int, int>::const_iterator it = histogram.begin(); it != histogram.end(); ++it) {
      std::cout << it->first << " intersection => ";
      std::cout << it->second << " rays\n";
    }

#if 0
    std::ofstream out("./segments.off");

    out << "OFF\n";
    out << (intersectionVectors.size()*2) << " " 
        << intersectionVectors.size() << " 0\n";
    for(unsigned i=0; i<intersectionVectors.size(); ++i) {
      out << intersectionVectors[i].a.x << " " << intersectionVectors[i].a.y << " " << intersectionVectors[i].a.z << "\n"
          << intersectionVectors[i].b.x << " " << intersectionVectors[i].b.y << " " << intersectionVectors[i].b.z << "\n";
    }
    for(unsigned i=0; i<2*intersectionVectors.size(); i+=2)
      out << "2 " << i << " " << (i+1) << "\n";
    
    out.close();

    out.open("./rays.off");
    out << (maximumRays.size()*2) << " " 
        << maximumRays.size() << " 0\n";
    for(unsigned i=0; i<maximumRays.size(); ++i) {
      out << maximumRays[i].a.x << " " << maximumRays[i].a.y << " " << maximumRays[i].a.z << "\n"
          << maximumRays[i].b.x << " " << maximumRays[i].b.y << " " << maximumRays[i].b.z << "\n";
    }
    for(unsigned i=0; i<2*maximumRays.size(); i+=2)
      out << "2 " << i << " " << (i+1) << "\n";
    out.close();
#endif

    std::clog << "Maximum: " << maximum << std::endl;
}


void
drawBackground(const vec3f& top, const vec3f& mid, const vec3f& bot)
{
    glDisable(GL_DEPTH_TEST);
    //glDepthMask(GL_FALSE);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glBegin(GL_QUAD_STRIP);
    glColor3fv(&bot.x);
    glVertex2f(-1, -1);
    glVertex2f( 1, -1);
    glColor3fv(&mid.x);
    glVertex2f(-1, 0);
    glVertex2f( 1, 0);
    glColor3fv(&top.x);
    glVertex2f(-1, 1);
    glVertex2f( 1, 1);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}




int doInteractive(const TriMesh& mesh)
{
    glfwInit();
    std::atexit(glfwTerminate);
    
    GLFWvidmode mode;
    
    glfwGetDesktopMode(&mode);
    if( !glfwOpenWindow(1024, 768, mode.RedBits, mode.GreenBits, mode.BlueBits, 
                        0, 16, 0, GLFW_WINDOW) )
    {
        std::cerr << "failed to open window!" << std::endl;
        return -1;
    }
    
    glewInit();

    glfwEnable(GLFW_MOUSE_CURSOR);
    glfwEnable(GLFW_KEY_REPEAT);
    glfwSetWindowTitle("Plane-Triangle intersection test");

    // Initialize AntTweakBar
    if( !TwInit(TW_OPENGL, NULL) )
    {
        std::cerr << "AntTweakBar initialization failed: " << TwGetLastError() << std::endl;
        return 1;
    }
    // Set GLFW event callbacks
    glfwSetWindowSizeCallback(WindowSizeCB);

    glfwSetMouseButtonCallback((GLFWmousebuttonfun)TwEventMouseButtonGLFW);
    glfwSetMousePosCallback((GLFWmouseposfun)TwEventMousePosGLFW);
    glfwSetMouseWheelCallback((GLFWmousewheelfun)TwEventMouseWheelGLFW);
    glfwSetKeyCallback((GLFWkeyfun)TwEventKeyGLFW);
    glfwSetCharCallback((GLFWcharfun)TwEventCharGLFW);

    // Init framebuffers to compute DC in 2D.
    initFBO(512, 512);


    TwBar *bar = TwNewBar("Controls");
    TwDefine(" GLOBAL ");

    bool rotate = false;
    vec3f top(0.25, 0.25, .5), mid(0.75, 0.75, .85), bot(1, 1, 1);

    vec4f objdiff(0.55, 0.5, 0, 0.5), objspec(.75, .75, .75, .2);
    GLfloat shine = 50;
    bool showObj = true;
    
    TwAddVarRW(bar, "rotate", TW_TYPE_BOOLCPP, &rotate, " ");

    TwAddVarRW(bar, "discretSteps", TW_TYPE_UINT32, &discretSteps, " label='discret. steps' min=2 ");

    TwAddButton(bar, "recompute", recompute, (void*)&mesh, " label='Recompute' ");

    TwAddVarRO(bar, "maxDepth", TW_TYPE_UINT32, &maximumDepth, " label='Max. depth' ");

    TwAddVarRW(bar, "top", TW_TYPE_COLOR3F, &top.x, " group='background' ");
    TwAddVarRW(bar, "mid", TW_TYPE_COLOR3F, &mid.x, " group='background' ");
    TwAddVarRW(bar, "bot", TW_TYPE_COLOR3F, &bot.x, " group='background' ");

    TwAddVarRW(bar, "Diff", TW_TYPE_COLOR4F, &objdiff.x, " group='Object' ");
    TwAddVarRW(bar, "Spec", TW_TYPE_COLOR4F, &objspec.x, " group='Object' ");
    TwAddVarRW(bar, "Shin", TW_TYPE_FLOAT, &shine, " group='Object' min='1' max='128' ");
    TwAddVarRW(bar, "Show", TW_TYPE_BOOLCPP, &showObj, " group='Object' ");

    TwAddVarRW(bar, "radius", TW_TYPE_FLOAT, &radius, "  group='Sphere' label='radius' min=0.0 step=0.001 max=2.0");
    TwAddVarRW(bar, "Color", TW_TYPE_COLOR4F, &sphereColor.x, " group='Sphere' ");

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    Camera cam;
    BoundingBox aabb = mesh.aabb;
    cam.target = aabb.center();
    cam.up = vec3f(0, 1, 0);
    cam.pos = cam.target + vec3f(0, 0, 2*aabb.extents().z);


    while( glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC) ) {
        glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );

        drawBackground(top, mid, bot);

        if (rotate)
            cam.relMove(vec3f(.04, 0, 0));

        setupCamera(cam);

        GLfloat lpos[4] = { cam.pos.x, cam.pos.y, cam.pos.z, 1 };
        glLightfv(GL_LIGHT0, GL_POSITION, lpos);

        drawPlaneIntersection(intersectionVectors);
        drawRays(maximumRays);

        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &objdiff.x);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &objspec.x);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
        if (showObj) drawMesh(mesh, cam.getDir());

        // Draw tweak bars
        TwDraw();

        // Present frame buffer
        glfwSwapBuffers();
    }

    return 0;
}


std::string getExtension(const std::string& filename)
{
    std::string::size_type dotpos = filename.rfind(".");
    if (dotpos != std::string::npos)
        return filename.substr(dotpos+1);
    return "";
}


int main(int argc, char **argv)
{
    if (argc == 1) {
        std::cerr << "missing input file!" << std::endl;
        return 1;
    }

    try {
        std::ifstream file(argv[1]);
        std::string ext = getExtension(argv[1]);
 
        TriMesh mesh;

       if (ext == "off" || ext == "OFF")
            mesh = loadOFFMesh(file);
        else if (ext == "obj" || ext == "OBJ")
            mesh = loadOBJMesh(file);
        else
            throw "Unknown file type!";
        
        if (doInteractive(mesh))
            return 1;

    }
    catch (const char* msg)  {
        std::cerr << "Failed: " << msg << std::endl;
        return 1;
    }
    catch (std::string msg) {
        std::cerr << "Failed: " << msg << std::endl;
    }
    
}
