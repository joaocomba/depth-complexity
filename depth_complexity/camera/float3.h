//-----------------------------------------------------------------------------
//           Name: float3.h
//         Author: Kevin Harris
//  Last Modified: 10/09br by Lenna
//-----------------------------------------------------------------------------

#ifndef _FLOAT3_H_
#define _FLOAT3_H_

#include <math.h>
#include <iostream>
#include "opengl.h"
	using namespace std;

#define printme(e){	cerr<<endl<<#e<<" = "<<(e)<<endl;}	//int x; cin>>x;}

enum{
	ZERO	= 2,
	ONE		= 3,
	TWO		= 5,
	THREE	= 7,
	FOUR	= 11,
	FIVE	= 13,
	SIX		= 17,
	SEVEN	= 19	
};

union float3{

		struct { float r; float g; float b;};
		struct { float x; float y; float z;};

    float3() { }
    float3(float x_, float y_, float z_);
    float3(const float scalar ) : x( scalar ), y( scalar ), z( scalar ) {}
    float3(const float3& v): x( v.x ), y( v.y ), z( v.z ) {}
    float3(const float* v): x( v[0] ), y( v[1]), z( v[2] ) {}
    
    void set(float x_, float y_, float z_);
    void set(float* v);
    float length(void);
    float3 normalize(void);
	float3 znormal();

    // Static utility methods
    static float distance(const float3 &v1, const float3 &v2);
    static float dotProduct(const float3 &v1,  const float3 &v2 );
    float dot( const float3 &v2 )const;
    static float3 crossProduct(const float3 &v1, const float3 &v2);
    
    // Operators...
    float3 operator + (const float3 &other)const;
    float3 operator - (const float3 &other)const;
    float3 operator * (const float3 &other)const;
    float3 operator / (const float3 &other)const;

    float3 operator * (const float scalar);
    friend float3 operator * (const float scalar, const float3 &other);
    
    float3& operator = (const float3 &other);

    float3 operator + () const;
    float3 operator - () const;

	bool operator > (const float f) { return x > f && y >f && z > f; }
    
//________________________________________________________________LNN MDF

 	float3 operator = ( const float right ) {
		r = g = b = right;
		return *this;
	} // end operator
	
	float3 operator += (const float3 &other){
		r += other.r;
		g += other.g;
		b += other.b;
		return *this;
	}
	float3 operator -= (const float3 &other){
		r -= other.r;
		g -= other.g;
		b -= other.b;
		return *this;
	}
	
	float3 operator / (const float r);
	
    void rotateY(float fAnguloEmRadianos);
    void glVertex()const{glVertex3f(x, y, z);}
    void glVertexN(float3 n)const {glNormal3f(n.x, n.y, n.z); glVertex3f(x, y, z);}
	void glNormal()const {glNormal3f(x, y, z);}
    void glTranslate()const {glTranslatef(x, y, z);}
    void glVectors()const {glVertex3f(0, 0, 0); glVertex3f(x, y, z);}
    void glColor(float a=1.0f)const {glColor4f(r, g, b, a);}

	float3 inv(){ return float3(x, y, -z);}
    
    float3 abs(){ return float3(fabs(x), fabs(y), fabs(z)); }
	
	float3  maxi(const float3 r){
		float3 result;
		result.x = max(x, r.x); result.y = max(y, r.y); result.z = max(z, r.z);
		return result;
	}
	float3  mini(const float3 r){
		float3 result;
		result.x = min(x, r.x); result.y = min(y, r.y); result.z = min(z, r.z);
		return result;
	}

	void  maxEq(const float3 r){
		x = max(x, r.x); y = max(y, r.y); z = max(z, r.z);
	}
	void  minEq(const float3 r){
		x = min(x, r.x); y = min(y, r.y); z = min(z, r.z);
	}
	
	int octant()const ;
	void setByOct(const float3 &src, const float3 &dst);
	static int decodeOct(int oct_code);
	
	void addMax(const float3 &n){
		*this += n*0.01;
	}
	void addMin(const float3 &n){
		*this += n*-0.01;
	}
	
	bool menor(const float3& a)const{ return x < a.x || y < a.y || z < a.z; }
	bool maior(const float3& a)const{ return x > a.x || y > a.y || z > a.z; }
	
	float3 intercept(const float3 &f, float &d)const;
};
//_____________________________________________________________OVERLOAD COUT
inline std::ostream &operator<<( std::ostream &out, const float3 f ) {
	return out << "( " << f.x << ", " << f.y << ", " <<  f.z << " )";
} // end oeprator

inline std::istream &operator>>( std::istream &in, float3 f ) {
	return in >> f.x>> f.y>>  f.z ;
} // end oeprator
// ----------------------------------------------------------------------------
inline float length(const float3 &u){
	return sqrt( pow(u.x,2) + pow(u.y,2) + pow(u.z,2) );
}


struct BBox{
	float3 mini, maxi,c;
	
	BBox(){mini = float3(99999.0f); maxi=float3(-99999.0f);}
	
	void setCenter(){c=(maxi+mini)/2;}	
	
};
//_____________________________________________________________OVERLOAD COUT
inline std::ostream &operator<<( std::ostream &out, const BBox b ) {
	return out << "MIN ( "<<b.mini<<" )\nMAX ("<< b.maxi << ")\nC ("<<b.c<<")\n";
} // end oeprator




#endif // _float3_H_
