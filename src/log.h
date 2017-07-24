#pragma once

#include <fstream>
#include <string>

enum {
  LOG_INFO  = 0,
  LOG_WARN  = 1,
  LOG_CRIT  = 2
};

class Log {
public:
  static Log &instance();

  void print(int type, const std::string &message);
  bool reopen(const std::string &filename);

private:
  Log();
  ~Log();

  Log(const Log &)  = delete;
  Log(const Log &&) = delete;

  Log &operator=(const Log &)   = delete;
  Log &operator=(const Log &&)  = delete;

  std::fstream file;
};
