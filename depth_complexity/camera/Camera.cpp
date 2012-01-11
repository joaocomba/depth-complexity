//-----------------------------------------------------------------------------
// classe para manipulação de uma câmera sintética
// AXES:	N = (at - eye)
//			U = up
//			V = N * U
//-----------------------------------------------------------------------------

//afUp = keep always normalized!!!!

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstring>
#include "Camera.h"
#include <math.h>
//-----------------------------------------------------------------------------
double trans[16] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};
bool down = true;
int NN = 50;

const float dNEAR =  1.9f;

float frustum[6][4];

void Camera::speed(int delta){ d = d*NN; NN+=delta; d = d/NN;}

 //usando com float3
  void matMulVec(double* i_mat, float3 &i_vec, float3 &o_vec){
	float aux[3];
	memset(aux, 0, sizeof(float)*3);
	for(size_t i=0; i<3; i++){
		aux[i]  = i_mat[i+4*0] * i_vec.x
				+ i_mat[i+4*1] * i_vec.y
				+ i_mat[i+4*2] * i_vec.z
				+ i_mat[i+4*3] * 1;
	}
	o_vec.x = aux[0];	o_vec.y = aux[1];	o_vec.z = aux[2];	
 }
 
void Camera::transf(double* m){
	memcpy(trans, m, sizeof(double)*16); 
	/*glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glLoadIdentity();
		gluLookAt(afEye.x, afEye.y, afEye.z, afAt.x,  afAt.y, afAt.z,afUp.x,  afUp.y, afUp.z);
		glMultMatrixd(trans);
		glMultMatrixd(m);		
		glGetDoublev(GL_MODELVIEW_MATRIX, trans);
	glPopMatrix();//*/
	/*if( !down ) {
		memcpy(trans, m, sizeof(double)*16);
	}else{
		matMulVec(m, afEye, afEye);
		down= false;
	}//*/
	//matMulVec(m, afAt, afAt);
}

//-----------------------------------------------------------------------------CONSTRUCTOR
Camera::Camera() {
	znear	=	dNEAR;
	zfar	=	50000.0f;
	alpha	=   45.0f;
	tgHalpha = tan(alpha/2.0f);
	setado = false;
	
	maxDPC = -1;
	//dst.set(0,0,0);
	
	perspective();
}
//-----------------------------------------------------------------------------DESTRUCTOR
Camera::~Camera() {}

