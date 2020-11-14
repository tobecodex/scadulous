#ifndef __METRICS_H
#define __METRICS_H

#include <map>
#include <string>

void dumpMetrics();
std::map<std::string, int> &metrics();

#endif