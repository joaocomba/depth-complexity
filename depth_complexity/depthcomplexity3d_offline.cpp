#include "flags.h"
#include "util.h"
#ifdef USE_RANDOM_DC3D
#include "dc_3d_random.h"
#else
#include "dc_3d.h"
#endif
#include "timer.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Usage:
//   All the commandline arguments have default values.
//  ./depthcomplexity3d_offline -h
//                              -f "models/suzanne.obj"
//                              -fboWidth 400
//                              -fboHeight 400
//                              -dsteps 10
//                              -fh histo.txt
//                              -fr rays.off

std::string getExtension(const std::string& filename);

int main (int argc, char **argv) {
  cmd_usage("Program to find depth complexity out (offline mode)");
  const char *filename = cmd_option("-f", "models/suzanne.obj", "Model in OBJ or OFF format.");
  const int fboWidth  = cmd_option("-fboWidth",  512, "Framebuffer width.");
  const int fboHeight = cmd_option("-fboHeight", 512, "Framebuffer height.");
  const int discretSteps = cmd_option("-dsteps", 10, "Discrete steps.");
  const char *filenameHistogram = cmd_option("-fh", "", "Save a *.txt file with histogram information");
  const char *filenameRays = cmd_option("-fr", "", "Save a *.off file with rays in ");
  const bool computeMoreRays = cmd_option("-cmr", false, "Whether rays above the threshold of intersections should be output");
  const int intersectionThreshold  = cmd_option("-it",  0, "Threshold of intersections");

  try {
    tic();
    std::ifstream file(filename);
    std::string ext = getExtension(filename);
    TriMesh mesh;
    if (ext == "off" || ext == "OFF")
      mesh = loadOFFMesh(file);
    else if (ext == "obj" || ext == "OBJ")
      mesh = loadOBJMesh(file);
    else
      throw "Unknown file type!";
    file.close();
    toc("Loading Mesh");

    // TODO(jpocom) Look for another way to call glewInit() and work in offscreen.
    glutInit(&argc, argv);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Dual Ray Implementation");
    glewInit();

	#ifdef USE_RANDOM_DC3D
    RDepthComplexity3D dc3d(fboWidth, fboHeight, discretSteps);
	#else
    DepthComplexity3D dc3d(fboWidth, fboHeight, discretSteps);
    #endif
    dc3d.setComputeHistogram(strcmp(filenameHistogram, "")!=0);
    dc3d.setComputeMaximumRays(strcmp(filenameRays, "")!=0);
    dc3d.setComputeGoodRays(computeMoreRays);
    dc3d.setThreshold(intersectionThreshold);

    tic();
    dc3d.process(mesh);
    toc("Computing Depth Complexity");

    std::cout << "Maximum: " << dc3d.maximum() << std::endl;

    // Saving Histogram file
    if (strcmp(filenameHistogram, "")!=0) {
      std::string extTxt = getExtension(filenameHistogram);
      if (extTxt == "txt" || extTxt == "TXT") {
        std::ofstream fileHistogram(filenameHistogram);
        dc3d.writeHistogram(fileHistogram);
        fileHistogram.close();
      } else throw "Histogram's file should be *.txt!";
    }

    // Saving Rays file
    if (strcmp(filenameRays, "")!=0) {
      std::string extOff = getExtension(filenameRays);
      if (extOff == "off" || extOff == "OFF") {
        std::ofstream fileRays(filenameRays);
        dc3d.writeRays(fileRays);
        fileRays.close();
      } else throw "Ray's file should be *.off!";
    }
   
    // Saving MoreRays file
    if(computeMoreRays) {
      unsigned numRays = 0;
      for(unsigned i = dc3d.getThreshold() ; i <= dc3d.maximum() ; ++i)
        numRays += dc3d.goodRays(i).size();
      std::cout << "Number of good rays: " << numRays << std::endl;
      for(unsigned i=intersectionThreshold;i<=dc3d.maximum();++i) {
        std::ostringstream filenameMoreRays;
        filenameMoreRays << "rays" << i << ".off";
        std::ofstream fileRays(filenameMoreRays.str().c_str());
        dc3d.writeRays(fileRays,dc3d.goodRays(i));
        fileRays.close();
      }
    }
      
  } catch (const char* msg)  {
    std::cerr << "Failed: " << msg << std::endl;
    return 1;
  } catch (std::string msg) {
    std::cerr << "Failed: " << msg << std::endl;
    return 1;
  }

  return 0;
}

std::string getExtension(const std::string& filename) {
  std::string::size_type dotpos = filename.rfind(".");
  if (dotpos != std::string::npos)
    return filename.substr(dotpos+1);
  return "";
}
