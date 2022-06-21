#pragma once
#include "MathDefs.h"
#include <memory>

namespace verdant {
enum class MaterialKind { Lambert };

class Material {
public:
  // Creates a Lambertian material with diffuse reflectance c
  static std::shared_ptr<Material> create_lambert(float3 c);

  float3 f() const;

private:
  Material(MaterialKind kind) : kind(kind) {}

  MaterialKind kind;
  float3 c;
};
} // namespace verdant
