// Local Project
#include "read_bin_xps.hpp"
namespace assimpXnalara {

char *readFileCStr(std::istream &fileStream) {
  char lengthByte1[1], lengthByte2[1], *str;
  int length1, length2;
  length1 = length2 = 0;
  /* Byte before a string is its length in bytes
   */
  fileStream.read(lengthByte1, 1);
  length1 = static_cast<int>(lengthByte1[0]);
  if (length1 >= LIMIT) {
    fileStream.read(lengthByte2, 1);
    length1 = static_cast<int>(lengthByte1[0]);
  }
  size_t length = (length1 % LIMIT) + (length2 * LIMIT);
  size_t lengthC = length + 1;
  str = new char[lengthC];
  memset(str, 0, lengthC);
  fileStream.read(str, length);
  return str;
}

std::string readFileStr(std::istream &fileStream) {
  char lengthByte1[1], lengthByte2[1], *str;
  int length1, length2;
  length1 = length2 = 0;
  /* Byte before a string is its length in bytes
   */
  fileStream.read(lengthByte1, 1);
  length1 = static_cast<int>(lengthByte1[0]);
  if (length1 >= LIMIT) {
    fileStream.read(lengthByte2, 1);
    length1 = static_cast<int>(lengthByte1[0]);
  }
  size_t length = (length1 % LIMIT) + (length2 * LIMIT);
  size_t lengthC = length + 1;
  str = new char[lengthC];
  memset(str, 0, lengthC);
  fileStream.read(str, length);
  return std::string(str, length);
}

bool findHeader(std::istream &fileStream,
                std::shared_ptr<XpsHeader> xpsHeaderPtr) {
  unsigned int magicNumber;
  fileStream.read(reinterpret_cast<char *>(&magicNumber), 4);
  fileStream.seekg(0);
  if (magicNumber == static_cast<unsigned int>(MAGIC_NUMBER)) {
    return true;
  }
  return false;
}

bool readHeader(std::istream &fileStream,
                std::shared_ptr<XpsHeader> xpsHeaderPtr) {
  /* The debug blocks are line by line because this will allow
   * the line before errors to still print.
   */
#if ASSIMP_XNALARA_READ_BIN_HEADER_DEBUG
  std::cout << "readHeader" << std::endl;
#endif
  xpsHeaderPtr = std::make_shared<XpsHeader>();
  // MagicNumber
  xpsHeaderPtr->magic_number = readUInt32(fileStream);
#if ASSIMP_XNALARA_READ_BIN_HEADER_DEBUG
  std::cout << "magic_number: " << xpsHeaderPtr->magic_number << std::endl;
#endif
  // XPS Version
  xpsHeaderPtr->version_major = readUInt16(fileStream);
#if ASSIMP_XNALARA_READ_BIN_HEADER_DEBUG
  std::cout << "version_major: " << xpsHeaderPtr->version_major << std::endl;
#endif
  xpsHeaderPtr->version_minor = readInt16(fileStream);
#if ASSIMP_XNALARA_READ_BIN_HEADER_DEBUG
  std::cout << "version_minor: " << xpsHeaderPtr->version_minor << std::endl;
#endif
  // XNAaral Name
  xpsHeaderPtr->xna_aral = readFileCStr(fileStream);
#if ASSIMP_XNALARA_READ_BIN_HEADER_DEBUG
  std::cout << "xna_aral: " << xpsHeaderPtr->xna_aral << std::endl;
#endif
  // Settings Length
  xpsHeaderPtr->settingsLen = readUInt32(fileStream);
#if ASSIMP_XNALARA_READ_BIN_HEADER_DEBUG
  std::cout << "settingsLen: " << xpsHeaderPtr->settingsLen << std::endl;
#endif
  // MachineName
  xpsHeaderPtr->machine = readFileCStr(fileStream);
#if ASSIMP_XNALARA_READ_BIN_HEADER_DEBUG
  std::cout << "machine: " << xpsHeaderPtr->machine << std::endl;
#endif
  // UserName
  xpsHeaderPtr->user = readFileCStr(fileStream);
#if ASSIMP_XNALARA_READ_BIN_HEADER_DEBUG
  std::cout << "user: " << xpsHeaderPtr->user << std::endl;
#endif
  // File-->File
  xpsHeaderPtr->files = readFileCStr(fileStream);
#if ASSIMP_XNALARA_READ_BIN_HEADER_DEBUG
  std::cout << "files: " << xpsHeaderPtr->files << std::endl;
#endif
  int streamSettingsEndPos =
      (int)fileStream.tellg() + xpsHeaderPtr->settingsLen * 4;
  if (hasTangentVersion(xpsHeaderPtr->version_major,
                        xpsHeaderPtr->version_minor)) {
    // Old and unsupported version
  } else {
    /* TODO: Do something with this data
     */
    unsigned int hash = readUInt32(fileStream);
    unsigned int items = readUInt32(fileStream);
    for (unsigned int i = 0; i < items; i++) {
      unsigned int optType = readUInt32(fileStream);
      unsigned int optcount = readUInt32(fileStream);
      unsigned int optInfo = readUInt32(fileStream);
    }
  }
  // skip remaining data
  fileStream.seekg(streamSettingsEndPos);
  return true;
}

bool readBones(std::istream &fileStream, aiScene *scenePtr,
               std::shared_ptr<XpsHeader> xpsHeaderPtr,
               BoneMapPtr_t boneNodeMapPtr) {
#if ASSIMP_XNALARA_READ_BIN_BONES_DEBUG
  std::cout << "readBones" << std::endl;
#endif
  unsigned int numBonesTotal = 0;
  numBonesTotal = readUInt32(fileStream);
#if ASSIMP_XNALARA_READ_BIN_BONES_DEBUG
  std::cout << "readBones numBonesTotal=" << numBonesTotal << std::endl;
#endif
  /* set buckets for the maps
   * bones ID is data type short
   */
  std::unordered_map<short, short> parentIdMap(numBonesTotal);
  boneNodeMapPtr->reserve(numBonesTotal);
  // root bone node
  if (!scenePtr->mRootNode) {
    scenePtr->mRootNode = new aiNode;
  }
  /* First Pass
   * Get all the bone data and create a bone node list.
   * Another loop will create the scene node hierarchy.
   * The scene node hierarchy could be constructed in this loop, but that
   * would assume parentNodeId<currentNodeId. Do not assume this.
   */
  short n = static_cast<short>(numBonesTotal);
  for (short boneId = 0; boneId < n; boneId++) {
    BoneNode &boneNode = (*boneNodeMapPtr)[boneId];
    boneNode.bone->mName = readFileCStr(fileStream);
    boneNode.node->mName = boneNode.bone->mName;
    // read bone parent id
    short boneParentId = readInt16(fileStream);
    parentIdMap.insert({boneId, boneParentId});
    // bone coordinates
    aiVector3D boneV3;
    boneV3.x = readFloat32(fileStream);
    boneV3.y = readFloat32(fileStream);
    boneV3.z = readFloat32(fileStream);
    aiMatrix4x4::Translation(boneV3, boneNode.bone->mOffsetMatrix);
#if ASSIMP_XNALARA_READ_BIN_BONES_DEBUG_2
    std::cout << "readBones: boneName=" << boneNode.bone->mName.C_Str()
              << "\nboneId=" << boneId << " boneParentId=" << boneParentId
              << "\ncoords=(" << boneV3.x << "," << boneV3.y << "," << boneV3.z
              << ")" << std::endl;
#endif
  }
  /* Second Pass
   * Create the scene node hierarchy.
   */
  for (short boneId = 0; boneId < n; boneId++) {
    aiNode *nodePtr = boneNodeMapPtr->at(boneId).node;
    short boneParentId = parentIdMap.at(boneId);
    // bones with a parentId less than 0 are root bones
    aiNode *nodeParent = scenePtr->mRootNode;
    if (boneParentId >= 0) {
      nodeParent = boneNodeMapPtr->at(boneParentId).node;
    }
    // add children one at a time
    aiNode *nodeArr[1];
    nodeArr[0] = nodePtr;
    nodeParent->addChildren(1, nodeArr);
  }
  return true;
}

/* Before this was implemented with many inline helper function
 * to read the binary file, but they were pasted into the function.
 * Possibly in the future inline functions will be used.
 */
bool readMeshes(std::istream &fileStream, aiScene *scenePtr,
                std::shared_ptr<XpsHeader> xpsHeaderPtr,
                BoneMapPtr_t boneNodeMapPtr) {
#if ASSIMP_XNALARA_READ_BIN_MESHES_DEBUG
  std::cout << "readMeshes" << std::endl;
#endif
  bool hasHeader, hasTangent, hasVariableWeights;
  hasHeader = hasTangent = hasVariableWeights = false;
  int verMajor, verMinor;
  scenePtr->mNumMeshes = readUInt32(fileStream);
  scenePtr->mMeshes = new aiMesh *[scenePtr->mNumMeshes];
#if ASSIMP_XNALARA_READ_BIN_MESHES_DEBUG
  std::cout << "readMeshes mNumMeshes=" << scenePtr->mNumMeshes << std::endl;
#endif
  if (xpsHeaderPtr) {
    hasHeader = true;
    verMajor = hasHeader ? xpsHeaderPtr->version_major : 0;
    verMinor = hasHeader ? xpsHeaderPtr->version_minor : 0;
    hasTangent = hasTangentVersion(verMajor, verMinor, hasHeader);
    hasVariableWeights = hasVariableWeight(verMajor, verMinor, hasHeader);
  }

  for (unsigned int meshId = 0; meshId < scenePtr->mNumMeshes; meshId++) {
    // create assimp mesh
    scenePtr->mMeshes[meshId] = new aiMesh();
    aiMesh *meshPtr = scenePtr->mMeshes[meshId];
    std::string meshName = readFileCStr(fileStream);
    // Name
    if (meshName.length() == 0) {
      meshName = "unnamed";
    }
#if ASSIMP_XNALARA_READ_BIN_MESHES_DEBUG
    std::cout << "readMeshes mesh#" << meshId << " meshName=" << meshName
              << std::endl;
#endif
    meshPtr->mName = meshName;
    // Local mesh bone node map
    std::unordered_map<short, std::vector<std::pair<unsigned int, float>>>
        boneVertexWeightMap;
    if (boneNodeMapPtr && !boneNodeMapPtr->empty()) {
      size_t boneNum = boneNodeMapPtr->size();
      boneVertexWeightMap.reserve(boneNum);
    }
    // UV Count
    unsigned int uvLayerCount = readUInt32(fileStream);
    // Materials
    scenePtr->mNumMaterials = readUInt32(fileStream);
    scenePtr->mMaterials = new aiMaterial *[scenePtr->mNumMaterials];
#if ASSIMP_XNALARA_READ_BIN_MESHES_DEBUG
    std::cout << "readMeshes mesh#" << meshId
              << " uvLayerCount=" << uvLayerCount
              << " mNumMaterials=" << scenePtr->mNumMaterials << std::endl;
#endif
    for (unsigned int materialId = 0; materialId < scenePtr->mNumMaterials;
         materialId++) {
      std::string textureFile = readFileCStr(fileStream);
      const int uvLayerId = readUInt32(fileStream);
#if ASSIMP_XNALARA_READ_BIN_MESHES_DEBUG_2
      std::cout << "readMeshes mesh#" << meshId << " mat#" << materialId
                << " textureFile=" << textureFile << " uvLayerId=" << uvLayerId
                << std::endl;
#endif
      // create assimp material
      scenePtr->mMaterials[materialId] = new aiMaterial();
      aiString texture_path(textureFile);
      /* Use if-else instead of switch-case
       * because may use more complex criteria fo texture mapping in future
       *
       * Assimp properties are keyed by (MATKEY, TextureType, index).
       * First argument of AddProperty() is the input data.
       */
      if (materialId == 0) {
        // Diffuse
        scenePtr->mMaterials[materialId]->AddProperty(
            &texture_path, _AI_MATKEY_TEXTURE_BASE, aiTextureType_DIFFUSE, 0);
        scenePtr->mMaterials[materialId]->AddProperty(
            &uvLayerId, 1, _AI_MATKEY_MAPPING_BASE, aiTextureType_DIFFUSE, 0);
      } else if (materialId == 1) {
        // Light Map
        scenePtr->mMaterials[materialId]->AddProperty(
            &texture_path, _AI_MATKEY_TEXTURE_BASE, aiTextureType_LIGHTMAP, 0);
        scenePtr->mMaterials[materialId]->AddProperty(
            &uvLayerId, 1, _AI_MATKEY_MAPPING_BASE, aiTextureType_DIFFUSE, 0);
      } else if (materialId == 2) {
        // Normals
        scenePtr->mMaterials[materialId]->AddProperty(
            &texture_path, _AI_MATKEY_TEXTURE_BASE, aiTextureType_NORMALS, 0);
        scenePtr->mMaterials[materialId]->AddProperty(
            &uvLayerId, 1, _AI_MATKEY_MAPPING_BASE, aiTextureType_DIFFUSE, 0);
      } else if (materialId == 3) {
        // Specular
        scenePtr->mMaterials[materialId]->AddProperty(
            &texture_path, _AI_MATKEY_TEXTURE_BASE, aiTextureType_SPECULAR, 0);
        scenePtr->mMaterials[materialId]->AddProperty(
            &uvLayerId, 1, _AI_MATKEY_MAPPING_BASE, aiTextureType_DIFFUSE, 0);
      }
    }

    // Vertices
    meshPtr->mNumVertices = readUInt32(fileStream);
    meshPtr->mVertices = new aiVector3D[meshPtr->mNumVertices];
    meshPtr->mNormals = new aiVector3D[meshPtr->mNumVertices];
    meshPtr->mColors[0] = new aiColor4D[meshPtr->mNumVertices];
#if ASSIMP_XNALARA_READ_BIN_MESHES_DEBUG
    std::cout << "readMeshes mesh#" << meshId
              << " mNumVertices=" << meshPtr->mNumVertices << std::endl;
#endif

    for (unsigned int vertexId = 0; vertexId < meshPtr->mNumVertices;
         vertexId++) {
      // Read Vertices
      meshPtr->mVertices[vertexId].x = readFloat32(fileStream);
      meshPtr->mVertices[vertexId].y = readFloat32(fileStream);
      meshPtr->mVertices[vertexId].z = readFloat32(fileStream);

      // Read Normals
      meshPtr->mNormals[vertexId].x = readFloat32(fileStream);
      meshPtr->mNormals[vertexId].y = readFloat32(fileStream);
      meshPtr->mNormals[vertexId].z = readFloat32(fileStream);

      // Read Vertex Color
      meshPtr->mColors[0][vertexId].r = readUShort8(fileStream) / 255.0;
      meshPtr->mColors[0][vertexId].g = readUShort8(fileStream) / 255.0;
      meshPtr->mColors[0][vertexId].b = readUShort8(fileStream) / 255.0;
      meshPtr->mColors[0][vertexId].a = readUShort8(fileStream) / 255.0;

#if ASSIMP_XNALARA_READ_BIN_MESHES_DEBUG_3
      std::cout << "readMeshes mesh#" << meshId << " vert#" << vertexId
                << " mVertices.x=" << meshPtr->mVertices[vertexId].x
                << " mNormals.x=" << meshPtr->mNormals[vertexId].x
                << " mColors.r=" << meshPtr->mColors[0][vertexId].r
                << std::endl;
#endif

      // UVs
      for (unsigned int uvLayerId = 0; uvLayerId < uvLayerCount; uvLayerId++) {
        meshPtr->mTextureCoords[uvLayerId] =
            new aiVector3D[meshPtr->mNumVertices];
        meshPtr->mTextureCoords[uvLayerId][vertexId].x =
            readFloat32(fileStream);
        meshPtr->mTextureCoords[uvLayerId][vertexId].y =
            readFloat32(fileStream);

        if (hasTangent) {
          // Where do we put tangent data
          // why is it 4D?
          readFloat32(fileStream);
          readFloat32(fileStream);
          readFloat32(fileStream);
          readFloat32(fileStream);
          // does it go in here?
          // meshPtr->mTangents;
          // meshPtr->mBitangents;
        }

#if ASSIMP_XNALARA_READ_BIN_MESHES_DEBUG_3
        std::cout << "readMeshes mesh#" << meshId << " vert#" << vertexId
                  << " uv#" << uvLayerId << " mTextureCoords.x="
                  << meshPtr->mTextureCoords[uvLayerId][vertexId].x
                  << std::endl;
#endif
      }

      /* Bone Weights
       * Only read if bones exist
       */
      if (boneNodeMapPtr && !boneNodeMapPtr->empty()) {
        unsigned int weightsCount = 4;
        if (hasVariableWeights) {
          weightsCount = readInt16(fileStream);
        }
#if ASSIMP_XNALARA_READ_BIN_MESHES_DEBUG_4
        std::cout << "readMeshes mesh#" << meshId << " vert#" << vertexId
                  << " weightsCount=" << weightsCount << std::endl;
#endif
        // create a bone-weight list local to vertex
        std::vector<std::pair<short, float>> weightPairList(weightsCount);
        // get bone id component
        for (std::pair<short, float> &weightPair : weightPairList) {
          weightPair.first = readInt16(fileStream);
        }
        // get weight component
        for (std::pair<short, float> &weightPair : weightPairList) {
          weightPair.second = readFloat32(fileStream);
        }
        /* Temporary map bone to vertex-weight list
         * This is done before mapping to the actual assimp bone
         * because the exact number of vertex-weights are not known
         * yet for each bone.
         */
        for (std::pair<short, float> weightPair : weightPairList) {
          boneVertexWeightMap[weightPair.first].push_back(
              {vertexId, weightPair.second});
#if ASSIMP_XNALARA_READ_BIN_MESHES_DEBUG_4
          std::cout << "readMeshes mesh#" << meshId << " vert#" << vertexId
                    << " bone#" << weightPair.first
                    << " weight=" << weightPair.second << std::endl;
#endif
        }
      }
    }

    /* Vertices already read.
     * Map assimp bone to to vertex-weight list.
     * Iterating through maps is slower than an array, but
     * gaps in bone id might exist so this way is safer.
     */
    for (auto weightPair : boneVertexWeightMap) {
      /* Do not assume bone id exists.
       * Ignore vertex weights for bones that were never declared at the
       * beginning.
       */
      auto boneNodeResult = boneNodeMapPtr->find(weightPair.first);
      if (boneNodeResult == boneNodeMapPtr->end()) {
        continue;
      }
      size_t vertexWeightNum = weightPair.second.size();
      boneNodeResult->second.bone->mNumWeights = vertexWeightNum;
      boneNodeResult->second.bone->mWeights =
          new aiVertexWeight[vertexWeightNum];
      unsigned int i = 0;
      for (std::pair<unsigned int, float> vertexWeightPair :
           weightPair.second) {
        boneNodeResult->second.bone->mWeights[i].mVertexId =
            vertexWeightPair.first;
        boneNodeResult->second.bone->mWeights[i].mWeight =
            vertexWeightPair.second;
#if ASSIMP_XNALARA_READ_BIN_MESHES_DEBUG_5
        std::cout << "readMeshes mesh#" << meshId
                  << " bone=" << boneNodeResult->second.bone->mName.C_Str()
                  << " mVertexId="
                  << boneNodeResult->second.bone->mWeights[i].mVertexId
                  << " mWeight="
                  << boneNodeResult->second.bone->mWeights[i].mWeight
                  << std::endl;
#endif
      }
    }

    // Faces
    meshPtr->mNumFaces = readUInt32(fileStream);
    meshPtr->mFaces = new aiFace[meshPtr->mNumFaces];
#if ASSIMP_XNALARA_READ_BIN_MESHES_DEBUG
    std::cout << "readMeshes mesh#" << meshId
              << " mNumFaces=" << meshPtr->mNumFaces << std::endl;
#endif
    for (unsigned int j = 0; j < meshPtr->mNumFaces; j++) {
      // Read Vertices
      meshPtr->mFaces[j].mNumIndices = 3;
      meshPtr->mFaces[j].mIndices = new unsigned int[3];
      meshPtr->mFaces[j].mIndices[0] = readUInt32(fileStream);
      meshPtr->mFaces[j].mIndices[1] = readUInt32(fileStream);
      meshPtr->mFaces[j].mIndices[2] = readUInt32(fileStream);
    }
  }
  return true;
}

bool readXnalaraModelBinary(std::istream &fileStream, aiScene *scenePtr) {
#if ASSIMP_XNALARA_READ_BIN_DEBUG
  std::cout << "readXnalaraModelBinary" << std::endl;
#endif
  std::shared_ptr<XpsHeader> xpsHeaderPtr;
  if (findHeader(fileStream, xpsHeaderPtr)) {
#if ASSIMP_XNALARA_READ_BIN_HEADER_DEBUG
    std::cout << "readXnalaraModelBinary: file header found." << std::endl;
#endif
    readHeader(fileStream, xpsHeaderPtr);
  } else {
#if ASSIMP_XNALARA_READ_BIN_HEADER_DEBUG
    std::cout << "readXnalaraModelBinary: no file header found." << std::endl;
#endif
  }
  BoneMapPtr_t boneNodeMapPtr = std::make_shared<BoneMap_t>();
  readBones(fileStream, scenePtr, xpsHeaderPtr, boneNodeMapPtr);
  if (boneNodeMapPtr->empty()) {
#if ASSIMP_XNALARA_READ_BIN_DEBUG
    std::cout << "readXnalaraModelBinary: No bones found." << std::endl;
#endif
  }
  if (!readMeshes(fileStream, scenePtr, xpsHeaderPtr, boneNodeMapPtr)) {
    std::cout << "ERROR: Can't read meshes." << std::endl;
    return false;
  }
  return true;
}

} // namespace assimpXnalara
