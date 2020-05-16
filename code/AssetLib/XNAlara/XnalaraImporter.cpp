// Local Project
#include "XnalaraImporter.hpp"

namespace assimpXnalara {

const aiImporterDesc desc = {"Xnalara Importer",
                             "",
                             "",
                             "Imports xnalara files.",
                             aiImporterFlags_SupportTextFlavour,
                             0,
                             0,
                             0,
                             0,
                             "xps mesh mesh.ascii"};

XnalaraImporter::XnalaraImporter() {}
XnalaraImporter::~XnalaraImporter() {}

bool XnalaraImporter::CanRead(const std::string &pFile,
                              Assimp::IOSystem *pIOHandler,
                              bool checkSig) const {
#if ASSIMP_XNALARA_IMPORTER_DEBUG
  std::cout << "XnalaraImporter::CanRead" << std::endl;
#endif
  if (!checkSig) {
    return SimpleExtensionCheck(pFile, "xps") ||
           SimpleExtensionCheck(pFile, "mesh") ||
           SimpleExtensionCheck(pFile, "mesh.ascii");
  } else {
    return BaseImporter::CheckMagicToken(pIOHandler, pFile,
                                         (const void *)&MAGIC_NUMBER, 1);
  }
}

const aiImporterDesc *XnalaraImporter::GetInfo() const { return &desc; }

void XnalaraImporter::InternReadFile(const std::string &pFile, aiScene *pScene,
                                     Assimp::IOSystem *pIOHandler) {
#if ASSIMP_XNALARA_IMPORTER_DEBUG
  std::cout << "XnalaraImporter::InternReadFile(" << pFile << ")" << std::endl;
#endif
  /* Assimp::IOSystem seems like a less standard filesystem and ifstream
   * std::filesystem and std::ifstream preferred
   */
  (void)pIOHandler; // mark as unused for compiler
  std::ifstream fileStream;
  fileStream.open(pFile, std::ios::in | std::ios::binary);
  if (!fileStream) {
    throw DeadlyImportError("Failed to open file " + pFile + ".");
  }
  /* Does assimp guarentee an allocated scene pointer?
   */
  if (!pScene) {
#if ASSIMP_XNALARA_IMPORTER_DEBUG
    std::cout << "XnalaraImporter::InternReadFile new aiScene" << std::endl;
#endif
    pScene = new aiScene;
  }
  readXnalaraModelBinary(fileStream, pScene);
}

} // namespace assimpXnalara
