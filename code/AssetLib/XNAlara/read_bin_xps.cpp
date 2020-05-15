// Local Project
#include "read_bin_xps.hpp"
namespace assimpXnalara {

std::vector<std::shared_ptr<aiMaterial>>
readMeshes(std::ifstream &fileStream, std::shared_ptr<XpsHeader> xpsHeaderPtr,
           bool hasBones) {
  bool hasHeader, hasTangent, hasVariableWeights;
  hasHeader = hasTangent = hasVariableWeights = false;
  int verMajor, verMinor;
  std::vector<std::shared_ptr<aiMaterial>> meshVector;
  unsigned int meshCount = readUInt32(fileStream);
  if (xpsHeaderPtr) {
    hasHeader = true;
  }
  verMajor = hasHeader ? xpsHeaderPtr->version_major : 0;
  verMinor = hasHeader ? xpsHeaderPtr->version_minor : 0;

  hasTangent = hasTangentVersion(verMajor, verMinor, hasHeader);
  hasVariableWeights = hasVariableWeights(verMajor, verMinor, hasHeader);

  for (int i = 0; i < meshCount; i++) {
    // Name
    std::string meshName = readFilesString(fileStream);
    if (meshName.length() == 0) {
      meshName = "unnamed";
    }
    std::cout << "Mesh Name:" << meshName << std::endl;
    // uv Count
    unsigned int uvLayerCount = readUInt32(fileStream);
    // Textures
    std::vector<std::shared_ptr<aiMaterial>> textureVector;
    unsigned int textureCount = readUInt32(fileStream);
    for (int j = 0; j < textureCount; j++) {
      std::string textureFile = ntpath.basename(readFilesString(fileStream));
      std::cout << "Texture file:" << textureFile << std::endl;
      unsigned int uvLayerId = readUInt32(fileStream);

      std::shared_ptr<aiMaterial> matPtr = std::make_shared<aiMaterial>();
      aiString texture_path(textureFile);
      matPtr->AddProperty(&texture_path,
                          AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0));
      textureVector.push_back(matPtr);
    }
  }
}

} // namespace assimpXnalara
