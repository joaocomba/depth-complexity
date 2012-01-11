#include "float3.h"

float3::float3( float x_, float y_, float z_ ){
    x = x_;
    y = y_;
    z = z_;
}

void float3::set( float x_, float y_, float z_ ){
    x = x_;
    y = y_;
    z = z_;
}
void float3::set( float *v ){
    x = v[0];
    y = v[1];
    z = v[2];
}

float float3::length( void ){
    return( (float)sqrt( x * x + y * y + z * z ) );
}

float3 float3::normalize( void ){
    float fLength = length();

    x = x / fLength;
    y = y / fLength;
    z = z / fLength;
    return *this;
}

float3 float3::znormal(){
	x = x/z;
	y = y/z;
	z = 1;
	return *this;
}

/*float3 normals[6] = {
	(1.0, 0.0, 0.0),
	(0.0, 1.0, 0.0),
	(0.0, 0.0, 1.0),
	(-1.0, 0.0, 0.0),
	(0.0, -1.0, 0.0),
	(0.0, 0.0, -1.0),	
};*/

#define EPS  0.0001
float3 float3::intercept(const float3 &b, float &dist)const{
	const float3 &a = *this;
	float3 ba = b-a;
	float3 ab = a-b;
		
	float3 normal; normal.setByOct(a, b);

	dist = normal.dot(a) / normal.dot(ab);
	if( dist >= 1 || dist <= 0) dist= -10;
	return a + ba*(dist);
}



int float3::octant()const{
	if( y < -EPS )
		if ( z < -EPS )
			if ( x < -EPS )
				return ZERO;
			else if ( x > EPS )
				return ONE;
			else
				return ZERO * ONE;
		else if (z>EPS)//z>0
			if ( x < -EPS )
				return THREE;
			else if ( x > EPS )
				return TWO;	
			else // x =0
				return THREE * TWO;
		else // z==0
			if ( x < -EPS )
				return THREE * ZERO;
			else if ( x > EPS )
				return TWO * ONE;
			else // x == 0
				return TWO * ONE * THREE * ZERO;
	else if( y > EPS )// y > 0		
		if ( z < -EPS )
			if ( x < -EPS )
				return FOUR;
			else if (x>EPS)
				return FIVE;
			else
				return FOUR * FIVE;
		else if (z>EPS)//z>0
			if ( x < -EPS )
				return SEVEN;
			else if(x>EPS)
				return SIX;	
			else
				return SIX * SEVEN;
		else // z==0
			if ( x < -EPS )
				return FOUR * SEVEN;
			else if ( x > EPS )
				return FIVE * SIX;
			else // x == 0
				return FOUR * FIVE * SIX * SEVEN;					
	else //y==0
		if ( z < -EPS )
			if ( x < -EPS )
				return ZERO*FOUR;
			else if ( x > EPS )
				return FIVE*ONE;
			else // x==0
				return FIVE*ONE*FOUR*ZERO;
		else if(z>EPS)//z>0
			if ( x < -EPS )
				return SEVEN * THREE;
			else if ( x > EPS )
				return SIX * TWO;
			else // x == 0
				return SIX * TWO * SEVEN * THREE;
		else // z==0
			if ( x < -EPS )
				return SEVEN * THREE * FOUR * ZERO;
			else if ( x > EPS )
				return SIX * TWO * FIVE * ONE;
			else // x == 0
				return SIX * TWO * SEVEN * THREE * FIVE * ONE * ZERO * FOUR;	
}

int float3::decodeOct(int oct_code){
	switch(oct_code){
		case ZERO: return 0; break;
		case ONE:  return 1; break;
		case TWO:  return 2; break;
		case THREE: return 3; break;
		case FOUR:  return 4;  break;
		case FIVE:	return 5; break;
		case SIX:	return 6; break;
		case SEVEN: return 7; break;
		default: {
			if( oct_code%ZERO == 0 ) return 0;
			if( oct_code%ONE == 0 ) return 1;
			if( oct_code%TWO == 0 ) return 2;
			if( oct_code%THREE == 0 ) return 3;
			if( oct_code%FOUR == 0 ) return 4;
			if( oct_code%FIVE == 0 ) return 5;
			if( oct_code%SIX == 0 ) return 6;
			if( oct_code%SEVEN == 0 ) return 7;
			return -1;
		}break;
	}
}


