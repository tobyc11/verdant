#include "Pipeline.h"

using namespace verdant;

int main(int argc, char **argv) {
  PathTracePipeline pipeline(320 * 4, 240 * 4, 32);
  pipeline.run();
  return 0;
}
