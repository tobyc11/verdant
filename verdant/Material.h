#pragma once
#include "MathDefs.h"
#include "Sampler.h"
#include <memory>

namespace verdant {
enum class MaterialKind { Lambert, Reflect, Refract, Glass, Specular };

class Material {
public:
  // Creates a Lambertian material with diffuse reflectance c
  static std::shared_ptr<Material> create_lambert(float3 c);
  static std::shared_ptr<Material> create_reflect(float3 c, float eta);
  static std::shared_ptr<Material> create_refract(float3 c, float eta);
  static std::shared_ptr<Material> create_glass(float3 c, float eta);
  static std::shared_ptr<Material> create_specular(float3 c, float eta);

  // Both L and V are in BRDF coordinates, with normal being (0, 0, 1)
  float3 f(const float3 &L, const float3 &V) const;

  // Sample an incoming direction
  float3 sample_f(UniformSampler &sampler, const float3 &V, float3 &L,
                  float &pdf) const;

  bool is_delta() const {
    return kind == MaterialKind::Reflect || kind == MaterialKind::Refract ||
           kind == MaterialKind::Glass || kind == MaterialKind::Specular;
  }

protected:
  float3 sample_f_lambert(UniformSampler &sampler, const float3 &V, float3 &L,
                          float &pdf) const;
  float3 sample_f_reflection(UniformSampler &sampler, const float3 &V,
                             float3 &L, float &pdf) const;
  float3 sample_f_refraction(UniformSampler &sampler, const float3 &V,
                             float3 &L, float &pdf) const;

  template <typename T1, typename T2>
  float3 sum_f(UniformSampler &sampler, const float3 &V, float3 &L, float &pdf,
               T1 fn1, T2 fn2) const {
    auto [_pdf, roll] = sampler.sample();
    const float k = 0.5f;
    if (roll < k) {
      pdf = k;
      return (this->*fn1)(sampler, V, L, pdf);
    } else {
      pdf = 1 - k;
      return (this->*fn2)(sampler, V, L, pdf);
    }
  }

private:
  Material(MaterialKind kind) : kind(kind) {}

  MaterialKind kind;
  float3 c;
  float etaI, etaT;
};
} // namespace verdant
