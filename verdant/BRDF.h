#pragma once
#include "MathDefs.h"
#include <algorithm>
#include <cmath>
#include <utility>

namespace verdant {
inline float3 F_lambert(float3 c) { return c / M_PI; }

inline float Snell_cosThetaT(float cosThetaI, float etaI, float etaT,
                             bool *is_total_internal_reflection) {
  float sinThetaI = std::sqrt(std::max(0.0f, 1 - cosThetaI * cosThetaI));
  float sinThetaT = sinThetaI * etaI / etaT;
  if (sinThetaT >= 1.0f) {
    // Handle total internal reflection
    *is_total_internal_reflection = true;
    return std::nanf("total internal reflection");
  }

  *is_total_internal_reflection = false;
  float cosThetaT = std::sqrt(std::max(0.0f, 1 - sinThetaT * sinThetaT));
  return cosThetaT;
}

// Fresnel equation for dielectric for unpolarized light. Sign of cosThetaI
// determins whether the incident light comes from outside or inside the medium
inline float Fr_dielectric(float cosThetaI, float etaI, float etaT) {
  cosThetaI = std::clamp(cosThetaI, -1.0f, 1.0f);

  // Check inside or outside
  bool entering = cosThetaI > 0.f;
  if (!entering) {
    std::swap(etaI, etaT);
    cosThetaI = std::abs(cosThetaI);
  }

  // Apply Snell's law
  bool is_total_internal_reflection;
  float cosThetaT =
      Snell_cosThetaT(cosThetaI, etaI, etaT, &is_total_internal_reflection);
  if (is_total_internal_reflection) {
    return 1.0f;
  }

  // R := reflectance
  // T := transmittance is 1-R
  // Rparl and Rperp are for s and p polarized light
  float Rparl = (etaT * cosThetaI - etaI * cosThetaT) /
                (etaT * cosThetaI + etaI * cosThetaT);
  float Rperp = (etaI * cosThetaI - etaT * cosThetaT) /
                (etaI * cosThetaI + etaT * cosThetaT);
  return (Rparl * Rparl + Rperp * Rperp) / 2;
}

// Schlick's approximation of the Fresnel term
inline float Fr_Schlick(float cosThetaI, float etaI, float etaT) {
  float rtR0 = (etaI - etaT) / (etaI + etaT);
  float R0 = rtR0 * rtR0;
  return R0 + (1 - R0) * powf(1 - cosThetaI, 5.0f);
}
} // namespace verdant