//-----------------------------------------------------------------------------PERSPECTIVE VIEW
// calcs the best perspective view for the scene
void Camera::perspective(){
	if( !setado ){
		afEye.x = 35.0f;   afAt.x  = -0.5f; afUp.x  = 0.0f;
		afEye.y = 35.0f;   afAt.y  = -0.5f; afUp.y  = 1.0f;
		afEye.z = 35.0f;   afAt.z  = -0.5f; afUp.z  = 0.0f;
	}else{
		//recalc
		afAt = ((mn+mx)/2.0f);
		//define y
		float deltay = max( (mx.x-mn.x), (mx.z-mn.z))/2.0;
		afEye.y = deltay/tgHalpha;
		//define x
		float deltax = max( (mx.y-mn.y), (mx.z-mn.z))/2.0;
		afEye.x = deltax/tgHalpha;
		//define z
		float deltaz = max( (mx.y-mn.y), (mx.x-mn.x))/2.0 ;
		afEye.z = deltaz/tgHalpha;

		afEye = -afEye;
		
		//afUp.set(0, 1, 0);		
		afUp.crossProduct((float3(afAt.x+10, afAt.y, afAt.z) - afEye), (afAt-afEye)).normalize();
		lookAt();
	}
}
//-----------------------------------------------------------------------------FRONT VIEW
// calcs the best front view for the scene
void Camera::front(){//look at XY plane
	if( !setado){
		afEye.x = 0.0f; afAt.x  = 0.0f; afUp.x  = 0.0f;
		afEye.y = 0.0f; afAt.y  = 0.0f; afUp.y  = 1.0f;
		afEye.z = 5.0f; afAt.z  = 0.0f; afUp.z  = 0.0f;
	} else {
		//recalc
		afAt = ((mn+mx)/2.0f);//centralize at
		//define z
		float delta = max( (float)fabs(mx.y-mn.y), (float)fabs(mx.x-mn.x))/2.0f;
		afEye = afAt;	afEye.z = mx.z + (delta)/tgHalpha;
		
		afUp.set(0, 1, 0);
		lookAt();	
	}
}
//-----------------------------------------------------------------------------UP VIEW
// calcs the best up view for the scene
void Camera::up(){//look at XZ plane
	afAt.x = -74.529; afAt.y = 123.124; afAt.z = -77.43;
	afEye.x = -80.2883; afEye.y = 127.903; afEye.z = -83.3244;
	afUp.x = 0; afUp.y = 1; afUp.z = 0;
}
//-----------------------------------------------------------------------------TOP VIEW
// calcs the best up view for the scene
void Camera::top(float3 pos){//look at XZ plane
	afAt.x = 0; afAt.y = 0; afAt.z = 0;
	afEye= pos;
	afUp.x = 1; afUp.y = 0; afUp.z = 0;
	afUp.normalize();
}
//-----------------------------------------------------------------------------MOVE FRENTE
// move the camera in its N axe
void Camera::MoveFrente(float fDistancia) {// walk on N
	float3 v = afAt - afEye; //get direction
	afEye += v *fDistancia;
	afAt  += v *fDistancia;
}
//-----------------------------------------------------------------------------MOVE LADO
// move the camera in its V axe
void Camera::MoveLado(float fDistancia) {//walk on V
	float l = (afAt - afEye).length();
	float3 v = afAt - afEye;
	float3 v1 = float3::crossProduct(v, afUp); v1.normalize();//get direction
	afEye += v1*fDistancia * l;
	afAt  += v1*fDistancia * l;
}
//-----------------------------------------------------------------------------MOVE VERTICAL
void Camera::MoveCimaBaixo(float fValor) {//walk on U
	float l = (afAt - afEye).length();
	afEye += afUp *fValor * l;
	afAt  += afUp *fValor * l;
}
//-----------------------------------------------------------------------------MOVE FRENTE OBJ
// move the camera in its N axe
void Camera::MoveFrenteObj(float fDistancia) {// walk on N
	afAt = (mx+mn)/2.0f;//look at the center of the scene
	float3 v = afAt - afEye; //get direction
	afEye += v *fDistancia;
}
//-----------------------------------------------------------------------------MOVE LADO OBJ
// move the camera in its V axe
void Camera::MoveLadoObj(float fDistancia) {//walk on V
	afAt = (mx+mn)/2.0f;//look at the center of the scene
	float3 v = afAt - afEye;
	float3 v1 = float3::crossProduct(v, afUp); v1.normalize();//get direction
	afEye += v1*fDistancia;
}
//-----------------------------------------------------------------------------MOVE VERTICAL OBJ
void Camera::MoveCimaBaixoObj(float fValor) {//walk on U
	afAt = (mx+mn)/2.0f;//look at the center of the scene
	afEye -= afUp *fValor;
}
//-----------------------------------------------------------------------------LOOK LEFT RIGHT OBJ
// rotaciona a camera em torno de UP (u)
void Camera::lookLefRigObj(float alpharad) {
	afAt = (mx+mn)/2.0f;//look at the center of the scene
	float3 v0 = (afAt - afEye);	

	//quat rot
	quaternion q ( cos(-alpharad/2.0f), sin(-alpharad/2.0f), afUp);//eixo
	quaternion v (0, v0);//vetor a ser rotacionado
	v = (q.cross(v)).cross( q.cconju() );// q v q*
	afEye = afAt - v.q;//olho fixo, at = olho + direção
	
	v =quaternion(0, afUp);//vetor a ser rotacionado
	v = (q.cross(v)).cross( q.cconju() );// q v q*	
	afUp = v.q + float3(0,1,0); afUp.normalize();
}
//-----------------------------------------------------------------------------LOOK VERTICAL OBJ
// rotaciona a camera em torno de v ( n cross u )
void Camera::lookUpDownObj(float alpharad) {
	afAt = (mx+mn)/2.0f;//look at the center of the scene
	float3 v0 = (afAt - afEye);	
	float3 v1 = float3::crossProduct( v0, afUp ).normalize();
	
	//quat rot
	quaternion q ( cos(-alpharad/2.0f), sin(-alpharad/2.0f), v1);//eixo
	quaternion v (0, v0);//vetor a ser rotacionado
	v = (q.cross(v)).cross( q.cconju() );// q v q*
	afEye = afAt - v.q;//olho fixo, at = olho + direção
	
	v =quaternion(0, afUp);//vetor a ser rotacionado
	v = (q.cross(v)).cross( q.cconju() );// q v q*	
	afUp = v.q+ float3(0,1,0); afUp.normalize();
}
//-----------------------------------------------------------------------------LOOK LEFT RIGHT
// rotaciona a camera em torno de UP (u)
void Camera::lookLefRig(float alpharad) {
	float3 v0;
	v0 = (afAt - afEye);	

	//quat rot
	quaternion q ( cos(-alpharad/2.0f), sin(-alpharad/2.0f), afUp);//eixo
	quaternion v (0, v0);//vetor a ser rotacionado
	v = (q.cross(v)).cross( q.cconju() );// q v q*
	afAt = afEye + v.q;//olho fixo, at = olho + direção
	
	v =quaternion(0, afUp);//vetor a ser rotacionado
	v = (q.cross(v)).cross( q.cconju() );// q v q*	
	afUp = v.q+ float3(0,1,0); afUp.normalize();
}
//-----------------------------------------------------------------------------LOOK VERTICAL
// rotaciona a camera em torno de v ( n cross u )
void Camera::lookUpDown(float alpharad) {
	float3 v0 = (afAt - afEye);	
	float3 v1 = float3::crossProduct( v0, afUp ).normalize();
	
	//quat rot
	quaternion q ( cos(-alpharad/2.0f), sin(-alpharad/2.0f), v1);//eixo
	quaternion v (0, v0);//vetor a ser rotacionado
	v = (q.cross(v)).cross( q.cconju() );// q v q*
	afAt = afEye + v.q;//olho fixo, at = olho + direção
	
	v =quaternion(0, afUp);//vetor a ser rotacionado
	v = (q.cross(v)).cross( q.cconju() );// q v q*	
	afUp = v.q+ float3(0,1,0); afUp.normalize();
}
//-----------------------------------------------------------------------------ROT IN N
void Camera::rotInN(float alpharad){
	float3 v0 = (afAt - afEye);	//N
	//float3 v1 = float3::crossProduct( v0, afUp ).normalize();//V
	
	//quat rot
	quaternion q ( cos(alpharad/2.0f), sin(alpharad/2.0f), v0);//eixo
	quaternion v (0, afUp);//vetor a ser rotacionado
	v = (q.cross(v)).cross( q.cconju() );// q v q*
	afUp = v.q+ float3(0,1,0); afUp.normalize();
}
//-----------------------------------------------------------------------------UPDATE
void Camera::update(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
		gluPerspective( alpha, aspect, znear, zfar); 
	glMatrixMode(GL_MODELVIEW);
}
//-----------------------------------------------------------------------------RESHAPE
// reshape view sizes
void Camera::reshape(int w, int h){
	//-- set the projection matrix
	aspect = float(w) / float(h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
		gluPerspective( alpha, aspect, znear, zfar); 
}
//-----------------------------------------------------------------------------RESHAPE
// reshape view sizes
void Camera::frustu(double ang, double asp){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
		gluPerspective( ang, asp, znear, zfar); 
}
//-----------------------------------------------------------------------------RESHAPE
// reshape view sizes
void Camera::lnnPerspective()const{
	// set the projection matrix
	gluPerspective( alpha, aspect, znear, zfar); 
}
//-----------------------------------------------------------------------------LOOK AT

// reposit. camera
void Camera::lookAt()const{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(afEye.x, afEye.y, afEye.z, afAt.x,  afAt.y, afAt.z, afUp.x,  afUp.y, afUp.z);
	//glMultMatrixd(trans);
}
//-----------------------------------------------------------------------------LOOK AT
void Camera::lnnLookAt()const{
	gluLookAt(afEye.x, afEye.y, afEye.z, afAt.x,  afAt.y, afAt.z,afUp.x,  afUp.y, afUp.z);
}
//-----------------------------------------------------------------------------LOOK AT
// reposit. camera
void Camera::setupText(){
	glMatrixMode( GL_TEXTURE );
	glLoadIdentity();
	
	gluPerspective( alpha, aspect, znear, zfar); 
	gluLookAt(afEye.x, afEye.y, afEye.z, afAt.x,  afAt.y, afAt.z,afUp.x,  afUp.y, afUp.z);

}
//-----------------------------------------------------------------------------LOOK AT
// reposit. camera
void Camera::setupThing(){
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	
	gluPerspective( alpha, aspect, znear, zfar); 
	gluLookAt(afEye.x, afEye.y, afEye.z, afAt.x,  afAt.y, afAt.z,afUp.x,  afUp.y, afUp.z);
}
//-----------------------------------------------------------------------------LOOK AT
// reposit. camera
void Camera::setupMVP(){
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt(afEye.x, afEye.y, afEye.z, afAt.x,  afAt.y, afAt.z,afUp.x,  afUp.y, afUp.z);
	
	gluPerspective( alpha, aspect, znear, zfar); 
}
//-----------------------------------------------------------------------------GET VIEW
// get view matrix without modeling
void Camera::getView(float* v){
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		gluLookAt(afEye.x, afEye.y, afEye.z,
					afAt.x,  afAt.y, afAt.z,
					afUp.x,  afUp.y, afUp.z);	
		glGetFloatv(GL_MODELVIEW_MATRIX , v);
	glPopMatrix();
		//GLenum errogl = glGetError();
}
//-----------------------------------------------------------------------------GET MODEL VIEW
// get view matrix without modeling
void Camera::getModelView(float* v){
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glLoadIdentity();
		gluLookAt(afEye.x, afEye.y, afEye.z,
					afAt.x,  afAt.y, afAt.z,
					afUp.x,  afUp.y, afUp.z);	
		glGetFloatv(GL_MODELVIEW_MATRIX , v);
	glPopMatrix();
	//GLenum errogl = glGetError();	
}
//-----------------------------------------------------------------------------update modelview
void Camera::updateModelView(){
	float mv[16];
	getModelView(mv);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(mv);
}
//-----------------------------------------------------------------------------set perspec
void Camera::setPerspec(float fov, float _aspect, float n, float f){
	alpha = fov;
	aspect = _aspect;
	znear = n;
	zfar = f;

	gluPerspective( fov, aspect,n, f);
}
//-----------------------------------------------------------------------------TRACE
// trace the vector and find the point that intercept the plane z=1
void Camera::traceEye(float3 &v, float w){
	float t = (w - afEye.z)/(v.z - afEye.z);
	float3 a;
	a.x =  v.x - afEye.x ;
	a.y =  v.y - afEye.y ;
	a.z =  v.z - afEye.z ;	
	a = a * t;
	v = float3(afEye.x+a.x, afEye.y+a.y, afEye.z+a.z);
}
//-----------------------------------------------------------------------------ADJUST UP
// i try to adjust the UP but.......
void Camera::adjustUp(){
	//afUp = afUp.crossProduct((float3(afAt.x+10, afAt.y, afAt.z) - afEye), (afAt-afEye));
	//afUp.normalize();
/*	float3 v = afEye * afUp;	v.normalize();
	v = afAt + v;
	float3 w = afEye * afAt;
	float3 t = afEye * v;
	afUp = w * t;	
	afUp.normalize();	
	afUp.y = afUp.x + afUp.z;
	printme(afUp);//*/
}

//-----------------------------------------------------------------------------BBOX
// setup bouding box
void Camera::bbox(float3 mn_, float3 mx_, bool review){
	mn = mn_ ;	mx=mx_ ;
	setado = true;
	
	bs.c = (mn+mx)/2;
	bs.r = (bs.c-mn).length();
	
	setFrustum();
	
	if( review ) perspective();
}

//-----------------------------------------------------------------------------DRAW AT
void Camera::drawAt(){
	glColor3f(0.5, 0, 0); glPointSize(5);
	glBegin(GL_POINTS); 
		afAt.glVertex(); 
	glEnd();
}
//-----------------------------------------------------------------------------DRAW BB CENTER
void Camera::drawBBcenter(){ 
	float3 mid = ( mn + mx ) /2.0f;
	glColor3f(1, 1, 1);
	glBegin(GL_POINTS); 
		glVertex3f(mid.x, mid.y, mid.z); 
	glEnd();
}
//-----------------------------------------------------------------------------
void Camera::reset(){
  afEye.x = 5.0f;
  afEye.y = 5.0f;
  afEye.z = 5.0f;

  afAt.x = -0.5f;
  afAt.y = -0.5f;
  afAt.z = -0.5f;

  afUp.x = 0.0f;
  afUp.y = 1.0f;
  afUp.z = 0.0f;
}
//-----------------------------------------------------------------------------
float* Camera::getEye4f(float* e)const{
	e[0]=afEye.x;
	e[1]=afEye.y;
	e[2]=afEye.z;
	e[3]=1;
	return e;
}
//-----------------------------------------------------------------------------
void Camera:: setEye(float a, float b, float c){ 
	afEye.x += a*DIF; afEye.y -= b*DIF; afEye.z -= c*DIF;
	afAt.x += a*DIF; afAt.y -= b*DIF; afAt.z -= c*DIF;
}

//-----------------------------------------------------------------------------
void Camera::push(){
	Keyframe set;
	set.e = afEye;
	set.a = afAt;
	set.u = afUp;
	sets.push_back(set);
}
//-----------------------------------------------------------------------------
void Camera::save(string filename){
	ofstream file (filename.c_str());
	file<<maxDPC<<endl;
	dst = afEye + afAt;
	file<<dst<<endl;
	file<<sets.size()<<endl;
	for(size_t i=0; i<sets.size(); i++){
		file<<sets[i];
	}
	file.close();
}
//-----------------------------------------------------------------------------
void Camera::load(string filename){
	ifstream file(filename.c_str());
	if ( !file.is_open() ){
		std::cerr<<"unable to open "<<filename.c_str()<<endl;
		return;	
	}
	file>>maxDPC; 
	file>>dst;
	//printme(maxDPC); printme(dst); 
	size_t n; file>>n;
	for(size_t i=0; i<n; i++){
		Keyframe s; file>>s;
		sets.push_back(s);
	}
	idx = 0;
	setupInterv();
}
//-----------------------------------------------------------------------------
void Camera::setupInterv(){
	afEye = sets[idx].e;
	afAt = dst-afEye;
	afUp = sets[idx].u;
	//printme(afEye);printme(afAt);printme(afUp);printme(dst);
	//afAt.normalize();
	if(sets.size()<2) return;
	
	d = -9999999;
	for(size_t i=0; i<sets.size()-1; i++){
		d = max(d, (sets[i].e-sets[i+1].e).length());
	}
	d = d/NN;
	idx = 0;
	t=0;
	if( sets.size()>1){
		r = sets[1].e - sets[0].e;
		D = r.length();
		r.normalize();
	}
}
//-----------------------------------------------------------------------------interpolate
void Camera::interpolateSets(){
	if(sets.size()<2){
	//	afEye = sets[idx].e;
	//	afAt = dst-afEye;
	//	afAt.normalize();	
	//printme(afEye);printme(afAt);printme(afUp);printme(dst);	
		return;
	} 
	if( t*d < D ){
		afEye = sets[idx].e + r*t*d;
		afAt = dst-afEye;
		afAt.normalize();
		t++;
	}else{
		int size = sets.size();
		idx = (idx+1)%size;
		int next = (idx+1)%size;
		r = sets[next].e - sets[idx].e;
		D = r.length();
		r.normalize();
		t=0;
	}
}
//-----------------------------------------------------------------------------PRINT
#include <fstream>
void Camera::print(){
	ofstream file ("camera.txt");
	
	file<<"afAt.x = "<<afAt.x<<"; afAt.y = "<<afAt.y<<"; afAt.z = "<<afAt.z<<";";
	file<<"afEye.x = "<<afEye.x<<"; afEye.y = "<<afEye.y<<"; afEye.z = "<<afEye.z<<";";
	file<<"afUp.x = "<<afUp.x<<"; afUp.y = "<<afUp.y<<"; afUp.z = "<<afUp.z<<";\n\n";

	file.close();
	
	/*
	printme(afAt.x); 	printme(afEye.x);
	printme(afAt.y); 	printme(afEye.y);
	printme(afAt.z); 	printme(afEye.z);*/
}

//----------------------------------------------------------------------------- set frustum
void Camera::setFrustum(){

	float dn =(afEye-bs.c).length()-bs.r;
	znear = (dn<=0 )? dNEAR : dn;
	zfar = (afEye-bs.c).length()+bs.r;
	
	printme(znear);
	printme(zfar);
	
	gluPerspective( alpha, aspect, znear, zfar);
}

//----------------------------------------------------------------------------- update frustum
void Camera::updateFrustum(){
	float3 a = (mn - afEye);	
	float3 b = (mx - afEye);	
	float3 c = (afAt - afEye);	c = c.normalize();
	
	float x = c.dot(a); 
	float y = c.dot(b); 
	float fa, ne;
	
	if( x > y ){
		fa = x;
		ne = (y>0)? y : dNEAR;
	}else{
		fa = y;
		ne = (x>0)? x : dNEAR;	
	}	
	//-- fuck the tigh frustum
	ne = 0.01;
	fa = 600000;
	
	gluPerspective( alpha, aspect, ne, fa);
}
