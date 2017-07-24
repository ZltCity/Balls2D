#include <ctime>

#include "log.h"

Log::Log() {
  this->reopen(std::string("log.txt"));
}

Log::~Log() {
  this->file.close();
}

Log &Log::instance() {
  static Log log;

  return log;
}

void Log::print(int type, const std::string &message)
{
  auto printTime = [](std::fstream &file) {
    time_t   t        = std::time(nullptr);
    tm      *l        = std::localtime(&t);
    char     buff[24] = { 0x00 };

    std::strftime(buff, sizeof(buff), "%d.%m.%Y %H:%M:%S", l);
    file << buff;
  };

  const char *strType[] = {
    "INFO",
    "WARN",
    "CRIT"
  };
  
  if (type < 0 || type >= sizeof(strType) / sizeof(char *))
    type = 0;

  printTime(file);
  file << '\t' << strType[type] << '\t' << message << std::endl;
  file.flush();
}

bool Log::reopen(const std::string &filename)
{
  this->file.open(filename, std::ios_base::out | std::ios_base::trunc);

  if (!this->file.is_open())
    return false;

  return true;
}

