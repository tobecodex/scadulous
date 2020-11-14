#include "Logger.h"

#include <iostream>

Logger *Logger::_logger = new Logger();

Logger::Logger(LogLevel logLevel) : _logLevel(logLevel)
{
  if (_logger) {
    delete _logger;
  }
  _logger = this;
}

void Logger::info(const char *msg)
{
  std::cerr << "INFO: " << msg << std::endl;
}

void Logger::debug(const char *msg)
{
  std::cerr << "DEBUG:" << msg << std::endl;
}

void Logger::warn(const char *msg)
{
  std::cerr << "WARN:" << msg << std::endl;
}

void Logger::error(const char *msg)
{
  std::cerr << "ERROR:" << msg << std::endl;
}
