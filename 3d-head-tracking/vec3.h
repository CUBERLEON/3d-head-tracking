#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <cstdio>

struct vec3 {
	vec3(float x = 0, float y = 0, float z = 0) : m_x(x), m_y(y), m_z(z) {}
    vec3(const vec3& r) : m_x(r.getX()), m_y(r.getY()), m_z(r.getZ()) {}
	~vec3() {}

	float getX() const { return m_x; }
    float getY() const { return m_y; }
    float getZ() const { return m_z; }

    void setX(float x) { m_x = x; }
    void setY(float y) { m_y = y; }
    void setZ(float z) { m_z = z; }

    float& operator[](int index) {
        if (index == 0) return m_x;
        if (index == 1) return m_y;
        return m_z;
    }

    float operator[](int index) const {
        if (index == 0) return m_x;
        if (index == 1) return m_y;
        return m_z;
    }

    vec3& operator=(const vec3& r) {
        m_x = r.getX(); m_y = r.getY(); m_z = r.getZ();
        return *this;
    }

    vec3& operator+=(const vec3& r) {
        m_x += r.getX(); m_y += r.getY(); m_z += r.getZ();
        return *this;
    }

    vec3 operator*(float r) const {
        return vec3(m_x*r, m_y*r, m_z*r);
    }

    vec3 operator+(const vec3& r) const {
        return vec3(m_x+r.getX(), m_y+r.getY(), m_z+r.getZ());
    }

    vec3 operator-(const vec3& r) const {
        return operator+(r * -1);
    }

    static vec3 normalize(const vec3& r) {
        float d = sqrt(r.getX()*r.getX() + r.getY()*r.getY() + r.getZ()*r.getZ());
        if (d < 1e-8)
            return r;
        return vec3(r.getX()/d, r.getY()/d, r.getZ()/d);
    }

    static float dot(const vec3& a, const vec3& b) {
        return a.getX()*b.getX() + a.getY()*b.getY() + a.getZ()*b.getZ();
    }

    static vec3 cross(const vec3& a, const vec3& b) {
        return vec3(a.getY()*b.getZ() - a.getZ()*b.getY(), -a.getX()*b.getZ() + b.getX()*a.getZ(), a.getX()*b.getY() - a.getY()*b.getX());
    }

    void println() {
        printf("(%f %f %f)\n", m_x, m_y, m_z);
    }
private:
    float m_x, m_y, m_z;
};

#endif // VEC3_H
