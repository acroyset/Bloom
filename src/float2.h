//
// Created by Andreas Royset on 4/26/25.
//

#ifndef VEC2F_H
#define VEC2F_H

#include <cmath>

class float2 {
    public:
    float x, y;

    float2() {
        x = y = 0;
    }
    float2(float x, float y) {
        this->x = x;
        this->y = y;
    }

    float2 operator+(const float2 &other) const {
        return {this->x + other.x, this->y + other.y};
    }
    float2 operator-(const float2 &other) const {
        return {this->x - other.x, this->y - other.y};
    }
    float2 operator-() const {
        return {-this->x, -this->y};
    }
    float2 operator*(const float2 &other) const {
        return {this->x * other.x, this->y * other.y};
    }
    float2 operator/(const float2 &other) const {
        return {this->x / other.x, this->y / other.y};
    }
    float2 operator*(const float a) const {
        return {this->x * a, this->y * a};
    }
    float2 operator/(const float a) const {
        return {this->x / a, this->y / a};
    }
    float2& operator+=(const float2 &other) {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }
    float2& operator-=(const float2 &other) {
        this->x -= other.x;
        this->y -= other.y;
        return *this;
    }
    float2& operator*=(const float2 &other) {
        this->x *= other.x;
        this->y *= other.y;
        return *this;
    }
    float2& operator/=(const float2 &other) {
        this->x /= other.x;
        this->y /= other.y;
        return *this;
    }
    float2& operator*=(const float a) {
        this->x *= a;
        this->y *= a;
        return *this;
    }
    float2& operator/=(const float a) {
        this->x /= a;
        this->y /= a;
        return *this;
    }

    [[nodiscard]] float sum() const {
        return this->x + this->y;
    }
    [[nodiscard]] float mag() const {
        return sqrt(this->mag2());
    }
    [[nodiscard]] float mag2() const {
        return this->x * this->x + this->y * this->y;
    }
    [[nodiscard]] float dot(const float2 other) const {
        return this->x * other.x + this->y * other.y;
    }
    [[nodiscard]] float2& normalize() {
        float mag = this->mag();
        if (mag != 0) {
            this->x /= mag;
            this->y /= mag;
        }
        return *this;
    }
    [[nodiscard]] float2 lerp(const float2& other, const float t) const {
        return *this*(1-t) + other*t;
    }
    [[nodiscard]] float2 invert() const {
        return {1/this->x, 1/this->y};
    }
    void clamp(const float min, const float max) {
        if (x < min) x = min;
        if (x > max) x = max;
        if (y < min) y = min;
        if (y > max) y = max;
    }
    void clear() {
        this->x = this->y = 0;
    }
};

#endif //VEC2F_H
