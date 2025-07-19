//
// Created by Andreas Royset on 6/13/25.
//

#ifndef INT2_H
#define INT2_H

#include <cmath>
#include "float2.h"

class int2 {
    public:
    int x;
    int y;

    int2() {
        x = 0;
        y = 0;
    }
    explicit int2(const int x) {
        this->x = x;
        this->y = x;
    }
    int2(const int x, const int y) {
        this->x = x;
        this->y = y;
    }

    bool operator == (const int2 other) const {
        return (x == other.x && y == other.y);
    }
    int2 operator + (const int2 other) const {
        return {x + other.x, y + other.y};
    }
    int2 operator - (const int2 other) const {
        return {x - other.x, y - other.y};
    }
    int2 operator * (const int2 other) const {
        return {x * other.x, y * other.y};
    }
    int2 operator * (const int other) const {
        return {x * other, y * other};
    }
    int2 operator * (const float other) const {
        return {int(float(x) * other), int(float(y) * other)};
    }
    int2 operator / (const int2 other) const {
        return {x / other.x, y / other.y};
    }
    int2 operator / (const int other) const {
        return {x / other, y / other};
    }
    int2 operator += (const int2 other){
        x = x + other.x;
        y = y + other.y;
        return *this;
    }
    int2 operator -= (const int2 other){
        x = x - other.x;
        y = y - other.y;
        return *this;
    }
    int2 operator *= (const int2 other){
        x = x * other.x;
        y = y * other.y;
        return *this;
    }
    int2 operator *= (const int other){
        x = x * other;
        y = y * other;
        return *this;
    }
    int2 operator /= (const int2 other){
        x = x / other.x;
        y = y / other.y;
        return *this;
    }

    explicit operator float2() const {
        return {static_cast<float>(x), static_cast<float>(y)};
    }

    [[nodiscard]] int dot(const int2 other) const {
        return x * other.x + y * other.y;
    }
    [[nodiscard]] int mag2() const {
        return x*x + y*y;
    }
    [[nodiscard]] int mag() const {
        return int(sqrt(this->mag2()));
    }
    int2 normalize() {
        const int mag = this->mag();
        if (mag == 0) return {};
        x /= mag;
        y /= mag;
        return *this;
    }
    int2 perp() {
        const int temp = x;
        x = -y;
        y = temp;
        return *this;
    }
    int2 lerp(const int2 other, const float t) const {
        return (*this)*(1-t) + other*t;
    }

};

#endif //INT2_H