/*	if( n.length() <=1 )
		dn = n.dot(src) / n.dot(dir);
	if( m.length() <=1 )
		dm = m.dot(src) / m.dot(dir);
	if( w.length() <=1 )
		dw = w.dot(src) / w.dot(dir);
	
	if( dn < dm ){
		if(dn < dw)	*this = n;
		else		*this = w;
	}else{
		if(dm < dw)	*this = m;
		else		*this = w;
	}//*/
	
void float3::setByOct(const float3 &src, const float3 &dst){
	float3 dir = src-dst;
	float3 n(0.0f,0.0f,0.0f), src_aux;

float3 defNormals[6] = {
	float3(1, 0, 0 ),
	float3(-1, 0, 0 ),
	float3(0, 1, 0 ),
	float3(0, -1, 0 ),
	float3(0, 0, 1 ),
	float3(0, 0, -1 )
};
	float minDist = 9999999999.0f;
	float dist;
	
	for( int i=0; i < 6; i++ ){
		float dtd = defNormals[i].dot(dir);
		if( dtd != 0 ){
			float dts = defNormals[i].dot(src);
			if(dts == 0 ){//src on the plane
				src_aux = src + EPS;
				dts = defNormals[i].dot(src_aux);
			}
			dist = dts/dtd;
			if(dist>=0 && dts !=0 ){
				if(dist < minDist){
					minDist = dist;
					n = defNormals[i];
				}
			}
		}
	}

	*this = n;

	/*for( int i=-1; i < 8;  ){	
		i++;
		switch (a){
			case ZERO: 
				switch(b){
					case ONE:	n.set(1,0,0); normals.push_back(n); break;
					case TWO:	n.set(1,0,0); normals.push_back(n); n.set(0,0,1); normals.push_back(n); break;
					case THREE: n.set(0,0,1); normals.push_back(n);break;
					case FOUR:	n.set(0,1,0); normals.push_back(n);break;
					case FIVE:	n.set(1,0,0); normals.push_back(n); n.set(0,1,0); normals.push_back(n);break;
					case SIX:	n.set(1,0,0); normals.push_back(n); n.set(0,0,1); normals.push_back(n); n.set(0,1,0); normals.push_back(n);break;				
					case SEVEN: n.set(0,0,1); normals.push_back(n); n.set(0,1,0);normals.push_back(n);break;					
				}break;	
			case ONE:
				switch(b){
					case ZERO:	 n.set(-1, 0, 0); normals.push_back(n);break;
					case TWO:	 n.set(0, 0, 1);  normals.push_back(n);break; 
					case THREE:  n.set(-1, 0, 0); normals.push_back(n);n.set(0,0,1);normals.push_back(n);break;	
					case FOUR:	 n.set(-1, 0, 0); normals.push_back(n);n.set(0,1,0);normals.push_back(n);break;
					case FIVE:	 n.set(0,1,0);    normals.push_back(n);break;
					case SIX:	 n.set(0, 0, 1); normals.push_back(n);n.set(0,1,0); normals.push_back(n);break;
					case SEVEN:  n.set(0, 0, 1); normals.push_back(n);n.set(-1,0,0);normals.push_back(n);n.set(0,1,0);normals.push_back(n);break;	
				}break;	
			case TWO:
				switch(b){
					case ZERO:	 n.set(0,0,-1); normals.push_back(n); n.set(-1,0,0); normals.push_back(n);break;
					case ONE:	 n.set(0,0,-1);normals.push_back(n); break;
					case THREE:  n.set(-1,0,0); normals.push_back(n);break;
					case FOUR:	 n.set(0,0,-1); normals.push_back(n);n.set(-1,0,0);normals.push_back(n);n.set(0,1,0);normals.push_back(n);break;
					case FIVE:	 n.set(0,0,-1); normals.push_back(n);n.set(0,1,0);normals.push_back(n);break;
					case SIX:	 n.set(0,1,0); normals.push_back(n); break;
					case SEVEN:  n.set(-1,0,0); normals.push_back(n); n.set(0,1,0); normals.push_back(n);break;
				}break;	
			case THREE:
				switch(b){
					case ZERO:	 n.set(0,0,-1);normals.push_back(n); break;
					case ONE:	 n.set(1,0,0); normals.push_back(n); n.set(0,0,-1); normals.push_back(n);break;
					case TWO:	 n.set(1,0,0); normals.push_back(n); break;
					case FOUR:	 n.set(0,1,0); normals.push_back(n);n.set(0,0,-1); normals.push_back(n);break;
					case FIVE:	 n.set(1,0,0); normals.push_back(n);n.set(0,0,-1); n.set(0,1,0); normals.push_back(n);break;
					case SIX:	 n.set(0,1,0); normals.push_back(n);n.set(1,0,0); normals.push_back(n);break;
					case SEVEN:  n.set(0,1,0); break;
				}break;	
			case FOUR:
				switch(b){
					case ZERO:	 n.set(0,-1,0); normals.push_back(n); break;
					case ONE:	 n.set(1,0,0); normals.push_back(n); n.set(0,-1,0); normals.push_back(n);break;
					case TWO:	 n.set(1,0,0); normals.push_back(n); n.set(0,0,1); normals.push_back(n); n.set(0,-1,0); normals.push_back(n);break;
					case THREE:  n.set(0,0,1); normals.push_back(n); n.set(0,-1,0); normals.push_back(n); break;
					case FIVE:	 n.set(1,0,0); normals.push_back(n);break;
					case SIX:	 n.set(1,0,0); normals.push_back(n); n.set(0,0,1); normals.push_back(n);break;
					case SEVEN:  n.set(0,0,1); normals.push_back(n);break;
				}break;		
			case FIVE:
				switch(b){
					case ZERO:   n.set(-1,0,0); normals.push_back(n); n.set(0,-1,0); normals.push_back(n);break;
					case ONE:	 n.set(0,-1,0);normals.push_back(n); break;
					case TWO:	 n.set(0,0,1); normals.push_back(n); n.set(0,-1,0); normals.push_back(n);break;
					case THREE:  n.set(-1,0,0); normals.push_back(n); n.set(0,0,1);normals.push_back(n); n.set(0,-1,0); normals.push_back(n);break;
					case FOUR:	 n.set(-1,0,0);normals.push_back(n); break;
					case SIX:	 n.set(0,0,1); normals.push_back(n);break;
					case SEVEN:  n.set(-1,0,0); normals.push_back(n); n.set(0,0,1);normals.push_back(n); break;
				}break;		
			case SIX:
				switch(b){
					case ZERO:  n.set(-1,0,0); m.set(0,0,-1); w.set(0,-1,0); break;
					case ONE:   n.set(0,0,-1); m.set(0,-1,0); break;
					case TWO:	n.set(0,-1,0); break;
					case THREE: n.set(-1,0,0); m.set(0,-1,0); break;
					case FOUR:	n.set(-1,0,0); m.set(0,0,-1); break;
					case FIVE:	n.set(0,0,-1); break;
					case SEVEN: n.set(-1,0,0); break;
				}break;
			case SEVEN:
				switch(b){
					case ZERO:  n.set(0,0,-1); m.set(0,-1,0); break;
					case ONE:	n.set(1,0,0); m.set(0,0,-1); w.set(0,-1,0); break;
					case TWO:	n.set(1,0,0); m.set(0,-1,0); break;
					case THREE: n.set(0,-1,0); break;
					case FOUR:	n.set(0,0,-1); break;
					case FIVE:	n.set(0,0,-1); m.set(1,0,0); break;
					case SIX:	n.set(1,0,0); break;
				}break;	
		}//end switch
		a = A/octante[i];								
	}//*/
/*	if( n.length() <=1 )
		dn = n.dot(src) / n.dot(dir);
	if( m.length() <=1 )
		dm = m.dot(src) / m.dot(dir);
	if( w.length() <=1 )
		dw = w.dot(src) / w.dot(dir);
	
	if( dn < dm ){
		if(dn < dw)	*this = n;
		else		*this = w;
	}else{
		if(dm < dw)	*this = m;
		else		*this = w;
	}//*/
}

