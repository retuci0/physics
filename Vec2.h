#pragma once

#include <cmath>
#include <stdexcept>


template <typename T>
class Vec2 {
public:
    T x, y;
    constexpr Vec2(T x = T(0), T y = T(0)) : x(x), y(y) {}

    static Vec2 ZERO;
    static Vec2 ONE;
    static Vec2 I;
    static Vec2 J;


    // operadores aritméticos

    constexpr Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
    constexpr Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
    constexpr Vec2 operator*(T scalar) const { return Vec2(x * scalar, y * scalar); }
    constexpr Vec2 operator/(T scalar) const { return Vec2(x / scalar, y / scalar); }
    constexpr Vec2 operator-() const { return Vec2(-x, -y); }


    // operadores de asignación

    Vec2& operator+=(const Vec2& other) { x += other.x; y += other.y; return *this; }
    Vec2& operator-=(const Vec2& other) { x -= other.x; y -= other.y; return *this; }
    Vec2& operator*=(T scalar) { x *= scalar; y *= scalar; return *this; }
    Vec2& operator/=(T scalar) { x /= scalar; y /= scalar; return *this; }


    // comparación

    constexpr bool operator==(const Vec2& other) const { return x == other.x && y == other.y; }
    constexpr bool operator!=(const Vec2& other) const { return !(*this == other); }


    // métodos geométricos básicos

    constexpr T sqlen() const { return x * x + y * y; }
    T len() const { return std::sqrt(sqlen()); }
    constexpr T sqdist(const Vec2& other) const { return (other - *this).sqlen(); }
    T dist(const Vec2& other) const { return std::sqrt(sqdist(other)); }


    // productos

    constexpr T dot(const Vec2& other) const { return x * other.x + y * other.y; }
    constexpr T cross(const Vec2& other) const { return x * other.y - y * other.x; }


    // normalizar

    Vec2 norm() const {
        T lenSq = sqlen();
        if (lenSq == T(0)) return Vec2<T>::ZERO;
        return *this / std::sqrt(lenSq);
    }


    // rotaciones, ángulos en rads

    T angle() const { return std::atan2(y, x); }
    Vec2 rotate(T angle) const {
        T c = std::cos(angle);
        T s = std::sin(angle);
        return Vec2(x * c - y * s, x * s + y * c);
    }
};


// operador con escalar a la izquierda (para permitir 2 * v)

template <typename T>
constexpr Vec2<T> operator*(T scalar, const Vec2<T>& v) {
    return v * scalar;
}


// vectores comunes

template <typename T>
Vec2<T> Vec2<T>::ZERO = Vec2<T>(T(0), T(0));
template <typename T>
Vec2<T> Vec2<T>::ONE = Vec2<T>(T(1), T(1));
template <typename T>
Vec2<T> Vec2<T>::I = Vec2<T>(T(1), T(0));
template <typename T>
Vec2<T> Vec2<T>::J = Vec2<T>(T(0), T(1));


// tipos comunes

using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;
using Vec2i = Vec2<int>;