#ifndef __DEBUG_H
#define __DEBUG_H

enum LogLevel
{
  ERROR = 0,
  WARN = 1,
  INFO = 2,
  DEBUG = 3
};

class Logger
{
private:
  static Logger *_logger;
  enum LogLevel _logLevel;
  
public:

  void error(const char *);
  void warn(const char *);
  void info(const char *);
  void debug(const char *);

  Logger(LogLevel logLevel = LogLevel::WARN);
};

#endif