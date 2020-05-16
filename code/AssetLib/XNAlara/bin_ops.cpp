// Local Project
#include "bin_ops.hpp"

namespace assimpXnalara {

bool hasTangentVersion(unsigned int verMajor, unsigned int verMinor,
                       bool hasHeader) {
  return hasHeader ? (verMinor <= 12 and verMajor <= 2) : true;
}

bool hasVariableWeight(unsigned int verMajor, unsigned int verMinor,
                       bool hasHeader) {
  return hasHeader ? (verMajor >= 3) : false;
}

} // namespace assimpXnalara
