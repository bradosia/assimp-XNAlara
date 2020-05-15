// C++
#include <fstream>
#include <string>
namespace assimpXnalara {

char readByte(std::ifstream& fileStream);
short readInt16(std::ifstream& fileStream);
unsigned int readUInt32(std::ifstream& fileStream);
std::string readSingle(std::ifstream& fileStream, unsigned int length);
bool hasHeader(std::string fileformat = ".xps");
bool hasTangentVersion(unsigned int verMajor, unsigned int verMinor,
                       bool hasHeader = true);
bool hasVariableWeights(unsigned int verMajor, unsigned int verMinor,
                        bool hasHeader = true);

} // namespace assimpXnalara
