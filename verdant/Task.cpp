#include "Task.h"

namespace verdant {
Task::Task(std::function<void()> fn) : fn(std::move(fn)) {}

void Task::add_successor(Task *succ) {
  succ->prerequisite_count += 1;
  successors.push_back(succ);
}

bool Task::decrement_successors() {
  bool has_ready = false;
  for (Task *succ : successors) {
    succ->prerequisite_count -= 1;
    if (succ->prerequisite_count == 0) {
      has_ready = true;
    }
  }
  return has_ready;
}

void Task::execute() { fn(); }
} // namespace verdant