// Static utility methods...

float float3::distance( const float3 &v1,  const float3 &v2  ){
    float dx = v1.x - v2.x;
    float dy = v1.y - v2.y;
    float dz = v1.z - v2.z;

    return (float)sqrt( dx * dx + dy * dy + dz * dz );
}

 float float3::dot( const float3 &v2 )const {
    return( x * v2.x + y * v2.y + z * v2.z  );
}

 float float3::dotProduct( const float3 &v1,  const float3 &v2 ){
    return( v1.x * v2.x + v1.y * v2.y + v1.z * v2.z  );
}

 float3 float3::crossProduct( const float3 &v1,  const float3 &v2 ){
    float3 vCrossProduct;

    vCrossProduct.x =  v1.y * v2.z - v1.z * v2.y;
    vCrossProduct.y = -v1.x * v2.z + v1.z * v2.x;
    vCrossProduct.z =  v1.x * v2.y - v1.y * v2.x;

    return vCrossProduct;
}

// Operators...

float3 float3::operator + ( const float3 &other )const{
    float3 vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x + other.x;
    vResult.y = y + other.y;
    vResult.z = z + other.z;

    return vResult;
}

float3 float3::operator + ( void ) const{
    return *this;
}

float3 float3::operator - ( const float3 &other )const{
    float3 vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x - other.x;
    vResult.y = y - other.y;
    vResult.z = z - other.z;

    return vResult;
}

