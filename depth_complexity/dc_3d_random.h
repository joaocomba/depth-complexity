#ifndef DC_3D_RANDOM_H
#define DC_3D_RANDOM_H

#include "util.h"

#include <map>
#include <vector>

// Usage:
//   TriMesh mesh = ...;
//   int discretSteps = 10;
//   int fboWidth = 400;
//   int fboHeight = 400;
//   DepthComplexity3D cd3d(mesh, fboWidth, fboHeight, discretSteps);  // Computation is done.
//   int maximum = dc3d.maximum();
//   int minimum = dc3d.minimum();
//   c3d.writeHistogram(std::cout);
//   c3d.writeRays(std::cout);
class RDepthComplexity3D {
public:
  RDepthComplexity3D(int fboWidth, int fboHeight, int discretSteps);
  virtual ~RDepthComplexity3D();

  void process(const TriMesh &mesh);

  void setComputeHistogram(bool computeHistogram);
  void setComputeMaximumRays(bool computeMaximumRays);
  void setComputeGoodRays(bool computeGoodRays);
  void setThreshold(unsigned threshold);

  unsigned maximum() const { return _maximum; }
  const std::vector<Segment> &maximumRays() const { return _maximumRays; }
  const std::vector<Segment> &goodRays(int intersect) const { return _goodRays[intersect]; }
  const std::vector<Segment> &usedPlanes() const { return _usedPlanes; }
  const std::vector<Point> &intersectionPoints() const { return _intersectionPoints; }
  unsigned getThreshold() { return _threshold; }

  void writeHistogram(std::ostream& out);
  void writeRays(std::ostream& out);
  void writeRays(std::ostream& out, const std::vector<Segment> & _rays);
  void writeRaysSpherical(std::ostream& out, int k);

private:
  enum PlaneAlign{
      AlignZ, AlignY, AlignX
  };

  void processMeshSegment(const Segment& segment, std::vector<Point> *points);
  bool intersectTriangleSegment(const Segment& segment, const Triangle& tri, Point *pnt);
  bool intersectPlaneSegment(const vec4d& plane, const vec3d& p0, const vec3d& p1, vec3d *pt);
  vec4d makePlane(const vec3d& a, const vec3d& b, const vec3d& c);

private:
  // Input
  const TriMesh *_mesh;
  int _fboWidth;
  int _fboHeight;
  int _discretSteps;
  unsigned _maximum;
  unsigned _threshold;

  // State
  bool _computeHistogram;
  bool _computeMaximumRays;
  bool _computeGoodRays;

  // Output
  std::vector<Segment> _maximumRays;
  std::vector< std::vector<Segment> > _goodRays;
  std::vector<Segment> _usedPlanes;
  std::vector<unsigned long long> _histogram;
  std::vector<Point> _intersectionPoints;
  //  std::vector<Segment> _intersectionSegments;

  friend int doInteractive(const TriMesh& mesh);
  friend void drawRays();
};


#endif // DC_3D_RANDOM_H
