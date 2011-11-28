#ifndef VECTOR_H__
#define VECTOR_H__

#include <cmath>

template<class T>
struct vec3 {
    T x, y, z;

    vec3() {}
    
    template<class U>
    vec3(const vec3<U>& v) : x(v.x), y(v.y), z(v.z)
    {}
    

  vec3(const T& a, const T& b, const T& c=T(0)) : x(a), y(b), z(c)
    {}
    
    T squaredLength() const
    {
        return x*x + y*y + z*z;
    }
    
    T length() const
    {
        return std::sqrt(squaredLength());
    }

    void normalize()
    {
        const T len = length();
        x /= len;
        y /= len;
        z /= len;
    }


    vec3 operator+=(const vec3& b)
    {
        x += b.x;
        y += b.y;
        z += b.z;
        return *this;
    }
    
    vec3 operator-=(const vec3& b)
    {
        x -= b.x;
        y -= b.y;
        z -= b.z;
        return *this;
    }

    int operator==(const vec3 &Q) const
    {
        return (x==Q.x && y==Q.y && z==Q.z);
    }

    int operator!=(const vec3 &Q) const
    {
        return (x!=Q.x || y!=Q.y || z!=Q.z);
    }
};

template<class T>
const vec3<T>& operator+(const vec3<T>& a)
{
    return a;
}

template<class T>
vec3<T> operator-(const vec3<T>& a)
{
    return vec3<T>(-a.x, -a.y, -a.z);
}


template<class T>
vec3<T> operator-(const vec3<T>& a, const vec3<T>& b)
{
    return vec3<T>(a.x-b.x, a.y-b.y, a.z-b.z);
}

template<class T>
const vec3<T> operator+(const vec3<T>& a, const vec3<T>& b)
{
    return vec3<T>(a.x+b.x, a.y+b.y, a.z+b.z);
}

template<class T>
const vec3<T> operator*(const T& a, const vec3<T>& b)
{
    return vec3<T>(a*b.x, a*b.y, a*b.z);
}

template<class T>
const vec3<T> operator*(const vec3<T>& a, const T& b)
{
    return vec3<T>(a.x*b, a.y*b, a.z*b);
}

template<class T>
const vec3<T> operator/(const vec3<T>& a, const T& b)
{
    return vec3<T>(a.x/b, a.y/b, a.z/b);
}

template<class T>
T dot(const vec3<T>& a, const vec3<T>& b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

template<class T>
vec3<T> cross(const vec3<T>& a, const vec3<T>& b)
{
    return vec3<T>(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
        );
}



typedef vec3<float> vec3f;
typedef vec3<double> vec3d;



template<class T>
struct vec4 {
    T x, y, z, w;
  
    vec4() {}
    vec4(const T& a, const T& b, const T& c, const T& d) :
        x(a), y(b), z(c), w(d)
    {}
    vec4(const vec3<T>& v, const T& d) :
        x(v.x), y(v.y), z(v.z), w(d)
    {}
    
    vec3<T> xyz() const
    {
        return vec3<T>(x, y, z);
    }
    
};

template<class T>
vec4<T> operator,(const vec3<T>& v, const T& d)
{
    return vec4<T>(v, d);
}

template<class T>
T dot(const vec4<T>& a, const vec4<T>& b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

typedef vec4<float> vec4f;
typedef vec4<double> vec4d;

#include <ostream>

template<class T>
std::ostream& operator<<(std::ostream& o, const vec3<T>& v)
{
    return o << "[" << v.x << "," << v.y << "," << v.z << "]";
}

template<class T>
std::ostream& operator<<(std::ostream& o, const vec4<T>& v)
{
    return o << "[" << v.x << "," << v.y << "," << v.z << "," << v.w << "]";
}

#endif
