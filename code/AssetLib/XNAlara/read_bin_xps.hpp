// C++
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

/* assimp 5.0.1
 * License: BSD
 */
#include <assimp/color4.h>
#include <assimp/material.h>
#include <assimp/quaternion.h>
#include <assimp/vector2.h>
#include <assimp/vector3.h>
#include <assimp/mesh.h>

// Local Project
#include "bin_ops.hpp"
#include "read_ascii_xps.hpp"
#include "xps_const.hpp"

namespace assimpXnalara {

class XpsHeader {
public:
  int magic_number, version_major, version_minor, xna_aral, settingsLen;
  std::string machine, user, files, settings, pose;
};

aiColor4t<char> readVertexColor(std::ifstream &fileStream);
aiVector2t<char> readUvVert(std::ifstream &fileStream);
aiVector3t<char> readXYZ(std::ifstream &fileStream);
aiQuaterniont<float> read4Float(std::ifstream &fileStream);
std::string readFilesString(std::ifstream &fileStream);

std::vector<std::shared_ptr<aiMaterial>>
readMeshes(std::ifstream &fileStream, std::shared_ptr<XpsHeader> xpsHeaderPtr,
           bool hasBones);

} // namespace assimpXnalara
