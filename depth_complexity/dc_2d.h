#ifndef DC_2D2_H
#define DC_2D2_H

#define GL_GLEXT_PROTOTYPES

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <algorithm>
#endif

#include "util.h"
#include <map>

// Compute the Depth complexity in 2D
//
// Usage:
//   int fboWidth = ..., fboHeight = ...;
//   DepthComplexity2D dc2d(fboWidth, fboHeight);
//
//   Segment from = ...;
//   Segment to = ...;
//   vector<Segment> segments = ...;
//   dc2d.process(from, to, segments);
//
class DepthComplexity2D {
public:
  DepthComplexity2D(const int fboWidth, const int fboHeight);

  void setComputeHistogram(bool computeHistogram) { this->_computeHistogram = computeHistogram; }
  void setComputeMaximumRays(bool computeMaximumRays) {this->_computeMaximumRays = computeMaximumRays; }
  void setComputeGoodRays(bool computeGoodRays) {this->_computeGoodRays = computeGoodRays; }
  void setThreshold(unsigned threshold) {this->_threshold = threshold; }

  // Compute the maximum depth complexity
  void process(
    const Segment &from, const Segment &to, const std::vector<Segment> &segments);

  // Copy stencil buffer to color buffer.
  // Colors are defined in a table.
  void copyStencilToColor();

  // Methods to obtain outputs
  unsigned                         maximum() const           { return _maximum; }
  std::vector<unsigned long long>  histogram()               { return _histogram; }
  std::vector<Segment>             maximumRays()             { return _maximumRays; }
  std::vector<Segment>             goodRays(unsigned i)      { return _goodRays[i]; }
  GLuint                           textureId() const         { return _textureId; }

private:
  // Create framebuffer objects
  bool initFBO();

  // Primal: point -> Dual: Segment
  Segment computeDualSegmentFromPoint(const Point &p);

  // Go through the framebuffer to find the maximum value.
  int findMaxValueInStencil();

  // Compute depth complexity using rays from one segment to the other.
  void findDepthComplexity2D();

  //
  void findMaximumRaysAndHistogram();

private:
  GLuint                                _textureId;
  GLuint                                _fboId;
  GLuint                                _rboId;

  // State
  bool                                  _status;
  bool                                  _computeHistogram;
  bool                                  _computeMaximumRays;
  bool                                  _computeGoodRays;

  // Inputs
  int                                   _fboWidth;
  int                                   _fboHeight;
  Segment                               _from;
  Segment                               _to;
  const std::vector<Segment>*           _segments;
  unsigned                              _threshold;

  // Outputs
  unsigned                              _maximum;
  std::vector<unsigned long long>       _histogram;
  std::vector<Segment>                  _maximumRays;
  std::vector< std::vector<Segment> >   _goodRays;
};

#endif // DC_2D2_H
