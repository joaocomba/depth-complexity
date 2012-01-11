//-- opengl util includes and defs
#ifndef _OPENGL_H
#define _OPENGL_H

//#include <GL/glew.h>
#include <iostream>
#include <cstdio>
	using namespace std;
	
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// ------------------------------------------------------------------------------
//-- using CG shaders
#ifdef USE_CG
	#include <Cg/cg.h>    /* Can't include this?  Is Cg Toolkit installed! */
	#include <Cg/cgGL.h>

	void drawRoundedRectangle( const float x, const float y, const float w, const float h, const GLclampf roundness );
#endif
// ------------------------------------------------------------------------------
//-- opengl error callback
#define checkGLErrors() \
	{ const GLenum errCode = glGetError(); \
	if ( errCode != GL_NO_ERROR) { \
		cerr << "[OpenGL ERROR] (" << errCode << ") " << __FILE__ \
			<< "(" << __LINE__ << ") : "<< gluErrorString( errCode ) << "\n"; } }
			
inline void checkGLError(const char *functionName, int adress = -1) {
	GLenum error;
	while (( error = glGetError() ) != GL_NO_ERROR) {
		fprintf (stderr, "[%s] GL error %s \n", functionName, gluErrorString(error));
		cerr<<"[ERROR]!\n";
		int x; cin>>x;
		exit(-1);
	}
	if( adress == 0 ){
		cerr<<"[ERROR] allocation GLundetected!\n";
		int x; cin>>x;
		exit(-1);
	}
}			
			
// ------------------------------------------------------------------------------

//nVidia built-in attributes indices
#define gl_Vertex_at				0
#define gl_Normal_at				2
#define gl_Color_at					3
#define gl_SecondaryColor_at	 	4
#define gl_FogCoord_at	 			5
#define gl_MultiTexCoord0_at	 	8
#define gl_MultiTexCoord1_at	 	9
#define gl_MultiTexCoord2_at	 	10
#define gl_MultiTexCoord3_at	 	11
#define gl_MultiTexCoord4_at	 	12
#define gl_MultiTexCoord5_at	 	13
#define gl_MultiTexCoord6_at	 	14
#define gl_MultiTexCoord7_at	 	15

#endif //-- OPENGL_H
