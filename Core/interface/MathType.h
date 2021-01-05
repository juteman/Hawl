#pragma once

#include "Eigen/Core"
#include "Eigen/Geometry"

namespace Hawl {
    using vec2f = Eigen::Vector2f;
    using vec3f = Eigen::Vector3f;
    using mat4f = Eigen::Matrix4f;


    // Add * / operator for vector, which always use in color compute
    inline vec2f operator*(const vec2f &a, const vec2f &b) { return vec2f(a.x() * b.x(), a.y() * b.y()); }

    inline vec3f operator*(const vec3f &a, const vec3f &b) {
        return vec3f(a.x() * b.x(), a.y() * b.y(), a.z() * b.z());
    }

    inline vec2f operator/(const vec2f &a, const vec2f &b) { return vec2f(a.x() / b.x(), a.y() / b.y()); }

    inline vec3f operator/(const vec3f &a, const vec3f &b) {
        return vec3f(a.x() / b.x(), a.y() / b.y(), a.z() / b.z());
    }
}
