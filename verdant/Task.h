#pragma once
#include <functional>
#include <vector>

namespace verdant {
class Task {
public:
  Task(std::function<void()> fn);

  bool is_ready() const { return prerequisite_count == 0; }

  void add_successor(Task *succ);

  bool decrement_successors();

  void execute();

  // For debugging
  unsigned int get_prereq_count() const { return prerequisite_count; }

private:
  std::function<void()> fn;
  std::vector<Task *> successors;
  unsigned int prerequisite_count = 0;
};
} // namespace verdant
