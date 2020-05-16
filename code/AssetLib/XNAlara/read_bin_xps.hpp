#ifndef XNALARA_READ_BIN_H
#define XNALARA_READ_BIN_H

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
#include <assimp/mesh.h>
#include <assimp/quaternion.h>
#include <assimp/scene.h>
#include <assimp/vector2.h>
#include <assimp/vector3.h>

// Local Project
#include "bin_ops.hpp"
#include "read_ascii_xps.hpp"
#include "xps_const.hpp"

namespace assimpXnalara {

class XpsHeader {
public:
  int magic_number, version_major, version_minor, settingsLen;
  std::string xna_aral, machine, user, files, settings, pose;
};

/* Smart pointers not used because
 * assimp will eventually take ownership.
 * Prevent accidental deletion of the pointers.
 */
class BoneNode {
public:
  BoneNode() {
    node = new aiNode;
    bone = new aiBone;
  }
  /* Do not delete pointers
   * They will be moved to assimp
   */
  ~BoneNode() {}
  aiNode *node;
  aiBone *bone;
};

using BoneMap_t = std::unordered_map<short, BoneNode>;
using BoneMapPtr_t = std::shared_ptr<BoneMap_t>;

char *readFileCStr(std::istream &fileStream);
std::string readFileStr(std::istream &fileStream);
bool findHeader(std::istream &fileStream,
                std::shared_ptr<XpsHeader> xpsHeaderPtr);
bool readHeader(std::istream &fileStream,
                std::shared_ptr<XpsHeader> xpsHeaderPtr);

/* readBones
 * Bones hierarchy is so strange to handle in assimp.
 * It seems the Collada Exporter assumes scene nodes are used for hierarchy
 * and node names are the same as bone names to form artificial bone nodes.
 * Bones for assimp will be handled this way:
 * 1. create temporary list of bones.
 * 2. create node hierarchy mimicking bone hierarchy.
 * 3. store bone information in mesh where referenced.
 * 4. bones that don't have any vertex weights won't be stored anywhere,
 * but will remain in the node hierarchy.
 */
bool readBones(std::istream &, aiScene *, std::shared_ptr<XpsHeader>,
               BoneMapPtr_t);
bool readMeshes(std::istream &, aiScene *, std::shared_ptr<XpsHeader>,
                BoneMapPtr_t);
/* Use std::istream rather than std::ifstream because data may be streamed
 * from memory potentially.
 */
bool readXnalaraModelBinary(std::istream &, aiScene *);

} // namespace assimpXnalara

#endif
