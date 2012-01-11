#ifndef QUATERNION_H
#define QUATERNION_H

/*
	created by Lenna
	at 07/04/2009
*/

#include <math.h>
#include "float3.h"
#include "opengl.h"

struct quaternion{
	//q0 + q //q complex, q0 real
	float q0; float3 q;
	
	quaternion(){}
	quaternion(float s, float3 v): q0(s), q(v){}
	quaternion(const quaternion& q): q0(q.q0), q(q.q){}
	quaternion(float cosen, float sen, float3 v):q0(cosen), q(v){ q = sen*q; }
	
	quaternion cconju(){ return quaternion(q0, -q); }//complex conjugation
	
//_____________________________________________________________________DOT	
	/*quaternion operator*( const quaternion x ) const {//dot  n interessa ainda
	}*/
//_____________________________________________________________________CROSS
	quaternion cross( const quaternion x ) const {//cross
		//Qp x Qq = (p0*q0) - (p*q) 
		//			+ q0*p + p0*q + p cross q
		float s = (x.q0 * q0) - float3::dotProduct(x.q , q);
		float3 p = (x.q0*q) + (q0 * x.q) + float3::crossProduct(x.q, q);
		return quaternion(s, p);
	}
};

//rotation = q v q*

#endif
