/**
 *  Copyright 2020-2021 Zhang QiuLiang (juteman). All rights reserved.
 *
 *  This file is a part of Hawl
 *  see(https://github.com/juteman/Hawl)
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#pragma once

#include "Eigen/Core"
#include "Eigen/Geometry"

namespace Hawl
{
using vec2f = Eigen::Vector2f;
using vec3f = Eigen::Vector3f;
using mat4f = Eigen::Matrix4f;


// Add * / operator for vector, which always use in color compute
inline vec2f operator*(const vec2f &a, const vec2f &b)
{
    return vec2f(a.x() * b.x(), a.y() * b.y());
}

inline vec3f operator*(const vec3f &a, const vec3f &b)
{
    return vec3f(a.x() * b.x(), a.y() * b.y(), a.z() * b.z());
}

inline vec2f operator/(const vec2f &a, const vec2f &b)
{
    return vec2f(a.x() / b.x(), a.y() / b.y());
}

inline vec3f operator/(const vec3f &a, const vec3f &b)
{
    return vec3f(a.x() / b.x(), a.y() / b.y(), a.z() / b.z());
}
}
