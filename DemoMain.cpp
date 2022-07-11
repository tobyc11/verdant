#include "HDRImage.h"
#include "Pipeline.h"
#include "TaskWorker.h"
#include <cstdlib>
#include <cstring>
#include <memory>

using namespace verdant;

int main(int argc, char **argv) {
  TaskWorker::init_default_workers();
  int samples = 32;
  // --single x y
  int x, y, single_shot = 0;
  if (argc == 4) {
    if (strcmp(argv[1], "--single") == 0) {
      x = atoi(argv[2]);
      y = atoi(argv[3]);
      single_shot = 1;
      printf("Single shot pixel %d %d\n", x, y);
    }
  } else if (argc >= 2) {
    // First argument is sample rate
    samples = atoi(argv[1]);
    if (!samples) {
      samples = 32;
    }
  }
  printf("Sample count is %d\n", samples);
  PathTracePipeline pipeline(320 * 4, 240 * 4, samples);
  if (single_shot) {
    pipeline.single_pixel(x, y);
  } else {
    pipeline.run();
  }
  TaskWorker::shutdown_default_workers();
  return 0;
}
