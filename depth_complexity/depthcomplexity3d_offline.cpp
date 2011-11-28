#include "flags.h"
#include "util.h"
#include "dc_3d.h"
#include "timer.h"

#include <iostream>
#include <fstream>
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
  cmd_usage("Programa para ejecutar dc in offline mode");
  const char *filename = cmd_option("-f", "models/suzanne.obj", "Model in OBJ or OFF format.");
  const int fboWidth  = cmd_option("-fboWidth",  512, "Framebuffer width.");
  const int fboHeight = cmd_option("-fboHeight", 512, "Framebuffer height.");
  const int discretSteps = cmd_option("-dsteps", 10, "Discrete steps.");
  const char *filenameHistogram = cmd_option("-fh", "", "Save a *.txt file with histogram information");
  const char *filenameRays = cmd_option("-fr", "", "Save a *.off file with rays in ");

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

    DepthComplexity3D dc3d(fboWidth, fboHeight, discretSteps);
    dc3d.setComputeHistogram(strcmp(filenameHistogram, "")!=0);
    dc3d.setComputeMaximumRays(strcmp(filenameRays, "")!=0);

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
