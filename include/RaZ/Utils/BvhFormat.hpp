#pragma once

#ifndef RAZ_BVHFORMAT_HPP
#define RAZ_BVHFORMAT_HPP

#include "RaZ/Animation/Skeleton.hpp"

#include <vector>

namespace Raz {

class FilePath;

class BvhFormat {
public:
  explicit BvhFormat(const FilePath& filePath) { import(filePath); }

  void import(const FilePath& filePath);

  Skeleton skeleton {};
};

} // namespace Raz

#endif // RAZ_BVHFORMAT_HPP
