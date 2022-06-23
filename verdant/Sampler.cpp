#include "Sampler.h"
#include <random>

namespace {
thread_local std::random_device rd;
thread_local std::mt19937 eng(rd());
} // namespace

namespace verdant {
UniformSampler::UniformSampler() : my_eng(eng) {}
} // namespace verdant