float3 float3::operator - (  ) const{
    float3 vResult(-x, -y, -z);
    return vResult;
}

float3 float3::operator * ( const float3 &other )const{
    float3 vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x * other.x;
    vResult.y = y * other.y;
    vResult.z = z * other.z;

    return vResult;
}

float3 float3::operator * ( const float scalar ){
    float3 vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x * scalar;
    vResult.y = y * scalar;
    vResult.z = z * scalar;

    return vResult;
}

float3 operator * ( const float scalar, const float3 &other ){
    float3 vResult(0.0f, 0.0f, 0.0f);

    vResult.x = other.x * scalar;
    vResult.y = other.y * scalar;
    vResult.z = other.z * scalar;

    return vResult;
}

float3 float3::operator / ( const float3 &other )const{
    float3 vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x / other.x;
    vResult.y = y / other.y;
    vResult.z = z / other.z;

    return vResult;
}

float3& float3::operator = ( const float3 &other ){
	x = y = z = 0.0f;
    x = other.x;
    y = other.y;
    z = other.z;

    return *this;
}

//-----------------------------------------------------------------------------
// rotaciona o vetor 3D em torno do eixo Y
//-----------------------------------------------------------------------------
void float3::rotateY( float fAnguloEmRadianos) {
  float fSeno, fCosseno;

  fSeno    = sin(fAnguloEmRadianos);
  fCosseno = cos(fAnguloEmRadianos);

  x = x * fCosseno + z * fSeno;
  z = z * fCosseno - x * fSeno;
}

//-----------------------------------------------------------------------------
// rotaciona o vetor 3D em torno do eixo Y
//-----------------------------------------------------------------------------
//void float3::rotate( float X, float Y, floatZ, float alphaRad) {
	
//}
//-----------------------------------------------------------------------------OPERATORS
float3 float3::operator / (const float r){ 
	float3 vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x / r;
    vResult.y = y / r;
    vResult.z = z / r;
    
    return vResult;
}
