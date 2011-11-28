#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "camera.hpp"

Camera::Camera() :
    pos(0, 0, -4),
    up(0, 1, 0),
    target(0, 0, 0)
{}



void
Camera::applyTransform()
{
    vec3f f = target - pos;
    f.normalize();

    up.normalize();

    vec3f s = cross(f, up);
    vec3f u = cross(s, f);
    up = u;
#if 1
    GLfloat mat[16] = {
        s.x, u.x, -f.x, 0,
        s.y, u.y, -f.y, 0,
        s.z, u.z, -f.z, 0,
        0, 0, 0, 1
    };
    glMultMatrixf(mat);
    glTranslatef(-pos.x, -pos.y, -pos.z);
#else
    gluLookAt(pos.x, pos.y, pos.z,
              target.x, target.y, target.z,
              up.x, up.y, up.z);
#endif
}


void
Camera::relMove(const vec3f& off)
{
    vec3f f = target - pos;

    vec3f localX = cross(up, -f);
    localX.normalize();
    
    vec3f localY = cross(-f, localX);
    localY.normalize();
    
    vec3f localZ = cross(localX, localY);
    
    pos += off.x * localX + off.y * localY + off.z * localZ;
}

vec3f
Camera::getDir() const
{
    vec3f dir = target - pos;
    dir.normalize();
    return dir;
}
