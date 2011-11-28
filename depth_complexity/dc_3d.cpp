#include "dc_3d.h"
#include "dc_2d.h"
#include "util.h"
#include "timer.h"

#include <cassert>
#include <iostream>
#include <iomanip>

template<class T>
T mix(const T& a, const T& b, double x) {
  const T& p = (1-x)*a;
  const T& q = x*b;
  return p + q;
}

DepthComplexity3D::DepthComplexity3D(int fboWidth, int fboHeight, int discretSteps):
  _fboWidth(fboWidth),
  _fboHeight(fboHeight),
  _discretSteps(discretSteps),
  _maximum(0),
  _computeHistogram(false),
  _computeMaximumRays(false) {

  _dc2d = new DepthComplexity2D(_fboWidth, _fboHeight);
}

DepthComplexity3D::~DepthComplexity3D() {}

void DepthComplexity3D::setComputeHistogram(bool computeHistogram) {
  this->_computeHistogram = computeHistogram;
  assert(_dc2d != 0);
  _dc2d->setComputeHistogram(computeHistogram);
}

void DepthComplexity3D::setComputeMaximumRays(bool computeMaximumRays) {
  this->_computeMaximumRays = computeMaximumRays;
  assert(_dc2d != 0);
  _dc2d->setComputeMaximumRays(computeMaximumRays);
}

void DepthComplexity3D::writeHistogram(std::ostream& out) {
  out << "Intersections Frequency\n";
  for (unsigned i=0; i< _histogram.size(); ++i)
    out << std::left << std::setw(14) << i << _histogram[i] << "\n";
}

void DepthComplexity3D::writeRays(std::ostream& out) {
  out << (_maximumRays.size()*2) << " "
      << _maximumRays.size() << " 0\n";

  std::vector<Segment>::const_iterator ite = _maximumRays.begin();
  std::vector<Segment>::const_iterator end = _maximumRays.end();
  for (; ite != end; ++ite) {
    out << ite->a.x << " " << ite->a.y << " " << ite->a.z << "\n"
        << ite->b.x << " " << ite->b.y << " " << ite->b.z << "\n";
  }

  for(unsigned i=0; i<2*_maximumRays.size(); i+=2)
    out << "2 " << i << " " << (i+1) << "\n";
}

void DepthComplexity3D::process(const TriMesh &mesh) {
  this->_mesh = &mesh;

  _maximum = 0;
  processMeshAlign(AlignZ, AlignX);
  processMeshAlign(AlignZ, AlignY);

  processMeshAlign(AlignY, AlignX);
  processMeshAlign(AlignY, AlignZ);

  processMeshAlign(AlignX, AlignY);
  processMeshAlign(AlignX, AlignZ);
}

// Call this varying palign and salign.
void DepthComplexity3D::processMeshAlign(const PlaneAlign &palign, const PlaneAlign &salign) {
  assert(palign != salign);

  BoundingBox aabb = _mesh->aabb;
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


  // generate all planes varying on z
  const unsigned steps = _discretSteps;

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
      std::vector<Segment> segments;
      processMeshPlane(plane, &segments);

      // make the segments extra-long
      vec3d dsa = sa.b - sa.a; sa.a -= dsa; sa.b += dsa;
      vec3d dsb = sb.b - sb.a; sb.a -= dsb; sb.b += dsb;

      _dc2d->process(sa, sb, segments);

      int tempMaximum = _dc2d->maximum();
      if (tempMaximum >= _maximum) {
        if (tempMaximum > _maximum) {
          _maximumRays.clear();
          _histogram.resize(tempMaximum+1);
          _intersectionPoints.clear();
          //          _intersectionSegments.clear();
        }
        _maximum = tempMaximum;
        std::vector<Segment> tempRays = _dc2d->maximumRays();
        std::vector<int> tempHist = _dc2d->histogram();

        // Testing rays and saving intersectin points.
        for (unsigned r=0; r<tempRays.size(); ++r) {
          for (unsigned s=0; s<segments.size(); ++s) {
            double t1, t2;
            if(segmentIntersection3D(tempRays[r], segments[s], &t1, &t2)) {
              _intersectionPoints.push_back(tempRays[r].a + t1*(tempRays[r].b-tempRays[r].a));
            }
          }
        }

        _maximumRays.insert(_maximumRays.end(), tempRays.begin(), tempRays.end());
        for(unsigned i=0; i< tempHist.size(); ++i)
          _histogram[i] += tempHist[i];

//        _intersectionSegments.insert(_intersectionSegments.end(), segments.begin(), segments.end());
//        _intersectionPoints.insert(_intersectionPoints.end(), points.begin(), points.end());
      }
    }
  }
}

void DepthComplexity3D::processMeshPlane(const vec4d& plane, std::vector<Segment> *segments) {
  assert(segments);

  for (unsigned i=0; i<_mesh->faces.size(); ++i) {
    Segment s;
    if (intersectPlaneTriangle(plane, _mesh->faces[i], &s))
      segments->push_back(s);
  }
}

bool DepthComplexity3D::intersectPlaneTriangle(const vec4d& plane, const Triangle& tri, Segment *seg) {
  assert(seg);

  double da, db, dc;
  da = dot(plane, vec4d(tri.a, 1));
  db = dot(plane, vec4d(tri.b, 1));
  dc = dot(plane, vec4d(tri.c, 1));

  if (da == 0 && db == 0 && dc == 0)
    return false;

  vec3d *p = &seg->a;

  if (std::signbit(da) != std::signbit(db)) {
    intersectPlaneSegment(plane, tri.a, tri.b, p);
    p = &seg->b;
  }

  if (std::signbit(db) != std::signbit(dc)) {
    intersectPlaneSegment(plane, tri.b, tri.c, p);
    p = &seg->b;
  }

  if (std::signbit(dc) != std::signbit(da)) {
    intersectPlaneSegment(plane, tri.c, tri.a, p);
    p = &seg->b;
  }
  return p == &seg->b;
}

bool DepthComplexity3D::intersectPlaneSegment(const vec4d& plane, const vec3d& p0, const vec3d& p1, vec3d *pt) {
  double num = -plane.w - dot(plane.xyz(), p0);
  double den = dot(plane.xyz(), p1 - p0);
  double r = num / den;
  *pt = mix(p0, p1, r);
  if (0 >= r && r <= 1)
      return true;
  return false;
}

vec4d DepthComplexity3D::makePlane(const vec3d& a, const vec3d& b, const vec3d& c) {
    vec3d normal = cross(b-a, c-a);
    normal.normalize();
    double d = dot(a, normal);
    return vec4d(normal, -d);
}
