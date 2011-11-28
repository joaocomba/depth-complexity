#ifndef CAMERA_H__
#define CAMERA_H__

#include "vector.hpp"

struct Camera {
    
    vec3f pos;
    vec3f up;
    vec3f target;

    Camera();
    
    void applyTransform();

    void relMove(const vec3f& off);

    vec3f getDir() const;
};


#endif
