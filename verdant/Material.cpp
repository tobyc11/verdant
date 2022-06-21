#include "Material.h"
#include "BRDF.h"
#include <memory>

namespace verdant {
std::shared_ptr<Material> Material::create_lambert(float3 c) {
  std::shared_ptr<Material> m(new Material(MaterialKind::Lambert));
  m->c = c;
  return m;
}

float3 Material::f() const { return F_lambert(c); }
} // namespace verdant