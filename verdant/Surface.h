#pragma once
#include "MathDefs.h"
#include "Sampler.h"
#include <memory>

namespace verdant {
enum class SurfaceKind { Lambert, Reflect, Refract, Glass, Specular };

// NOTE: to port this code to the lesser GPU programming models (CUDA deals with
// C++ just fine) without any modification, do not use dynamic dispatch. This is
// the reason why this is a discriminated union instead of a virtual base.
class Surface {
public:
  // Creates a Lambertian material with diffuse reflectance c
  static std::shared_ptr<Surface> create_lambert(float3 c);
  static std::shared_ptr<Surface> create_reflect(float3 c, float eta);
  static std::shared_ptr<Surface> create_refract(float3 c, float eta);
  static std::shared_ptr<Surface> create_glass(float3 c, float eta);
  static std::shared_ptr<Surface> create_specular(float3 c, float eta);

  // Both L and V are in BRDF coordinates, with normal being (0, 0, 1)
  float3 f(const float3 &L, const float3 &V) const;

  // Sample an incoming direction
  float3 sample_f(UniformSampler &sampler, const float3 &V, float3 &L,
                  float &pdf) const;

  bool is_delta() const {
    return kind == SurfaceKind::Reflect || kind == SurfaceKind::Refract ||
           kind == SurfaceKind::Glass || kind == SurfaceKind::Specular;
  }

protected:
  float3 sample_f_lambert(UniformSampler &sampler, const float3 &V, float3 &L,
                          float &pdf) const;
  float3 sample_f_reflection(UniformSampler &sampler, const float3 &V,
                             float3 &L, float &pdf) const;
  float3 sample_f_refraction(UniformSampler &sampler, const float3 &V,
                             float3 &L, float &pdf) const;
  float3 sample_f_glass(UniformSampler &sampler, const float3 &V, float3 &L,
                        float &pdf) const;
  float3 sample_f_specular(UniformSampler &sampler, const float3 &V, float3 &L,
                           float &pdf) const;

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
  Surface(SurfaceKind kind) : kind(kind) {}

  SurfaceKind kind;
  float3 c;
  float etaI, etaT;
};
} // namespace verdant
