#include "Metrics.h"

#include <chrono>
#include <iostream>

static std::map<std::string, int> _metrics;

std::map<std::string, int> &metrics()
{
  return _metrics;
}

void dumpMetrics()
{
  static auto start = std::chrono::steady_clock::now();

  auto now = std::chrono::steady_clock::now();
  auto msecs = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
  std::cout << "fps:" << metrics()["frames"] / (float)(msecs.count() / (float)1000) << std::endl;

  for (auto kv : _metrics) {
    std::cout << kv.first  << " " << kv.second << std::endl;
  }

  std::cout << "--" << std::endl;
  start = now;
}

