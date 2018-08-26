#ifndef VEC2_H
#define VEC2_H

#include <cmath>

struct vec2 {
	vec2(float x = 0, float y = 0) : m_x(x), m_y(y) {}
    vec2(const vec2& r) : m_x(r.getX()), m_y(r.getY()) {}
	~vec2() {}

	float getX() const { return m_x; }
    float getY() const { return m_y; }

    void setX(float x) { m_x = x; }
    void setY(float y) { m_y = y; }

    float& operator[](int index) {
        if (index == 0) return m_x;
        return m_y;
    }

    float operator[](int index) const {
        if (index == 0) return m_x;
        return m_y;
    }

    vec2& operator=(const vec2& r) {
        m_x = r.getX(); m_y = r.getY();
        return *this;
    }

    vec2 operator*(float r) const {
        return vec2(m_x*r, m_y*r);
    }

    vec2 operator+(const vec2& r) const {
        return vec2(m_x+r.getX(), m_y+r.getY());
    }

    vec2 operator-(const vec2& r) const {
        return operator+(r * -1);
    }

    static vec2 normalize(const vec2& r) {
        float d = sqrt(r.getX()*r.getX() + r.getY()*r.getY());
        return vec2(r.getX()/d, r.getY()/d);
    }

    static float dot(const vec2& a, const vec2& b) {
        return a.getX()*b.getX() + a.getY()*b.getY();
    }

    static float cross(const vec2& a, const vec2& b) {
        return a.getX()*b.getY() - a.getY()*b.getX();
    }
private:
    float m_x, m_y;
};

#endif // VEC2_H
