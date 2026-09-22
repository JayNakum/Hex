#pragma once

#include "./details/matrices/mat2x2.h"
#include "./details/matrices/mat3x3.h"
#include "./details/matrices/mat4x4.h"

#include "./simd/mat2f.h"
#include "./simd/mat3f.h"
#include "./simd/mat4f.h"

typedef mat2x2<float> mat2f;
typedef mat3x3<float> mat3f;
typedef mat4x4<float> mat4f;
