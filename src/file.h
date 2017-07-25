#pragma once

#include <memory>
#include <cstddef>
#include <cstdint>
#include <string>

struct FileDesc {
  std::shared_ptr<uint8_t> buff;
  size_t size;
};

FileDesc loadFile(const std::string &filepath);
