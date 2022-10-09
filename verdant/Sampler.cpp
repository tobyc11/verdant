#include "Sampler.h"
#include <mutex>
#include <random>

namespace {
std::mutex rd_mutex;
std::random_device rd;
thread_local verdant::UniformSampler uniform_sampler_per_thread;
} // namespace

namespace verdant {
UniformSampler::UniformSampler() {
  std::unique_lock<std::mutex> lk(rd_mutex);
  random_engine = std::mt19937(rd());
}

UniformSampler &UniformSampler::per_thread() {
  return uniform_sampler_per_thread;
}
} // namespace verdant
