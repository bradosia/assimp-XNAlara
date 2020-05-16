// C++
#include <fstream>
#include <string>

namespace assimpXnalara {

inline char readByte(std::istream &fileStream) {
  char data;
  fileStream.read(reinterpret_cast<char *>(&data), 1);
  return data;
}

inline unsigned short readUShort8(std::istream &fileStream) {
  char data;
  fileStream.read(reinterpret_cast<char *>(&data), 1);
  return static_cast<unsigned char>(data);
}

inline short readInt16(std::istream &fileStream) {
  short data;
  fileStream.read(reinterpret_cast<char *>(&data), 2);
  return data;
}

inline unsigned short readUInt16(std::istream &fileStream) {
  unsigned short data;
  fileStream.read(reinterpret_cast<char *>(&data), 2);
  return data;
}

inline unsigned int readUInt32(std::istream &fileStream) {
  unsigned int data;
  fileStream.read(reinterpret_cast<char *>(&data), 4);
  return data;
}

inline float readFloat32(std::istream &fileStream) {
  float data;
  fileStream.read(reinterpret_cast<char *>(&data), 4);
  return data;
}

bool hasTangentVersion(unsigned int verMajor, unsigned int verMinor,
                       bool hasHeader = true);
bool hasVariableWeight(unsigned int verMajor, unsigned int verMinor,
                       bool hasHeader = true);

} // namespace assimpXnalara
