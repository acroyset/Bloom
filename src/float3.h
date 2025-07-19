//
// Created by Andreas Royset on 4/26/25.
//

#ifndef VEC3F_H
#define VEC3F_H

#include <cmath>

class float3 {
    public:
    float x, y, z;

    float3() {
        x = y = z = 0;
    }
    float3(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    explicit float3(const float x) {
        this->x = x;
        this->y = x;
        this->z = x;
    }

    bool operator==(const float3 &other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    float3 operator+(const float3 &other) const {
        return {this->x + other.x, this->y + other.y, this->z + other.z};
    }
    float3 operator-(const float3 &other) const {
        return {this->x - other.x, this->y - other.y, this->z - other.z};
    }
    float3 operator-() const {
        return {-this->x, -this->y, -this->z};
    }
    float3 operator*(const float3 &other) const {
        return {this->x * other.x, this->y * other.y, this->z * other.z};
    }
    float3 operator/(const float3 &other) const {
        return {this->x / other.x, this->y / other.y, this->z / other.z};
    }
    float3 operator*(const float a) const {
        return {this->x * a, this->y * a, this->z * a};
    }
    float3 operator/(const float a) const {
        return {this->x / a, this->y / a, this->z / a};
    }
    float3& operator+=(const float3 &other) {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;
        return *this;
    }
    float3& operator-=(const float3 &other) {
        this->x -= other.x;
        this->y -= other.y;
        this->z -= other.z;
        return *this;
    }
    float3& operator*=(const float3 &other) {
        this->x *= other.x;
        this->y *= other.y;
        this->z *= other.z;
        return *this;
    }
    float3& operator/=(const float3 &other) {
        this->x /= other.x;
        this->y /= other.y;
        this->z /= other.z;
        return *this;
    }
    float3& operator*=(const float a) {
        this->x *= a;
        this->y *= a;
        this->z *= a;
        return *this;
    }
    float3& operator/=(const float a) {
        this->x /= a;
        this->y /= a;
        this->z /= a;
        return *this;
    }

    [[nodiscard]] float sum() const {
        return this->x + this->y + this->z;
    }
    [[nodiscard]] float mag() const {
        float mag2 = this->mag2();
        if  (mag2 == 0) {
            return 0;
        }
        return sqrt(mag2);
    }
    [[nodiscard]] float mag2() const {
        return this->x * this->x + this->y * this->y + this->z * this->z;
    }
    [[nodiscard]] float dot(const float3 other) const {
        return this->x * other.x + this->y * other.y + this->z * other.z;
    }
    [[nodiscard]] float3& normalize() {
        float mag = this->mag();
        if (mag != 0) {
            this->x /= mag;
            this->y /= mag;
            this->z /= mag;
        }
        return *this;
    }
    [[nodiscard]] float3 lerp(const float3& other, const float t) const {
        return *this*(1-t) + other*t;
    }
    [[nodiscard]] float3 invert() const {
        return {1/this->x, 1/this->y, 1/this->z};
    }
    [[nodiscard]] float3 cross(const float3& other) const {
        return {
        this->y * other.z - this->z * other.y,
        this->z * other.x - this->x * other.z,
        this->x * other.y - this->y * other.x};
    }
    [[nodiscard]] float3 power(const float power) const {
        return {pow(this->x, power), pow(this->y, power), pow(this->z, power)};
    }
    [[nodiscard]] float3 abs() const {
        return {std::abs(this->x), std::abs(this->y), std::abs(this->z)};
    }
    void clamp(const float min, const float max) {
        if (std::isnan(this->x)){this->x = min;};
        if (std::isnan(this->y)){this->y = min;};
        if (std::isnan(this->z)){this->z = min;};
        if (x < min) x = min;
        if (x > max) x = max;
        if (y < min) y = min;
        if (y > max) y = max;
        if (z < min) z = min;
        if (z > max) z = max;
    }
    void clear() {
        this->x = this->y = this->z = 0;
    }
};

#endif //VEC3F_H
