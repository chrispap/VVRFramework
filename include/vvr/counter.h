#pragma once

#include <chrono>
#include <iostream>

struct ScopedCounter {
  ScopedCounter(const char *name) : name(name) {
    t0 = std::chrono::high_resolution_clock::now();
  }

  ~ScopedCounter() {
    using std::chrono::duration_cast;
    using std::chrono::microseconds;
    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt = duration_cast<microseconds>(t1 - t0).count();
    std::cout << name << " took " << dt / 1000. << " ms" << std::endl;
  }

private:
  std::chrono::high_resolution_clock::time_point t0;
  const char *name;
};
