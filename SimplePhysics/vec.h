#pragma once
#include <cmath>

/**
 * @brief vec2 / vec3 / vec4: POD (Plain Old Data)
 *
 * Only 'compound assignments(+,-,*,/)' are member function.
 * The others (unary, binary, scalar mul/div, dot, cross, length, normalize) are all non-member
 * function.
 */

namespace math
{
    struct vec2
    {
        float x{}, y{};

        // compound assignments
        vec2& operator+=(const vec2& r) noexcept
        {
            x += r.x;
            y += r.y;
            return *this;
        }
        vec2& operator-=(const vec2& r) noexcept
        {
            x -= r.x;
            y -= r.y;
            return *this;
        }
        vec2& operator*=(float s) noexcept
        {
            x *= s;
            y *= s;
            return *this;
        }
        vec2& operator/=(float s) noexcept
        {
            x /= s;
            y /= s;
            return *this;
        }
    };
    struct vec3
    {
        float x{}, y{}, z{};

        // compound assignments
        vec3& operator+=(const vec3& r) noexcept
        {
            x += r.x;
            y += r.y;
            z += r.z;
            return *this;
        }
        vec3& operator-=(const vec3& r) noexcept
        {
            x -= r.x;
            y -= r.y;
            z -= r.z;
            return *this;
        }
        vec3& operator*=(float s) noexcept
        {
            x *= s;
            y *= s;
            z *= s;
            return *this;
        }
        vec3& operator/=(float s) noexcept
        {
            x /= s;
            y /= s;
            z /= s;
            return *this;
        }
    };
    struct vec4
    {
        float x{}, y{}, z{}, w{};

        // compound assignments
        vec4& operator+=(const vec4& r) noexcept
        {
            x += r.x;
            y += r.y;
            z += r.z;
            w += r.w;
            return *this;
        }
        vec4& operator-=(const vec4& r) noexcept
        {
            x -= r.x;
            y -= r.y;
            z -= r.z;
            w -= r.w;
            return *this;
        }
        vec4& operator*=(float s) noexcept
        {
            x *= s;
            y *= s;
            z *= s;
            w *= s;
            return *this;
        }
        vec4& operator/=(float s) noexcept
        {
            x /= s;
            y /= s;
            z /= s;
            w /= s;
            return *this;
        }
    };

    // ----------------------------------
    // Unary operators
    // ----------------------------------
    inline vec2 operator-(vec2 v) { return { -v.x, -v.y }; }
    inline vec3 operator-(vec3 v) { return { -v.x, -v.y, -v.z }; }
    inline vec4 operator-(vec4 v) { return { -v.x, -v.y, -v.z, -v.w }; }

    // ----------------------------------
    // Binary operators
    // ----------------------------------
    inline vec2 operator+(vec2 a, vec2 b) { return { a.x + b.x, a.y + b.y }; }
    inline vec2 operator-(vec2 a, vec2 b) { return { a.x - b.x, a.y - b.y }; }
    inline vec3 operator+(vec3 a, vec3 b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
    inline vec3 operator-(vec3 a, vec3 b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
    inline vec4 operator+(vec4 a, vec4 b) { return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
    inline vec4 operator-(vec4 a, vec4 b) { return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }

    // ----------------------------------
    // Scalar multiply / divide (with symmetry support)
    // ----------------------------------
    inline vec2 operator*(float s, vec2 v) { return { v.x * s, v.y * s }; }
    inline vec2 operator*(vec2 v, float s) { return { v.x * s, v.y * s }; }
    inline vec2 operator/(vec2 v, float s) { return { v.x / s, v.y / s }; }

    inline vec3 operator*(float s, vec3 v) { return { v.x * s, v.y * s, v.z * s }; }
    inline vec3 operator*(vec3 v, float s) { return { v.x * s, v.y * s, v.z * s }; }
    inline vec3 operator/(vec3 v, float s) { return { v.x / s, v.y / s, v.z / s }; }

    inline vec4 operator*(float s, vec4 v) { return { v.x * s, v.y * s, v.z * s, v.w * s }; }
    inline vec4 operator*(vec4 v, float s) { return { v.x * s, v.y * s, v.z * s, v.w * s }; }
    inline vec4 operator/(vec4 v, float s) { return { v.x / s, v.y / s, v.z / s, v.w / s }; }

    // ----------------------------------
    // dot product
    // ----------------------------------
    inline float dot(const vec2& a, const vec2& b) noexcept { return a.x * b.x + a.y * b.y; }
    inline float dot(const vec3& a, const vec3& b) noexcept
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    inline float dot(const vec4& a, const vec4& b) noexcept
    {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    // ----------------------------------
    // cross product (vec3)
    // ----------------------------------
    inline vec3 cross(vec3 a, vec3 b)
    {
        return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
    }

    // ----------------------------------
    // normalize
    // ----------------------------------
    inline vec3 normalize(vec3 v)
    {
        float l = std::sqrt(dot(v, v));
        return l > 0.0f ? (v * (1.0f / l)) : v;
    }

    // ----------------------------------
    // length
    // ----------------------------------
    inline float length2(const vec2& v) noexcept { return dot(v, v); };
    inline float length2(const vec3& v) noexcept { return dot(v, v); };
    inline float length2(const vec4& v) noexcept { return dot(v, v); };

    inline float length(const vec2& v) noexcept { return std::sqrt(dot(v, v)); };
    inline float length(const vec3& v) noexcept { return std::sqrt(dot(v, v)); };
    inline float length(const vec4& v) noexcept { return std::sqrt(dot(v, v)); };

    // ----------------------------------
    // zero check
    // ----------------------------------
    inline bool is_near_zero(const vec2& v, float eps = 1e-8f) noexcept
    {
        return (std::fabs(v.x) < eps && std::fabs(v.y) < eps);
    }
    inline bool is_near_zero(const vec3& v, float eps = 1e-8f) noexcept
    {
        return (std::fabs(v.x) < eps && std::fabs(v.y) < eps && std::fabs(v.z) < eps);
    }
    inline bool is_near_zero(const vec4& v, float eps = 1e-8f) noexcept
    {
        return (std::fabs(v.x) < eps && std::fabs(v.y) < eps && std::fabs(v.z) < eps &&
            std::fabs(v.w) < eps);
    }
} // namespace math
