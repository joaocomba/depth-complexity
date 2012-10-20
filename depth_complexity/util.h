#ifndef UTIL_H_
#define UTIL_H_

#include <vector>

#include "vector.hpp"

struct Triangle {
    vec3d a, na,
        b, nb,
        c, nc;
};

typedef vec3d Point;

struct Segment {
  Segment(const Point &a_, const Point &b_):a(a_), b(b_), active(true) {}
  Segment() : active(true){}
  vec3d a, b;
  bool active;
};
bool operator<(const Segment &s1, const Segment &s2);

/*struct CuttingSegment: Segment {
  CuttingSegment() : intersect(0) {}
  CuttingSegment(const Segment &s, const int intersect_ = 0):intersect(intersect_) {}
  int intersect;
};*/

struct BoundingBox {
    vec3d min, max;
    bool empty;
    
    BoundingBox() : empty(true)
    {}
    
    BoundingBox& merge(const vec3d& p)
    {
        if (empty) {
            empty = false;
            min = max = p;
        } else {
            min.x = std::min(min.x, p.x);
            min.y = std::min(min.y, p.y);
            min.z = std::min(min.z, p.z);
            max.x = std::max(max.x, p.x);
            max.y = std::max(max.y, p.y);
            max.z = std::max(max.z, p.z);
        }
        return *this;
    }

    vec3d extents() const
    {
        if (empty)
            return vec3d(0,0,0);
        else
            return max - min;
    }

    vec3d center() const
    {
        return 0.5 * (min + max);
    }
};

struct TriMesh {
    std::vector<Triangle> faces;
    BoundingBox aabb;
};

TriMesh loadOFFMesh(std::istream& in);
TriMesh loadOBJMesh(std::istream& in);

struct Sphere {
	vec3d center;
	double radius;
};

// help functions
bool lineIntersection3D(const Segment &line1, const Segment &line2, double *t1, double *t2);
bool lineIntersection2D(const Segment &line1, const Segment &line2, double *t1, double *t2);
bool segmentIntersection2D(const Segment &seg1, const Segment &seg2, double *t1, double *t2);
bool segmentIntersection3D(const Segment &seg1, const Segment &seg2, double *t1, double *t2);
bool segmentSphereIntersection3D(const Segment &seg, const Sphere sph, vec3d & out0, vec3d & out1);
vec3d cartesianToSpherical(vec3d point);

#endif
