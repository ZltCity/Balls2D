#include <fstream>
#include <cstring>

#include "file.h"

size_t _getFileSize(std::fstream &filepath) {
  size_t size = 0;
  
  filepath.seekg(0, std::ios_base::end);
  size = filepath.tellg();
  filepath.seekg(0, std::ios_base::beg);

  return size;
} 

FileDesc loadFile(const std::string &filepath) {
  FileDesc      desc = { std::shared_ptr<uint8_t>(), 0 };
  std::fstream  file;

  file.open(filepath);

  if (!file.is_open())
    return desc;

  size_t size = _getFileSize(file);

  try {
    desc.buff.reset(new uint8_t[size + 1], [](uint8_t *buff) { delete []buff; });
  } catch (std::bad_alloc &) {
    return desc;
  }

  std::memset(desc.buff.get(), 0, size + 1);
  file.read(reinterpret_cast<char *>(desc.buff.get()), size);
  desc.size = size;

  return desc;
}
