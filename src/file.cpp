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

File::operator bool() const {
  return this->size > 0;
}

bool File::load(const std::string &filepath) {
  std::fstream file;

  file.open(filepath);

  if (!file.is_open())
    return false;

  size_t size = _getFileSize(file);

  try {
    this->buff.reset(new uint8_t[size + 1], [](uint8_t *buff) { delete []buff; });
  } catch (std::bad_alloc &) {
    return false;
  }

  std::memset(this->buff.get(), 0, size + 1);
  file.read(reinterpret_cast<char *>(this->buff.get()), size);
  this->size = size;

  return true;
}

uint8_t *File::getPtr() {
  return this->buff.get();
}

const uint8_t *File::getPtr() const {
  return this->buff.get();
}

size_t File::getSize() const {
  return this->size;
}
