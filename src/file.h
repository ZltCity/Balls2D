#pragma once

#include <memory>
#include <cstddef>
#include <cstdint>
#include <string>

class File {
public:
  File() = default;

  operator bool() const;

  bool load(const std::string &filepath);

  uint8_t *getPtr();
  const uint8_t *getPtr() const;

  size_t getSize() const;

  template<typename T_PTR>
  T_PTR *getPtr() {
    return reinterpret_cast<T_PTR *>(this->getPtr());
  }

private:
  std::shared_ptr<uint8_t>  buff;
  size_t                    size;
};

