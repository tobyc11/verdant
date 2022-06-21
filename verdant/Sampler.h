#pragma once
#include "MathDefs.h"
#include <random>
#include <tuple>

namespace verdant {
class UniformSampler {
public:
  UniformSampler() : eng(rd()) {}

  std::tuple<float, float> sample() {
    std::uniform_real_distribution<> dist(0, 1);
    return {pdf(), dist(eng)};
  }

  float pdf() const { return 1.0f; }

private:
  std::random_device rd;
  std::mt19937 eng;
};

class UniformHemisphereDistribution {
public:
  // Returns the pdf and the unit vector representing the direction
  std::tuple<float, float3> sample(UniformSampler &base_sampler) const {
    auto [u0_pdf, u0] = base_sampler.sample();
    auto [u1_pdf, u1] = base_sampler.sample();

    float phi = 2 * M_PI * u0;
    float theta = acos(u1);

    float x = sin(theta) * cos(phi);
    float y = sin(theta) * sin(phi);
    float z = cos(theta);

    float pdf = 1.0f / (2 * M_PI);
    return {pdf, {x, y, z}};
  }

  float pdf(const float3 &value) const { return 1.0f / (2 * M_PI); }
};

class CosineWeightedHemisphereDistribution {
public:
  // Returns the pdf and the unit vector representing the direction
  std::tuple<float, float3> sample(UniformSampler &base_sampler) const {
    auto [u0_pdf, u0] = base_sampler.sample();
    auto [u1_pdf, u1] = base_sampler.sample();

    float phi = 2 * M_PI * u0;
    float theta = acos(sqrt(u1));

    float x = sin(theta) * cos(phi);
    float y = sin(theta) * sin(phi);
    float z = cos(theta);

    float pdf = z / M_PI;
    return {pdf, {x, y, z}};
  }

  float pdf(const float3 &value) const {
    // Assert value is a normal vector
    return value.z() / M_PI;
  }
};
} // namespace verdant
