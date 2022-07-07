#pragma once
#include <memory>
#include <vmmlib/matrix.hpp>

namespace verdant {
using float2 = vmml::Vector<2, float>;
using float3 = vmml::Vector<3, float>;
using float4 = vmml::Vector<4, float>;

using int2 = vmml::Vector<2, int>;
using int3 = vmml::Vector<3, int>;
using int4 = vmml::Vector<4, int>;

using uint2 = vmml::Vector<2, unsigned int>;
using uint3 = vmml::Vector<3, unsigned int>;
using uint4 = vmml::Vector<4, unsigned int>;

using float2x2 = vmml::Matrix<2, 2, float>;
using float2x3 = vmml::Matrix<2, 3, float>;
using float2x4 = vmml::Matrix<2, 4, float>;
using float3x2 = vmml::Matrix<3, 2, float>;
using float3x3 = vmml::Matrix<3, 3, float>;
using float3x4 = vmml::Matrix<3, 4, float>;
using float4x2 = vmml::Matrix<4, 2, float>;
using float4x3 = vmml::Matrix<4, 3, float>;
using float4x4 = vmml::Matrix<4, 4, float>;

class Ray {
public:
  Ray(float3 origin, float3 dir) : origin(origin), dir(dir) {}

  float3 origin;
  float3 dir;
};

// Forward declaration for Intersection members
class Material;

// Intersection is the main interface between Scene and the rest of the ray
// tracer
struct Intersection {
  float t;
  float3 normal;
  std::shared_ptr<Material> material;

  float3x3 make_tangent_basis() const {
    float3 i, j, k;
    k = normal;
    j = float3(0.0f, 1.0f, 0.0f);
    i = j.cross(k);
    i.normalize();
    j = k.cross(i);

    float3x3 tangent_basis;
    tangent_basis.set_column(0, i);
    tangent_basis.set_column(1, j);
    tangent_basis.set_column(2, k);
    return tangent_basis;
  }

  float3 tangent_to_world(const float3 &v) const {
    // Get a vector orthogonal to normal
    return make_tangent_basis() * v;
  }
};
} // namespace verdant
