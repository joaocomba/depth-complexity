//-----------------------------------------------------------------------------
// classe para manipulação de uma câmera sintética
//-----------------------------------------------------------------------------

#ifndef CAMERA_H
#define CAMERA_H

#include "float3.h"
#include "quaternion.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>
	using namespace std;

//#define printme( e ) { cerr << endl << #e << " = " << ( e ) << "\n"; } 
#define SM_SIZE 2048
#define DIF 200

//--------------------------------------------------
struct Keyframe{
	float3 e, a, u;
};

struct BSphere{
	float r;
	float3 c;
};

inline ostream &operator<<( std::ostream &out, const Keyframe f ) {
	return	out <<	f.e.x << " " << f.e.y << " " <<  f.e.z <<endl <<
					f.a.x << " " << f.a.y << " " <<  f.a.z <<endl<<
					f.u.x << " " << f.u.y << " " <<  f.u.z <<endl;
} // end oeprator
inline istream &operator>>( std::istream &in, Keyframe &f ) {
	return	in >>	f.e.x >> f.e.y >>  f.e.z >> 
					f.a.x >> f.a.y >>  f.a.z >> 
					f.u.x >> f.u.y >>  f.u.z ;
} // end oeprator
//-----------------------------------------------------------------------------
class Camera {
	private:
		float d, D, t;//distance interval, current interval, 'time'
		float3 dst, r, s;
		size_t idx;

		
	protected:
		// posição da câmera
		float3 afEye;
		// ponto que a câmera está olhando
		float3 afAt;
		// vetor apontando para parte de cima da câmera
		float3 afUp;
		
		bool setado;		
		float alpha, tgHalpha, aspect;		
		float znear, zfar;
		
		float3 mn, mx;
		BSphere bs;
		int maxDPC; //max depth complexity

		//sets of bases to save path
		vector<Keyframe> sets;
		
		
		
	public:
		
		// construtor
		Camera();
		// destrutor
		~Camera();

		void MoveFrente(float fDistancia);
		void MoveLado(float fDistancia);
		void MoveCimaBaixo(float fValor);	
		
		void MoveFrenteObj(float fDistancia);
		void MoveLadoObj(float fDistancia);
		void MoveCimaBaixoObj(float fValor);
		void lookLefRigObj(float fAnguloEmRadianos);
		void lookUpDownObj(float fValor);	
		
		void rotInN(float alpharad);	
		
		void setNear(float x){znear = x; update();}
		void setFar(float x){zfar = x; update();}
		void setAlpha(float a){alpha = a;}
		
		void lookLefRig(float fAnguloEmRadianos);
		void lookUpDown(float fValor);
		float3 GetEye()const{ return afEye; }
		float3 GetAt() const{ return afAt; }
		float3 GetUp() const{ return afUp; }
		float3 GetDir() const{ return (afAt-afEye).normalize(); }

		void print();//comand line only
		void lookAt()const; // gluLookAt	
		void reshape(int w, int h); // perspective	
		void setupText();
		void setupThing();
		void setupMVP();//setyup modelviewproj
		void update();
		void getModelView(float* v);
		void getView(float* v);
		void traceEye(float3 &v, float w);	
		
		void updateModelView();
		void setPerspec(float fov, float aspect, float n, float f);
		
		void adjustUp();
		
		void front();
		void up();
		void perspective();
		void top(float3 pos);
		
		void bbox(float3 mn, float3 mx, bool review);
		
		void drawAt();
		void drawBBcenter();
		
		void calcFrustum();	
		void updateFrustum();
		void setFrustum();	
		
		//sm funcs
		void lnnPerspective()const;
		void lnnLookAt()const;
		void frustu(double ang, double asp);
		
		//phan funcs
		void transf(double* m);
		void setEye(float a, float b, float c);
		
		//phong func
		float* getEye4f(float* e)const;
		void reset();

		//functions to save and load camera bases (configurations)
		void push();
		void save(string filename="camera.txt");
		void load(string filename);

		void setupInterv();
		void interpolateSets();
		void speed(int d);
		
		int getMaxDPC(){return maxDPC;}
};

#endif // CAMERA_H
